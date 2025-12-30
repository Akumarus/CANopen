#include "sdo.h"
#include "obj.h"
#include "port.h"

// static co_res_t co_sdo_upload(co_obj_t *co, co_msg_t *msg);
// static co_res_t co_sdo_download(co_obj_t *co, co_msg_t *msg);
// static co_res_t co_sdo_abort(co_obj_t *co, co_msg_t *msg, uint32_t abort_code);

#define SDO_DEFAULT_TIMEOUT_MS 1000
#define SDO_GET_SIZE_FROM_CMD(msg) 4 - ((msg->frame.sdo.cmd & SDO_SIZE_N_MASK) >> 2)
#define SDO_SET_SERVER_ID(msg) ((msg)->id = ((msg)->id - COB_ID_SDO_TX) + COB_ID_SDO_RX)

co_res_t co_subscribe_sdo(co_obj_t *co, uint8_t node_id, co_hdl_t callback) {
    assert(co != NULL);
    assert(node_id > 0 && node_id < 127);
    assert(callback != NULL);

    uint32_t id = (co->role == CANOPEN_CLIENT) ? COB_ID_SDO_RX : COB_ID_SDO_TX;
    id += co->node_id;

    return canopen_config_callback(co, id, 1, callback);
}

co_res_t co_transmite_sdo(co_obj_t *co, uint8_t node_id, co_msg_t *msg, uint8_t dlc) {
    assert(co != NULL);
    assert(node_id > 0 && node_id < 127);
    assert(msg != NULL);

    msg->dlc = dlc;
    msg->type = (co->role == CANOPEN_CLIENT) ? TYPE_SDO_TX : TYPE_SDO_RX;
    msg->id = (co->role == CANOPEN_CLIENT) ? COB_ID_SDO_TX : COB_ID_SDO_RX;
    msg->id += node_id;

    fifo_state_t fifo_state = fifo_push(&co->fifo_tx, msg);
    return (fifo_state == FIFO_FULL) ? CANOPEN_ERROR : CANOPEN_OK;
}
co_res_t co_client_process_sdo(co_obj_t *co, co_msg_t *msg) {
    assert(co != NULL);
    assert(msg != NULL);

    return CANOPEN_OK;
}

co_res_t co_server_process_sdo(co_obj_t *co, co_msg_t *msg) {
    assert(co != NULL);
    assert(msg != NULL);

    switch (msg->frame.sdo.cmd) {
    case SDO_REQ_UPLOAD:
        // co_sdo_upload(co, msg);
        break;
    case SDO_REQ_WRITE_1BYTE:
    case SDO_REQ_WRITE_2BYTE:
    case SDO_REQ_WRITE_3BYTE:
    case SDO_REQ_WRITE_4BYTE:
        // co_sdo_download(co, msg);
        break;

    case SDO_REQ_ABORT:
        break;

    default:
        // co_sdo_abort(co, msg, SDO_ABORT_INVALID_CS);
        break;
    }
    return CANOPEN_OK;
}

static co_res_t co_process_upload_sdo(co_obj_t *co, co_msg_t *msg) {
    uint16_t data_size = object_dictionary_get_size(msg->frame.sdo.idx, msg->frame.sdo.sidx);
    if (data_size == 0) {
        uint8_t id = (msg->id - COB_ID_SDO_TX) + COB_ID_SDO_RX;
        return co_abort(co, id, msg->frame.sdo.idx, msg->frame.sdo.sidx, SDO_ABORT_OBJ_NOT_EXIST);
    } else if (data_size <= 4) {
        object_dictionary_read(msg->frame.sdo.idx, msg->frame.sdo.sidx, &msg->frame.sdo.data,
                               sizeof(uint32_t));
        SDO_SET_SERVER_ID(msg);
    }
    return CANOPEN_OK;

    // return canopen_sdo_transmit(canopen, msg, SDO_RESP_UPLOAD_DATA,
    // msg->frame.sdo.index, msg->frame.sdo.sub_index, msg->frame.sdo.data);
    return CANOPEN_OK;
}

// static co_res_t co_sdo_download(co_obj_t *co, co_msg_t *msg) {
//     uint8_t data_size = SDO_GET_SIZE_FROM_CMD(msg);
//     object_dictionary_write(msg->frame.sdo.index, msg->frame.sdo.sub_index, &msg->frame.sdo.data,
//                             data_size);
//     SDO_SET_SERVER_ID(msg);
//     // return canopen_sdo_transmit(canopen, msg, SDO_RESP_DOWNLOAD_OK,
//     // msg->frame.sdo.index, msg->frame.sdo.sub_index, 0);
//     return CANOPEN_OK;
// }

// static co_res_t co_sdo_abort(co_obj_t *co, co_msg_t *msg, uint32_t abort_code) {
//     msg->id = (msg->id - COB_ID_SDO_TX) + COB_ID_SDO_RX;
//     // return canopen_sdo_transmit(canopen, msg, SDO_REQ_ABORT,
//     // msg->frame.sdo.index, msg->frame.sdo.sub_index, abort_code);
//     return CANOPEN_OK;
// }