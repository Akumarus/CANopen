#include "nmt.h"
#include "fifo.h"

#define NMT_SIZE 2

canopen_state_t canopen_config_nmt(canopen_t *canopen, canopen_msg_t *msg, uint8_t node_id)
{
    assert(canopen != NULL);
    assert(node_id < 128);

    memset(msg, 0, sizeof(canopen_msg_t));
    msg->id = NMT + node_id;
    msg->dlc = NMT_SIZE;
    msg->type = TYPE_NMT;
    msg->node = get_node_index(canopen, msg->id);
    msg->node->nmt_state = (canopen->role == CANOPEN_CLIENT) ? NMT_STATE_OPERATIONAL : NMT_STATE_BOOTUP;

    return CANOPEN_OK;
}

canopen_state_t canopen_config_nmt_bootup_collback(canopen_t *canopen, canopen_msg_t *msg)
{
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

canopen_state_t canopen_transmite_nmt_bootup(canopen_msg_t *msg)
{
    assert(msg != NULL);

    if (msg->frame.nmt.node_id == 0)
        return CANOPEN_ERROR;

    msg->id = NMT_STATE_BOOTUP;
}