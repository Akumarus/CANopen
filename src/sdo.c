#include "sdo.h"
#include "obj.h"
#include "port.h"

static co_res_t co_sdo_upload(co_obj_t *co, co_msg_t *msg);
static co_res_t co_sdo_download(co_obj_t *co, co_msg_t *msg);
static co_res_t co_sdo_abort(co_obj_t *co, co_msg_t *msg, uint32_t abort_code);

#define SDO_DEFAULT_TIMEOUT_MS 1000
#define SDO_GET_SIZE_FROM_CMD(msg) 4 - ((msg->frame.sdo.cmd & SDO_SIZE_N_MASK) >> 2)
#define SDO_SET_SERVER_ID(msg) ((msg)->id = ((msg)->id - COB_ID_SDO_TX) + COB_ID_SDO_RX)

co_res_t co_sdo_cfg(co_obj_t *co, co_msg_t *msg, uint8_t node_id, co_hdl_t callback) {

    assert(co != NULL);
    assert(msg != NULL);
    assert(node_id < 128);

    memset(msg, 0, sizeof(co_msg_t));

    msg->id = (co->role == CANOPEN_CLIENT) ? COB_ID_SDO_TX : COB_ID_SDO_RX;
    msg->id += node_id;
    msg->type = (co->role == CANOPEN_CLIENT) ? TYPE_SDO_TX : TYPE_SDO_RX;
    msg->dlc = COB_SIZE_SDO;

    uint32_t id = (co->role == CANOPEN_CLIENT) ? COB_ID_SDO_RX : COB_ID_SDO_TX;
    id += co->node_id;

    return canopen_config_callback(co, id, 1, callback);
}

co_res_t co_sdo_send(co_obj_t *co, co_msg_t *msg, uint8_t cmd, uint16_t index, uint8_t sub_index,
                     uint32_t data) {
    assert(co != NULL);
    assert(msg != NULL);

    msg->frame.sdo.cmd = cmd;
    msg->frame.sdo.index = index;
    msg->frame.sdo.sub_index = sub_index;
    msg->frame.sdo.data = data;

    fifo_state_t fifo_state = fifo_push(&co->fifo_tx, msg);
    return (fifo_state == FIFO_FULL) ? CANOPEN_ERROR : CANOPEN_OK;
}

co_res_t co_cli_proc_sdo(co_obj_t *co, co_msg_t *msg) {

    if (msg->frame.sdo.cmd == SDO_REQ_ABORT) {
        return CANOPEN_ERROR;
    }

    // Сброс таймера
    return CANOPEN_OK;
}

co_res_t co_srv_proc_sdo(co_obj_t *co, co_msg_t *msg) {
    switch (msg->frame.sdo.cmd) {
    case SDO_REQ_UPLOAD:
        co_sdo_upload(co, msg);
        break;

    case SDO_REQ_WRITE_1BYTE:
    case SDO_REQ_WRITE_2BYTE:
    case SDO_REQ_WRITE_3BYTE:
    case SDO_REQ_WRITE_4BYTE:
        co_sdo_download(co, msg);
        break;

    case SDO_REQ_ABORT:
        break;

    default:
        co_sdo_abort(co, msg, SDO_ABORT_INVALID_CS);
        break;
    }

    return CANOPEN_OK;
}

static co_res_t co_sdo_upload(co_obj_t *co, co_msg_t *msg) {
    uint16_t data_size = object_dictionary_get_size(msg->frame.sdo.index, msg->frame.sdo.sub_index);
    if (data_size == 0)
        return co_sdo_abort(co, msg, SDO_ABORT_OBJ_NOT_EXIST);
    else if (data_size <= 4)
        object_dictionary_read(msg->frame.sdo.index, msg->frame.sdo.sub_index, &msg->frame.sdo.data,
                               sizeof(uint32_t));
    SDO_SET_SERVER_ID(msg);
    // return canopen_sdo_transmit(canopen, msg, SDO_RESP_UPLOAD_DATA,
    // msg->frame.sdo.index, msg->frame.sdo.sub_index, msg->frame.sdo.data);
    return CANOPEN_OK;
}

static co_res_t co_sdo_download(co_obj_t *co, co_msg_t *msg) {
    uint8_t data_size = SDO_GET_SIZE_FROM_CMD(msg);
    object_dictionary_write(msg->frame.sdo.index, msg->frame.sdo.sub_index, &msg->frame.sdo.data,
                            data_size);
    SDO_SET_SERVER_ID(msg);
    // return canopen_sdo_transmit(canopen, msg, SDO_RESP_DOWNLOAD_OK,
    // msg->frame.sdo.index, msg->frame.sdo.sub_index, 0);
    return CANOPEN_OK;
}

static co_res_t co_sdo_abort(co_obj_t *co, co_msg_t *msg, uint32_t abort_code) {
    msg->id = (msg->id - COB_ID_SDO_TX) + COB_ID_SDO_RX;
    // return canopen_sdo_transmit(canopen, msg, SDO_REQ_ABORT,
    // msg->frame.sdo.index, msg->frame.sdo.sub_index, abort_code);
    return CANOPEN_OK;
}