#include "nmt.h"
#include "fifo.h"

#define NMT_SIZE 2
#define BOOTUP_CMD 0x05
#define HEARTBEAT_CMD 0x00

co_res_t co_server_process_nmt(co_obj_t *co, co_msg_t *msg)
{
    assert(co != NULL);
    assert(co->role != CANOPEN_CLIENT);

    uint8_t node_id = canopen_get_node_id(msg);
    if ((node_id != co->node_id) && (node_id != 0))
        return CANOPEN_ERROR; // TODO Нужно вернуть, что фильтр неправильно
                              // сконфигурирован или может это норма

    switch (msg->frame.nmt.cmd) {
    case NMT_CS_START:
        if (co->nmt_state == NMT_STATE_PRE_OPERATIONAL ||
            co->nmt_state == NMT_STATE_STOPPED) {
            co->nmt_state = NMT_STATE_OPERATIONAL;
            co_nmt_send_heartbeat(co);
        }
        break;
    case NMT_CS_STOP:
        if (co->nmt_state == NMT_STATE_OPERATIONAL ||
            co->nmt_state == NMT_STATE_PRE_OPERATIONAL) {
            co->nmt_state = NMT_STATE_STOPPED;
            co_nmt_send_heartbeat(co);
        }
        break;
    case NMT_CS_PREOP:
        if (co->nmt_state == NMT_STATE_OPERATIONAL ||
            co->nmt_state == NMT_STATE_STOPPED) {
            co->nmt_state = NMT_STATE_PRE_OPERATIONAL;
            co_nmt_send_heartbeat(co);
        }
        break;
    case NMT_CS_RESET:
        co->nmt_state = NMT_CS_RESET;
        // Логика сброса узла
        co_nmt_send_heartbeat(co);
        break;

    case NMT_CS_COM_RESET:
        co->nmt_state = NMT_CS_COM_RESET;
        // Логика сброса настроек сети
        // canopen_tran
        co_nmt_send_heartbeat(co);
        break;
    default:
        return CANOPEN_ERROR;
    }

    return CANOPEN_OK;
}

co_res_t co_nmt_send_bootup(co_obj_t *co)
{
    assert(co != NULL);
    assert(co->role == CANOPEN_SERVER);

    co_msg_t msg;
    memset(&msg, 0, sizeof(co_msg_t));
    msg.id = HEARTBEAT + co->node_id;
    msg.dlc = 1;
    msg.type = TYPE_NMT;
    msg.frame.nmt.cmd = BOOTUP_CMD; // TODO чему должно быть равно
    co->nmt_state = NMT_STATE_PRE_OPERATIONAL;
    fifo_state_t fifo_state = fifo_push(&co->fifo_tx, &msg);
    return (fifo_state == FIFO_OK) ? CANOPEN_OK : CANOPEN_ERROR;
}

co_res_t co_nmt_send_heartbeat(co_obj_t *co)
{
    assert(co != NULL);
    assert(co->role == CANOPEN_SERVER);

    if (!co->heartbeat_enable || co->nmt_state == NMT_STATE_BOOTUP ||
        co->nmt_state == NMT_STATE_RESETING)
        return CANOPEN_OK;

    co_msg_t msg;
    memset(&msg, 0, sizeof(co_msg_t));
    msg.id = HEARTBEAT + co->node_id;
    msg.dlc = 1;
    msg.type = TYPE_HEARTBEAT;
    msg.frame.nmt.cmd = co->nmt_state;
    fifo_state_t fifo_state = fifo_push(&co->fifo_tx, &msg);
    return (fifo_state == FIFO_OK) ? CANOPEN_OK : CANOPEN_ERROR;
}

co_res_t canopen_server_timeout(co_obj_t *canopen, uint32_t current_time)
{
    assert(canopen != NULL);
    assert(canopen->role == CANOPEN_SERVER);

    if (canopen->nmt_state == NMT_STATE_BOOTUP) {
        co_nmt_send_bootup(canopen);
        return CANOPEN_OK;
    }

    if (canopen->heartbeat_enable && canopen->nmt_state != NMT_STATE_BOOTUP &&
        canopen->nmt_state != NMT_STATE_RESETING) {
        uint32_t elapsed = current_time - canopen->last_heartbeat_time;
        if (elapsed > canopen->heartbeat_interval_ms)
            co_nmt_send_heartbeat(canopen);
    }

    if (canopen->nmt_state == NMT_STATE_RESETING) {
        // TODO Что-то делаем
    }
    return CANOPEN_OK;
}

co_res_t co_nmt_send_cmd(co_obj_t *co, uint8_t node_id, co_nmt_cmd_t cmd)
{
    assert(co != NULL);
    assert(co->role == CANOPEN_CLIENT);

    co_msg_t msg;
    memset(&msg, 0, sizeof(co_msg_t));
    msg.id = NMT;
    msg.dlc = 2;
    msg.type = TYPE_NMT;
    msg.frame.nmt.cmd = cmd;
    msg.frame.nmt.node_id = node_id;

    if (((cmd == NMT_CS_RESET) || (cmd == NMT_CS_COM_RESET)) && node_id != 0) {
        co_node_t *node = get_node_index(co, node_id);
        if (node)
            node->online = false;
    }

    fifo_state_t fifo_state = fifo_push(&co->fifo_tx, &msg);
    return (fifo_state == FIFO_OK) ? CANOPEN_OK : CANOPEN_ERROR;
}

co_res_t canopen_client_process_heartbeat(co_obj_t *canopen, co_msg_t *msg)
{
    assert(canopen != NULL);
    assert(msg != NULL);
    assert(canopen->role == CANOPEN_CLIENT);

    uint8_t node_id = msg->id - HEARTBEAT;
    co_node_t *node = get_node_index(canopen, node_id);
    if (node == NULL) {
        // TODO Можно самим добавить узел сети
        return CANOPEN_ERROR;
    }

    switch (msg->frame.nmt.cmd) {
    case NMT_STATE_BOOTUP:
    case NMT_STATE_STOPPED:
    case NMT_STATE_RESETING:
    case NMT_STATE_OPERATIONAL:
    case NMT_STATE_PRE_OPERATIONAL:
        node->nmt_state = msg->frame.nmt.cmd;
        break;

    default:
        return CANOPEN_ERROR;
    }
    node->online = true;
    node->last_heartbeat_time =
        canopen->timestamp; // TODO Проверить, то ли время
    return CANOPEN_OK;
}

co_res_t canopen_client_timeout(co_obj_t *canopen, uint32_t current_time)
{
    assert(canopen != NULL);
    assert(canopen->role == CANOPEN_CLIENT);

    for (uint8_t i = 0; i < canopen->node_count; i++) {
        co_node_t *node = &canopen->node[i];
        if (node->online && node->heartbeat_timeout > 0) {
            uint32_t time_since_last = current_time - node->last_heartbeat_time;
            if (time_since_last > node->heartbeat_timeout)
                node->online = false;
        }
    }
    return CANOPEN_OK;
}
