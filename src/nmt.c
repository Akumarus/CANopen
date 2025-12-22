#include "nmt.h"
#include "fifo.h"

#define NMT_SIZE 2
#define BOOTUP_CMD 0x05
#define HEARTBEAT_CMD 0x00

canopen_state_t canopen_config_nmt(canopen_t *canopen, canopen_msg_t *msg, uint8_t node_id)
{
    assert(canopen != NULL);
    assert(node_id < 128);

    memset(msg, 0, sizeof(canopen_msg_t));
    msg->id = NMT + node_id; // 0 + nodeid
    msg->dlc = NMT_SIZE;     // 2
    msg->type = TYPE_NMT;
    msg->node = get_node_index(canopen, msg->id);
    // msg->node->nmt_state = (canopen->role == CANOPEN_CLIENT) ? NMT_STATE_OPERATIONAL : NMT_STATE_BOOTUP;

    return CANOPEN_OK;
}

canopen_state_t canopen_server_process_nmt(canopen_t *canopen, uint8_t cmd)
{
    assert(canopen != NULL);
    assert(canopen->role != CANOPEN_CLIENT);

    switch (cmd)
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

canopen_state_t canopen_server_transmite_heartbeat(canopen_t *canopen)
{
    assert(canopen != NULL);

    if (canopen->role != CANOPEN_SERVER)
        return CANOPEN_ERROR;

    canopen_msg_t msg;
    memset(&msg, 0, sizeof(canopen_msg_t));
    msg.id = HEARTBIT + canopen->node_id;
    msg.dlc = 1;
    msg.type = TYPE_HEARTBEAT;
    msg.frame.nmt.cmd = canopen->nmt_state;
    fifo_state_t fifo_state = fifo_push(&canopen->fifo_tx, &msg);
    if (fifo_state == FIFO_FULL)
        return CANOPEN_ERROR;

    return CANOPEN_OK;
}

canopen_state_t canopen_transmite_nmt(canopen_t *canopen, canopen_msg_t *msg, canopen_nmt_cmd_t cmd)
{
    assert(canopen != NULL);
    assert(msg != NULL);

    if (canopen->role != CANOPEN_CLIENT)
        return CANOPEN_ERROR;

    msg->frame.nmt.cmd = cmd;
    fifo_state_t fifo_state = fifo_push(&canopen->fifo_tx, msg);
    if (fifo_state == FIFO_FULL)
        return CANOPEN_ERROR;

    return CANOPEN_OK;
}

canopen_state_t canopen_process(canopen_t *canopen, canopen_msg_t *msg)
{
    assert(canopen != NULL);
    assert(msg != NULL);

    if (canopen->role == CANOPEN_CLIENT)
        return CANOPEN_OK;

    if (msg->frame.nmt.node_id != canopen->node_id && msg->frame.nmt.node_id != 0)
        return CANOPEN_OK;

    switch (msg->frame.nmt.cmd)
    {
    case NMT_CS_START_REMOTE_NODE:
        break;
    case NMT_CS_STOP_REMOTE_NODE:
        break;
    case NMT_CS_ENTER_PRE_OPERATION:
        break;
    case NMT_CS_RESET_NODE:
        break;
    case NMT_CS_RESET_COMMUNICATION:
        break;
    default:
        return CANOPEN_ERROR;
    }

    return CANOPEN_OK;
}
