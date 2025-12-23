#include "nmt.h"
#include "fifo.h"

#define NMT_SIZE 2
#define BOOTUP_CMD 0x05
#define HEARTBEAT_CMD 0x00

canopen_state_t canopen_server_process_nmt(canopen_t *canopen, canopen_msg_t *msg)
{
    assert(canopen != NULL);
    assert(canopen->role != CANOPEN_CLIENT);

    uint8_t node_id = canopen_get_node_id(msg);
    if ((node_id != canopen->node_id) && (node_id != 0))
        return CANOPEN_ERROR; // TODO Нужно вернуть, что фильтр неправильно сконфигурирован или может это норма

    switch (msg->frame.nmt.cmd)
    {
    case NMT_CS_START_REMOTE_NODE:
        if (canopen->nmt_state == NMT_STATE_PRE_OPERATIONAL ||
            canopen->nmt_state == NMT_STATE_STOPPED)
        {
            canopen->nmt_state = NMT_STATE_OPERATIONAL;
            canopen_server_transmite_heartbeat(canopen);
        }
        break;
    case NMT_CS_STOP_REMOTE_NODE:
        if (canopen->nmt_state == NMT_STATE_OPERATIONAL ||
            canopen->nmt_state == NMT_STATE_PRE_OPERATIONAL)
        {
            canopen->nmt_state = NMT_STATE_STOPPED;
            canopen_server_transmite_heartbeat(canopen);
        }
        break;
    case NMT_CS_ENTER_PRE_OPERATION:
        if (canopen->nmt_state == NMT_STATE_OPERATIONAL ||
            canopen->nmt_state == NMT_STATE_STOPPED)
        {
            canopen->nmt_state = NMT_STATE_PRE_OPERATIONAL;
            canopen_server_transmite_heartbeat(canopen);
        }
        break;
    case NMT_CS_RESET_NODE:
        canopen->nmt_state = NMT_CS_RESET_NODE;
        // Логика сброса узла
        canopen_server_transmite_heartbeat(canopen);
        break;

    case NMT_CS_RESET_COMMUNICATION:
        canopen->nmt_state = NMT_CS_RESET_COMMUNICATION;
        // Логика сброса настроек сети
        // canopen_tran
        canopen_server_transmite_heartbeat(canopen);
        break;
    default:
        return CANOPEN_ERROR;
    }

    return CANOPEN_OK;
}

canopen_state_t canopen_server_transmite_bootup(canopen_t *canopen)
{
    assert(canopen != NULL);
    assert(canopen->role == CANOPEN_SERVER);

    canopen_msg_t msg;
    memset(&msg, 0, sizeof(canopen_msg_t));
    msg.id = HEARTBEAT + canopen->node_id;
    msg.dlc = 1;
    msg.type = TYPE_NMT;
    msg.frame.nmt.cmd = BOOTUP_CMD; // TODO чему должно быть равно
    canopen->nmt_state = NMT_STATE_PRE_OPERATIONAL;
    fifo_state_t fifo_state = fifo_push(&canopen->fifo_tx, &msg);
    return (fifo_state == FIFO_OK) ? CANOPEN_OK : CANOPEN_ERROR;
}

canopen_state_t canopen_server_transmite_heartbeat(canopen_t *canopen)
{
    assert(canopen != NULL);
    assert(canopen->role == CANOPEN_SERVER);

    if (!canopen->heartbeat_enable ||
        canopen->nmt_state == NMT_STATE_BOOTUP ||
        canopen->nmt_state == NMT_STATE_RESETING)
        return CANOPEN_OK;

    canopen_msg_t msg;
    memset(&msg, 0, sizeof(canopen_msg_t));
    msg.id = HEARTBEAT + canopen->node_id;
    msg.dlc = 1;
    msg.type = TYPE_HEARTBEAT;
    msg.frame.nmt.cmd = canopen->nmt_state;
    fifo_state_t fifo_state = fifo_push(&canopen->fifo_tx, &msg);
    return (fifo_state == FIFO_OK) ? CANOPEN_OK : CANOPEN_ERROR;
}

canopen_state_t canopen_server_timeout(canopen_t *canopen, uint32_t current_time)
{
    assert(canopen != NULL);
    assert(canopen->role == CANOPEN_SERVER);

    if (canopen->nmt_state == NMT_STATE_BOOTUP)
    {
        canopen_server_transmite_bootup(canopen);
        return CANOPEN_OK;
    }

    if (canopen->heartbeat_enable &&
        canopen->nmt_state != NMT_STATE_BOOTUP &&
        canopen->nmt_state != NMT_STATE_RESETING)
    {
        uint32_t elapsed = current_time - canopen->last_heartbeat_time;
        if (elapsed > canopen->heartbeat_interval_ms)
            canopen_server_transmite_heartbeat(canopen);
    }

    if (canopen->nmt_state == NMT_STATE_RESETING)
    {
        // TODO Что-то делаем
    }
    return CANOPEN_OK;
}

canopen_state_t canopen_client_transmite_nmt_cmd(canopen_t *canopen, uint8_t node_id, canopen_nmt_cmd_t cmd)
{
    assert(canopen != NULL);
    assert(canopen->role == CANOPEN_CLIENT);

    canopen_msg_t msg;
    memset(&msg, 0, sizeof(canopen_msg_t));
    msg.id = NMT;
    msg.dlc = 2;
    msg.type = TYPE_NMT;
    msg.frame.nmt.cmd = cmd;
    msg.frame.nmt.node_id = node_id;

    if (((cmd == NMT_CS_RESET_NODE) || (cmd == NMT_CS_RESET_COMMUNICATION)) && node_id != 0)
    {
        canopen_node_t *node = get_node_index(canopen, node_id);
        if (node)
            node->online = false;
    }

    fifo_state_t fifo_state = fifo_push(&canopen->fifo_tx, &msg);
    return (fifo_state == FIFO_OK) ? CANOPEN_OK : CANOPEN_ERROR;
}

canopen_state_t canopen_client_process_heartbeat(canopen_t *canopen, canopen_msg_t *msg)
{
    assert(canopen != NULL);
    assert(msg != NULL);
    assert(canopen->role == CANOPEN_CLIENT);

    uint8_t node_id = msg->id - HEARTBEAT;
    canopen_node_t *node = get_node_index(canopen, node_id);
    if (node == NULL)
    {
        // TODO Можно самим добавить узел сети
        return CANOPEN_ERROR;
    }

    switch (msg->frame.nmt.cmd)
    {
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
    node->last_heartbeat_time = canopen->timestamp; // TODO Проверить, то ли время
    return CANOPEN_OK;
}

canopen_state_t canopen_client_timeout(canopen_t *canopen, uint32_t current_time)
{
    assert(canopen != NULL);
    assert(canopen->role == CANOPEN_CLIENT);

    for (uint8_t i = 0; i < canopen->node_count; i++)
    {
        canopen_node_t *node = &canopen->node[i];
        if (node->online && node->heartbeat_timeout > 0)
        {
            uint32_t time_since_last = current_time - node->last_heartbeat_time;
            if (time_since_last > node->heartbeat_timeout)
                node->online = false;
        }
    }
    return CANOPEN_OK;
}
