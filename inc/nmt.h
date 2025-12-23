#ifndef NMT_H
#define NMT_H

#include "def.h"
#include "can_open.h"

canopen_state_t canopen_server_transmite_heartbeat(canopen_t *canopen);
canopen_state_t canopen_client_transmite_nmt_cmd(canopen_t *canopen, uint8_t node_id, canopen_nmt_cmd_t cmd);

static inline canopen_state_t canopen_nmt_start_remote_node(canopen_t *canopen, uint8_t node_id)
{
    return canopen_client_transmite_nmt_cmd(canopen, node_id, NMT_CS_START_REMOTE_NODE);
}

static inline canopen_state_t canopen_nmt_stop_remote_node(canopen_t *canopen, uint8_t node_id)
{
    return canopen_client_transmite_nmt_cmd(canopen, node_id, NMT_CS_STOP_REMOTE_NODE);
}

static inline canopen_state_t canopen_nmt_enter_pre_operation(canopen_t *canopen, uint8_t node_id)
{
    return canopen_client_transmite_nmt_cmd(canopen, node_id, NMT_CS_ENTER_PRE_OPERATION);
}
static inline canopen_state_t canopen_nmt_reset_node(canopen_t *canopen, uint8_t node_id)
{
    return canopen_client_transmite_nmt_cmd(canopen, node_id, NMT_CS_RESET_NODE);
}

static inline canopen_state_t canopen_nmt_reset_communication(canopen_t *canopen, uint8_t node_id)
{
    return canopen_client_transmite_nmt_cmd(canopen, node_id, NMT_CS_RESET_COMMUNICATION);
}



/** Broadcast nmt command --------------------------------------------------------------------*/
static inline canopen_state_t canopen_nmt_broadcast_start_remote_node(canopen_t *canopen, uint8_t node_id)
{
    return canopen_client_transmite_nmt_cmd(canopen, node_id, NMT_CS_START_REMOTE_NODE);
}

static inline canopen_state_t canopen_nmt_broadcast_stop_remote_node(canopen_t *canopen, uint8_t node_id)
{
    return canopen_client_transmite_nmt_cmd(canopen, node_id, NMT_CS_STOP_REMOTE_NODE);
}

static inline canopen_state_t canopen_nmt_broadcast_enter_pre_operation(canopen_t *canopen, uint8_t node_id)
{
    return canopen_client_transmite_nmt_cmd(canopen, node_id, NMT_CS_ENTER_PRE_OPERATION);
}
static inline canopen_state_t canopen_nmt_broadcast_reset_node(canopen_t *canopen, uint8_t node_id)
{
    return canopen_client_transmite_nmt_cmd(canopen, node_id, NMT_CS_RESET_NODE);
}

static inline canopen_state_t canopen_nmt_broadcast_reset_communication(canopen_t *canopen, uint8_t node_id)
{
    return canopen_client_transmite_nmt_cmd(canopen, node_id, NMT_CS_RESET_COMMUNICATION);
}


#endif // NMT_H