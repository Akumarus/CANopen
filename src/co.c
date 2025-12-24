#include "co.h"
#include "nmt.h"
#include "pdo.h"
#include "port.h"
#include "sdo.h"

static co_res_t is_callback_register(co_obj_t *canopen, uint32_t id);
static co_msg_type_t canopen_msg_type_from_id(uint32_t id);
static void init_nodes(co_obj_t *canopen);
static void init_filters(co_obj_t *canopen);
static void init_fifo(co_obj_t *canopen);
static void init_callbacks(co_obj_t *canopen);

co_res_t co_init(co_obj_t *co, co_role_t role, uint8_t node_id, uint32_t ide)
{
    assert(co != NULL);
    assert((role == CANOPEN_CLIENT) || (role == CANOPEN_SERVER));
    assert(node_id < 128);
    assert(ide == COB_ID_STD);

    co->ide = ide;
    co->role = role;
    co->node_id = node_id;
    co->info.bank_count = 0;
    co->info.status.all = 0;
    co->info.callbacks_count = 0;

    init_fifo(co);
    init_nodes(co);
    init_filters(co);
    init_callbacks(co);
    port_can_init();

    return CANOPEN_OK;
}

co_res_t co_config_node_id(co_obj_t *co, uint8_t node_id)
{
    assert(co != NULL);
    assert(node_id < 128);

    // TODO быстрый перебор
    for (uint8_t i = 0; i < NODES_COUNT; i++) {
        if (co->node[i].id == 0xFF) {
            co->node[i].id = node_id;
            return CANOPEN_OK;
        }
    }
    return CANOPEN_ERROR; // TODO Добавить статус
}

