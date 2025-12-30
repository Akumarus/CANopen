#include "co.h"
#include "nmt.h"
#include "pdo.h"
#include "port.h"
#include "sdo.h"

static co_res_t is_callback_register(co_obj_t *canopen, uint32_t id);
static void co_init_nodes(co_obj_t *co);
static void co_init_filters(co_obj_t *canopen);
static void co_init_fifo(co_obj_t *canopen);
static void co_init_callbacks(co_obj_t *canopen);
static void co_check_timeout(co_timeout_t *cob, uint32_t time);

co_res_t co_init(co_obj_t *co, co_role_t role, uint8_t node_id, uint32_t ide) {
    assert(co != NULL);
    assert((role == CANOPEN_CLIENT) || (role == CANOPEN_SERVER));
    assert(node_id < 128);
    assert(ide == COB_ID_STD);

    co->ide = ide;
    co->role = role;
    co->node_id = node_id;
    co->node_count = 0;
    co->info.bank_count = 0;
    co->info.status.all = 0;
    co->info.callbacks_count = 0;

    co_init_fifo(co);
    co_init_nodes(co);
    co_init_filters(co);
    co_init_callbacks(co);
    port_can_init();

    return CANOPEN_OK;
}

co_res_t co_config_node_id(co_obj_t *co, uint8_t node_id) {
    assert(co != NULL);
    assert(node_id < 128);

    // TODO быстрый перебор
    for (uint8_t i = 0; i < NODES_COUNT; i++) {
        if (co->node[i].id == 0xFF) {
            co->node[i].id = node_id;
            co->node_count++;
            return CANOPEN_OK;
        }
    }
    return CANOPEN_ERROR; // TODO Добавить статус
}

co_res_t co_process_msg_tx(co_obj_t *co) {
    assert(co != NULL);

    co_msg_t msg = {0};
    while (!fifo_is_empty(&co->fifo_tx)) {
        if (port_get_free_mailboxes() == 0) // TODO Другой статус
            return CANOPEN_ERROR;

        if (fifo_pop(&co->fifo_tx, &msg) == FIFO_OK) {
            port_can_send(msg.id, COB_RTR_DATA, co->ide, msg.dlc, msg.frame.row);
        }
    }

    return CANOPEN_OK;
}

co_res_t co_process_msg_rx(co_obj_t *co) {
    assert(co != NULL);

    while (!fifo_is_empty(&co->fifo_rx)) {
        co_msg_t msg = {0};
        if (fifo_pop(&co->fifo_rx, &msg) != FIFO_OK)
            return CANOPEN_ERROR;

        uint32_t current_time = port_get_timestamp();
        co_node_t *node = co_get_node_obj(co, msg.id);
        switch (msg.type) {
        case TYPE_SDO_TX:
            co_server_process_sdo(co, &msg);
            node->sdo.time = current_time;
            break;
        case TYPE_SDO_RX:
            co_client_process_sdo(co, &msg);
            node->sdo.time = current_time;
            break;
        case TYPE_PDO1_TX:
        case TYPE_PDO2_TX:
        case TYPE_PDO3_TX:
        case TYPE_PDO4_TX:
        case TYPE_PDO1_RX:
        case TYPE_PDO2_RX:
        case TYPE_PDO3_RX:
        case TYPE_PDO4_RX:
            node->pdo.time = current_time;
            break;
        case TYPE_NMT:
            co_srv_proc_nmt(co, &msg);
            break;
        case TYPE_SYNC:
            break;
        // case TYPE_EMCY:
        //     break;
        case TYPE_HEARTBEAT:
            // co_cli_proc_heartbeat(co, &msg);
            node->heartbeat.time = current_time;
            break;
        default:
            break;
        }

        // // TODO Нужно вынести в отдельную функцию
        // for (uint8_t i = 0; i < MAX_CALLBACKS; i++) {
        //     if ((co->callbacks[i].id == msg.id) && (co->callbacks[i].callback != NULL)) {
        //         co->callbacks[i].callback(&msg);
        //     }
        // }
    }

    return CANOPEN_OK;
}

