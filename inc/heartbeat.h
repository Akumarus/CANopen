#ifndef HEARTBEAT_H
#define HEARTBEAT_H

#include "def.h"
#include "co.h"

co_res_t co_client_process_heartbeat(co_obj_t *co, co_msg_t *msg);
co_res_t co_server_process_heartbeat(co_obj_t *co, uint32_t time);

#endif // HEARTBEAT_H