co_res_t co_process_msg_tx(co_obj_t *co)
{
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

co_res_t co_process_msg_rx(co_obj_t *co)
{
    assert(co != NULL);

    while (!fifo_is_empty(&co->fifo_rx)) {
        co_msg_t msg = {0};
        if (fifo_pop(&co->fifo_rx, &msg) != FIFO_OK)
            return CANOPEN_ERROR;

        // // TODO Нужно вынести в отдельную функцию
        for (uint8_t i = 0; i < MAX_CALLBACKS; i++) {
            if ((co->callbacks[i].id == msg.id) && (co->callbacks[i].callback != NULL)) {
                co->callbacks[i].callback(&msg);
            }
        }

        switch (msg.type) {
        case TYPE_SDO_TX:
            co_srv_proc_sdo(co, &msg);
            break;
        case TYPE_SDO_RX:
            co_cli_proc_sdo(co, &msg);
            break;
        case TYPE_PDO1_TX:
        case TYPE_PDO2_TX:
        case TYPE_PDO3_TX:
        case TYPE_PDO4_TX:
        case TYPE_PDO1_RX:
        case TYPE_PDO2_RX:
        case TYPE_PDO3_RX:
        case TYPE_PDO4_RX:
            break;
        case TYPE_NMT:
            co_srv_proc_nmt(co, &msg);
            break;
        case TYPE_SYNC:
            break;
        case TYPE_EMCY:
            break;
        case TYPE_HEARTBEAT:
            co_cli_proc_heartbeat(co, &msg);
            break;
        default:
            break;
        }
    }

    return CANOPEN_OK;
}

co_res_t co_process_time(co_obj_t *co)
{
    assert(co != NULL);

    uint32_t elapsed = 0;
    uint32_t current_time = port_get_timestamp() - co->timestamp;
    for (uint8_t i = 0; i < NODES_COUNT; i++) {
        co_node_t *node = &co->node[i];

        if (node->id == 0xFF) // TODO задачть через дефайн или переменную
            break;

        // для каждой ноды нужно расчитать время с последнего сообщения для всех сообщений
        // elapsed = co_get_elapsed_time(current_time, node->sdo_timestamp);
        // if (elapsed >= 1000) // TODO задачть через дефайн или переменную
        // {
        //     node->sdo_timestamp = 0;
        //     return CANOPEN_ERROR;
        // }
    }

    return CANOPEN_OK;
}

// co_res_t co_get_elapsed_time(uint32_t time, uint32_t last_time)
// {
//     return (time >= last_time) ? time - last_time : (UINT32_MAX - last_time) + time + 1;
// }

co_res_t canopen_config_callback(co_obj_t *canopen, uint32_t id, uint8_t fifo, co_hdl_t callback)
{
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

co_res_t canopen_send_msg_to_fifo_rx(co_obj_t *canopen, co_msg_t *msg)
{
    assert(canopen != NULL);
    assert(msg != NULL);

    if (fifo_push(&canopen->fifo_rx, msg) != FIFO_OK)
        return CANOPEN_ERROR;

    return CANOPEN_OK;
}

co_res_t canopen_get_msg_from_handler(co_msg_t *msg, uint32_t fifo)
{
    assert(msg != NULL);
    assert((fifo == COB_RX_FIFO0) || (fifo == COB_RX_FIFO1));

    if (port_can_receive_message(&msg->id, msg->frame.row, &msg->dlc, fifo)) {
        msg->type = canopen_msg_type_from_id(msg->id);
    }
    return CANOPEN_OK;
}

uint8_t canopen_get_node_id(co_msg_t *msg)
{
    assert(msg != NULL);

    uint32_t cobid = msg->id;
    return cobid & 0x7F;
}

co_node_t *get_node_index(co_obj_t *canopen, uint8_t node_id)
{
    for (uint8_t i = 0; i < NODES_COUNT; i++) {
        if (canopen->node[i].id == node_id)
            return &canopen->node[i];
    }
    return NULL;
}

static co_res_t is_callback_register(co_obj_t *canopen, uint32_t id)
{
    for (uint8_t i = 0; i < canopen->info.callbacks_count; i++) {
        if (canopen->callbacks[i].id == id && canopen->callbacks[i].callback == NULL)
            return CANOPEN_ERROR;
    }
    return CANOPEN_OK;
}

static void init_callbacks(co_obj_t *canopen)
{
    for (uint8_t i = 0; i < MAX_CALLBACKS; i++) {
        canopen->callbacks[i].id = 0;
        canopen->callbacks[i].callback = NULL;
    }
}

static void init_nodes(co_obj_t *canopen)
{
    for (uint8_t i = 0; i < NODES_COUNT; i++) {
        canopen->node[i].id = 0xFF;
        // canopen->node[i].status.all = 0;
        // canopen->node[i].sdo_timestamp = 0;
        // canopen->node[i].pdo_timestamp = 0;
        // canopen->node[i].nmt_timestamp = 0;
    }
}

static void init_filters(co_obj_t *co)
{
    for (uint8_t i = 0; i < MAX_BANK_COUNT; i++)
        co->banks[i].fifo = 0xFF;
}

static void init_fifo(co_obj_t *canopen)
{
    fifo_config_t fifo_tx_config = {.message = canopen->buffer_tx, .size = CAN_FIFO_SIZE};
    fifo_config_t fifo_rx_config = {.message = canopen->buffer_rx, .size = CAN_FIFO_SIZE};
    fifo_init(&canopen->fifo_tx, fifo_tx_config);
    fifo_init(&canopen->fifo_rx, fifo_rx_config);
}

static co_msg_type_t canopen_msg_type_from_id(uint32_t id)
{
    switch (id) {
    case 0x000:
        return TYPE_NMT;
    case 0x080:
        return TYPE_SYNC;
    case 0x100:
        return TYPE_TIMESTAMP;
    case 0x7E4:
        return TYPE_LSS_RX;
    case 0x7E5:
        return TYPE_LSS_TX;
    }

    // uint8_t node_id = id & 0x07F;
    uint16_t base_id = id & 0x780;

    switch (base_id) {
    case 0x080:
        if (id >= 0x081 && id <= 0x0FF)
            return TYPE_EMCY;
        break;
    case 0x180:
        return TYPE_PDO1_TX;
    case 0x200:
        return TYPE_PDO1_RX;
    case 0x280:
        return TYPE_PDO1_TX;
    case 0x300:
        return TYPE_PDO1_RX;
    case 0x380:
        return TYPE_PDO1_TX;
    case 0x400:
        return TYPE_PDO1_RX;
    case 0x480:
        return TYPE_PDO1_TX;
    case 0x500:
        return TYPE_PDO1_RX;
    case 0x580:
        return TYPE_SDO_RX;
    case 0x600:
        return TYPE_SDO_TX;
    case 0x700:
        return TYPE_HEARTBEAT;
    }
    return TYPE_UNKNOWN;
}
