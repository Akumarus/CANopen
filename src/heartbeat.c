#include "heartbeat.h"

static co_res_t co_transmite_heartbeat(co_obj_t *co);

co_res_t co_client_process_heartbeat(co_obj_t *co, co_msg_t *msg) {
    assert(co != NULL);
    assert(msg != NULL);
    assert(co->role == CANOPEN_CLIENT);

    uint8_t node_id = msg->id - COB_ID_HEARTBEAT;
    co_node_t *node = co_get_node_obj(co, node_id);
    if (node == NULL) {
        // TODO Можно самим добавить узел сети
        return CANOPEN_ERROR;
    }
    node->state = msg->frame.nmt.cmd;
    return CANOPEN_OK;
}

co_res_t co_server_process_heartbeat(co_obj_t *co, uint32_t time) {
    assert(co != NULL);
    assert(co->role == CANOPEN_SERVER);

    if (co->heartbeat_enable && co->nmt_state != NMT_STATE_BOOTUP &&
        co->nmt_state != NMT_STATE_RESETING) {
        uint32_t elapsed = time - co->last_heartbeat_time;
        if (elapsed > co->heartbeat_interval_ms)
        {
            fifo_state_t fifo_res = co_transmite_heartbeat(co);
            if(fifo_res != FIFO_OK)
                return CANOPEN_ERROR;

            co->last_heartbeat_time = time;
        }        
    }
    return CANOPEN_OK;
}

static co_res_t co_transmite_heartbeat(co_obj_t *co) {
    assert(co != NULL);
    assert(co->role == CANOPEN_SERVER);

    if (!co->heartbeat_enable || co->nmt_state == NMT_STATE_BOOTUP ||
        co->nmt_state == NMT_STATE_RESETING)
        return CANOPEN_OK;

    co_msg_t msg = {0};
    msg.id = COB_ID_HEARTBEAT + co->node_id;
    msg.dlc = 1;
    msg.type = TYPE_HEARTBEAT;
    msg.frame.nmt.cmd = co->nmt_state;
    fifo_state_t fifo_state = fifo_push(&co->fifo_tx, &msg);
    return (fifo_state == FIFO_OK) ? CANOPEN_OK : CANOPEN_ERROR;
}