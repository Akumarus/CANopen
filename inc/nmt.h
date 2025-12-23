#ifndef NMT_H
#define NMT_H

#include "can_open.h"
#include "def.h"

co_res_t canopen_server_transmite_heartbeat(co_obj_t *canopen);
co_res_t canopen_client_transmite_nmt_cmd(co_obj_t *canopen, uint8_t node_id,
                                          canopen_nmt_cmd_t cmd);

inline co_res_t canopen_nmt_start_remote_node(co_obj_t *canopen, uint8_t node_id)
{
    return canopen_client_transmite_nmt_cmd(canopen, node_id, NMT_CS_START_REMOTE_NODE);
}

inline co_res_t canopen_nmt_stop_remote_node(co_obj_t *canopen, uint8_t node_id)
{
    return canopen_client_transmite_nmt_cmd(canopen, node_id, NMT_CS_STOP_REMOTE_NODE);
}

inline co_res_t canopen_nmt_enter_pre_operation(co_obj_t *canopen, uint8_t node_id)
{
    return canopen_client_transmite_nmt_cmd(canopen, node_id, NMT_CS_ENTER_PRE_OPERATION);
}
inline co_res_t canopen_nmt_reset_node(co_obj_t *canopen, uint8_t node_id)
{
    return canopen_client_transmite_nmt_cmd(canopen, node_id, NMT_CS_RESET_NODE);
}

inline co_res_t canopen_nmt_reset_communication(co_obj_t *canopen, uint8_t node_id)
{
    return canopen_client_transmite_nmt_cmd(canopen, node_id, NMT_CS_RESET_COMMUNICATION);
}

/** Broadcast nmt command --------------------------------------------------------------------*/
inline co_res_t canopen_nmt_broadcast_start_remote_node(co_obj_t *canopen, uint8_t node_id)
{
    return canopen_client_transmite_nmt_cmd(canopen, node_id, NMT_CS_START_REMOTE_NODE);
}

inline co_res_t canopen_nmt_broadcast_stop_remote_node(co_obj_t *canopen, uint8_t node_id)
{
    return canopen_client_transmite_nmt_cmd(canopen, node_id, NMT_CS_STOP_REMOTE_NODE);
}

inline co_res_t canopen_nmt_broadcast_enter_pre_operation(co_obj_t *canopen, uint8_t node_id)
{
    return canopen_client_transmite_nmt_cmd(canopen, node_id, NMT_CS_ENTER_PRE_OPERATION);
}
inline co_res_t canopen_nmt_broadcast_reset_node(co_obj_t *canopen, uint8_t node_id)
{
    return canopen_client_transmite_nmt_cmd(canopen, node_id, NMT_CS_RESET_NODE);
}

inline co_res_t canopen_nmt_broadcast_reset_communication(co_obj_t *canopen, uint8_t node_id)
{
    return canopen_client_transmite_nmt_cmd(canopen, node_id, NMT_CS_RESET_COMMUNICATION);
}

#endif // NMT_H