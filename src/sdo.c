#include "sdo.h"
#include "obj.h"
#include "port.h"

static co_res_t co_process_upload_sdo(co_obj_t *co, co_msg_t *msg);
static co_res_t co_process_download_sdo(co_obj_t *co, co_msg_t *msg);
// static co_res_t co_sdo_abort(co_obj_t *co, co_msg_t *msg, uint32_t abort_code);

#define SDO_DEFAULT_TIMEOUT_MS 1000
#define SDO_GET_SIZE_FROM_CMD(msg) 4 - ((msg->data[0] & SDO_SIZE_N_MASK) >> 2)
#define SDO_SET_SERVER_ID(msg) ((msg)->id = ((msg)->id - ID_RSDO) + ID_TSDO)

// #define RPDO1(node_id) (0x200 + ((node_id) & 0x7F))

co_res_t co_subscribe_sdo(co_obj_t *co, uint32_t id, co_hdl_t callback) {
    co_config_filter(co->banks, ID_RSDO + (id & 0x7F), 1);
    co_config_filter(co->banks, ID_TSDO + (id & 0x7F), 1);
    co_config_callback(co, id, callback);
    return CANOPEN_OK;
}

co_res_t co_transmite_sdo(co_obj_t *co, uint32_t id, uint8_t *data, uint8_t dlc) {
    co_msg_t msg = { .dlc = dlc, .id = id};
    memcpy(msg.data, data, dlc);
    fifo_state_t fifo_state = fifo_push(&co->fifo_tx, &msg);
    return (fifo_state == FIFO_FULL) ? CANOPEN_ERROR : CANOPEN_OK;
}

co_res_t co_client_process_sdo(co_obj_t *co, co_msg_t *msg) {
    return CANOPEN_OK;
}

co_res_t co_server_process_sdo(co_obj_t *co, co_msg_t *msg) {
    switch (msg->data[0]) {
    case SDO_REQ_UPLOAD:
        co_process_upload_sdo(co, msg);
        break;
    case SDO_REQ_WRITE_1BYTE:
    case SDO_REQ_WRITE_2BYTE:
    case SDO_REQ_WRITE_3BYTE:
    case SDO_REQ_WRITE_4BYTE:
        co_process_download_sdo(co, msg);
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
    // uint16_t idx = msg->frame.sdo.idx = (msg->frame.row[2] << 8 | msg->frame.row[1]);
    uint16_t idx = (msg->data[2] << 8 | msg->data[1]);
    uint16_t size = co_od_size(idx, msg->data[3]);
    if (size == 0) {
        // uint8_t id = (msg->id - COB_ID_SDO_TX) + COB_ID_SDO_RX;
        // co_abort_sdo(co, id, idx, msg->frame.sdo.sidx, SDO_ABORT_OBJ_NOT_EXIST);
        return CANOPEN_ERROR;
    } else if (size <= 4) {
        co_od_read(idx, msg->data[3], &msg->data[4], sizeof(uint32_t));
        SDO_SET_SERVER_ID(msg);
        msg->data[0] = 0x43;
        co_transmite_sdo(co, msg->id, msg->data, 8);
    }
    return CANOPEN_OK;
}

static co_res_t co_process_download_sdo(co_obj_t *co, co_msg_t *msg) {
    assert(co != NULL);
    assert(msg != NULL);

    uint8_t data_size = SDO_GET_SIZE_FROM_CMD(msg);
    // co_od_write(msg->data.idx, msg->frame.sdo.sidx, &msg->data, data_size);
    SDO_SET_SERVER_ID(msg);
    // co_transmite_sdo(co, msg->id)
    // return canopen_sdo_transmit(canopen, msg, SDO_RESP_DOWNLOAD_OK,
    // msg->frame.sdo.index, msg->frame.sdo.sub_index, 0);
    return CANOPEN_OK;
}

// static co_res_t co_sdo_abort(co_obj_t *co, co_msg_t *msg, uint32_t abort_code) {
//     msg->id = (msg->id - COB_ID_SDO_TX) + COB_ID_SDO_RX;
//     // return canopen_sdo_transmit(canopen, msg, SDO_REQ_ABORT,
//     // msg->frame.sdo.index, msg->frame.sdo.sub_index, abort_code);
//     return CANOPEN_OK;
// }