co_res_t co_process_time(co_obj_t *co) {
    assert(co != NULL);

    uint32_t current_time = port_get_timestamp() - co->timestamp;
    for (uint8_t i = 0; i < co->node_count; i++) {
        co_node_t *node = &co->node[i];

        if (node->id == 0xFF)
            break;

        /** Проверяем на связь с услом сети по pdo/sdo/nmt */
        co_check_timeout(&node->pdo, current_time);
        co_check_timeout(&node->sdo, current_time);
        co_check_timeout(&node->heartbeat, current_time);
    }

    return CANOPEN_OK;
}

static void co_check_timeout(co_timeout_t *cob, uint32_t time) {
    uint32_t elapsed = (time >= cob->time) ? time - cob->time : (UINT32_MAX - cob->time) + time + 1;
    cob->online = (elapsed > cob->timeout) ? true : false;
}

co_res_t canopen_config_callback(co_obj_t *canopen, uint32_t id, uint8_t fifo, co_hdl_t callback) {
    assert(canopen != NULL);
    assert((fifo == COB_RX_FIFO0) || (fifo == COB_RX_FIFO1));
    assert(callback != NULL);
    assert(canopen->info.callbacks_count < MAX_CALLBACKS);

    if (is_callback_register(canopen, id))
        return CANOPEN_ERROR;

    canopen->callbacks[canopen->info.callbacks_count].id = id;
    canopen->callbacks[canopen->info.callbacks_count].callback = callback;
    canopen->info.callbacks_count++;

    if (co_cnf_filter_list_16b(canopen->banks, id, fifo) != CANOPEN_OK)
        return CANOPEN_ERROR;

    return CANOPEN_OK;
}

co_res_t canopen_send_msg_to_fifo_rx(co_obj_t *canopen, co_msg_t *msg) {
    assert(canopen != NULL);
    assert(msg != NULL);

    if (fifo_push(&canopen->fifo_rx, msg) != FIFO_OK)
        return CANOPEN_ERROR;

    return CANOPEN_OK;
}

co_res_t canopen_get_msg_from_handler(co_msg_t *msg, uint32_t fifo) {
    assert(msg != NULL);
    assert((fifo == COB_RX_FIFO0) || (fifo == COB_RX_FIFO1));

    port_can_receive_message(&msg->id, msg->frame.row, &msg->dlc, fifo);
    msg->type = msg->id & 0x780;
    return CANOPEN_OK;
}

uint8_t canopen_get_node_id(co_msg_t *msg) {
    assert(msg != NULL);

    uint32_t cobid = msg->id;
    return cobid & 0x7F;
}

co_node_t *co_get_node_obj(co_obj_t *co, uint8_t node_id) {
    for (uint8_t i = 0; i < NODES_COUNT; i++) {
        if (co->node[i].id == node_id)
            return &co->node[i];
    }
    return NULL;
}

static co_res_t is_callback_register(co_obj_t *canopen, uint32_t id) {
    for (uint8_t i = 0; i < canopen->info.callbacks_count; i++) {
        if (canopen->callbacks[i].id == id && canopen->callbacks[i].callback == NULL)
            return CANOPEN_ERROR;
    }
    return CANOPEN_OK;
}

static void co_init_callbacks(co_obj_t *canopen) {
    for (uint8_t i = 0; i < MAX_CALLBACKS; i++) {
        canopen->callbacks[i].id = 0;
        canopen->callbacks[i].callback = NULL;
    }
}

static void co_init_nodes(co_obj_t *co) {
    for (uint8_t i = 0; i < co->node_count; i++) {
        co->node[i].id = 0xFF;
        memset(&co->node[i].pdo, 0, sizeof(co_timeout_t));
        memset(&co->node[i].sdo, 0, sizeof(co_timeout_t));
        memset(&co->node[i].heartbeat, 0, sizeof(co_timeout_t));
    }
}

static void co_init_filters(co_obj_t *co) {
    for (uint8_t i = 0; i < MAX_BANK_COUNT; i++)
        co->banks[i].fifo = 0xFF;
}

static void co_init_fifo(co_obj_t *canopen) {
    fifo_config_t fifo_tx_config = {.message = canopen->buffer_tx, .size = CAN_FIFO_SIZE};
    fifo_config_t fifo_rx_config = {.message = canopen->buffer_rx, .size = CAN_FIFO_SIZE};
    fifo_init(&canopen->fifo_tx, fifo_tx_config);
    fifo_init(&canopen->fifo_rx, fifo_rx_config);
}