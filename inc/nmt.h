#ifndef NMT_H
#define NMT_H

#include "can_open.h"
#include "def.h"

co_res_t co_srv_proc_nmt(co_obj_t *co, co_msg_t *msg);
co_res_t co_cli_proc_heartbeat(co_obj_t *co, co_msg_t *msg);

co_res_t co_nmt_send_bootup(co_obj_t *co);
co_res_t co_nmt_send_heartbeat(co_obj_t *co);
co_res_t co_nmt_send_cmd(co_obj_t *co, uint8_t node_id, co_nmt_cmd_t cmd);

// Короткие и понятные названия
inline co_res_t co_nmt_start(co_obj_t *co, uint8_t id)
{
    return co_nmt_send_cmd(co, id, NMT_CS_START);
}

inline co_res_t co_nmt_stop(co_obj_t *co, uint8_t id)
{
    return co_nmt_send_cmd(co, id, NMT_CS_STOP);
}

inline co_res_t co_nmt_preop(co_obj_t *co, uint8_t id)
{
    return co_nmt_send_cmd(co, id, NMT_CS_PREOP);
}

inline co_res_t co_nmt_reset(co_obj_t *co, uint8_t id)
{
    return co_nmt_send_cmd(co, id, NMT_CS_RESET);
}

inline co_res_t co_nmt_com_reset(co_obj_t *co, uint8_t id)
{
    return co_nmt_send_cmd(co, id, NMT_CS_COM_RESET);
}

// Broadcast версии
inline co_res_t co_nmt_bcast_start(co_obj_t *co)
{
    return co_nmt_send_cmd(co, 0, NMT_CS_START);
}

inline co_res_t co_nmt_bcast_stop(co_obj_t *co)
{
    return co_nmt_send_cmd(co, 0, NMT_CS_STOP);
}

inline co_res_t co_nmt_bcast_preop(co_obj_t *co)
{
    return co_nmt_send_cmd(co, 0, NMT_CS_PREOP);
}

inline co_res_t co_nmt_bcast_reset(co_obj_t *co)
{
    return co_nmt_send_cmd(co, 0, NMT_CS_RESET);
}

inline co_res_t co_nmt_bcast_com_reset(co_obj_t *co)
{
    return co_nmt_send_cmd(co, 0, NMT_CS_COM_RESET);
}

#endif // NMT_H