#include "nmt.h"
#include "fifo.h"

#define NMT_SIZE 2
#define BOOTUP_CMD 0x05
#define HEARTBEAT_CMD 0x00

co_res_t co_srv_proc_nmt(co_obj_t *co, co_msg_t *msg) {
    assert(co != NULL);
    assert(co->role != CANOPEN_CLIENT);

    uint8_t node_id = msg->id - ID_NMT;
    if ((node_id != co->node_id) && (node_id != 0))
        return CANOPEN_ERROR; // TODO Нужно вернуть, что фильтр неправильно
                              // сконфигурирован или может это норма

    // switch (msg->frame.nmt.cmd) {
    // case NMT_CS_START:
    //     if (co->nmt_state == NMT_STATE_PRE_OPERATIONAL || co->nmt_state == NMT_STATE_STOPPED) {
    //         co->nmt_state = NMT_STATE_OPERATIONAL;
    //     }
    //     break;
    // case NMT_CS_STOP:
    //     if (co->nmt_state == NMT_STATE_OPERATIONAL || co->nmt_state == NMT_STATE_PRE_OPERATIONAL) {
    //         co->nmt_state = NMT_STATE_STOPPED;
    //     }
    //     break;
    // case NMT_CS_PREOP:
    //     if (co->nmt_state == NMT_STATE_OPERATIONAL || co->nmt_state == NMT_STATE_STOPPED) {
    //         co->nmt_state = NMT_STATE_PRE_OPERATIONAL;
    //     }
    //     break;
    // case NMT_CS_RESET:
    //     co->nmt_state = NMT_CS_RESET;
    //     // Логика сброса узла
    //     break;

    // case NMT_CS_COM_RESET:
    //     co->nmt_state = NMT_CS_COM_RESET;
    //     // Логика сброса настроек сети
    //     // canopen_tran
    //     break;
    // default:
    //     return CANOPEN_ERROR;
    // }

    return CANOPEN_OK;
}

co_res_t co_nmt_send_bootup(co_obj_t *co) {
    assert(co != NULL);
    assert(co->role == CANOPEN_SERVER);

    co_msg_t msg = {0};
    msg.id = ID_HEARTBEAT + co->node_id;
    msg.dlc = 1;
    // msg.frame.nmt.cmd = BOOTUP_CMD; // TODO чему должно быть равно
    co->nmt_state = NMT_STATE_PRE_OPERATIONAL;
    fifo_state_t fifo_state = fifo_push(&co->fifo_tx, &msg);
    return (fifo_state == FIFO_OK) ? CANOPEN_OK : CANOPEN_ERROR;
}

co_res_t canopen_server_timeout(co_obj_t *canopen, uint32_t current_time) {
    assert(canopen != NULL);
    assert(canopen->role == CANOPEN_SERVER);

    if (canopen->nmt_state == NMT_STATE_BOOTUP) {
        co_nmt_send_bootup(canopen);
        return CANOPEN_OK;
    }

    if (canopen->nmt_state == NMT_STATE_RESETING) {
        // TODO Что-то делаем
    }
    return CANOPEN_OK;
}

co_res_t co_nmt_send_cmd(co_obj_t *co, uint8_t node_id, co_nmt_cmd_t cmd) {
    assert(co != NULL);
    assert(co->role == CANOPEN_CLIENT);

    co_msg_t msg = {0};
    msg.id = ID_NMT;
    msg.dlc = 2;
    // msg.frame.nmt.cmd = cmd;
    // msg.frame.nmt.node_id = node_id;

    // if (((cmd == NMT_CS_RESET) || (cmd == NMT_CS_COM_RESET)) && node_id != 0) {
    //     co_node_t *node = co_get_node_obj(co, node_id);
    // }

    fifo_state_t fifo_state = fifo_push(&co->fifo_tx, &msg);
    return (fifo_state == FIFO_OK) ? CANOPEN_OK : CANOPEN_ERROR;
}
