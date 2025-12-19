#include "sdo.h"
#include "obj.h"
#include "port.h"

static canopen_state_t canopen_server_process_sdo_upload_initiate(canopen_t *canopen, canopen_msg_t *msg);
static canopen_state_t canopen_server_process_download_expedited(canopen_t *canopen, canopen_msg_t *msg);
static canopen_state_t canopen_server_send_abort(canopen_t *canopen, canopen_msg_t *msg, uint32_t abort_code);

#define SDO_DEFAULT_TIMEOUT_MS 1000
#define SDO_GET_SIZE_FROM_CMD(msg) 4 - ((msg->frame.sdo.cmd & SDO_SIZE_N_MASK) >> 2)
#define SDO_SET_SERVER_ID(msg) ((msg)->id = ((msg)->id - SDO_TX) + SDO_RX)

__attribute__((weak)) void canopen_sdo_callback(canopen_t *canopen, canopen_msg_t *msg)
{
}

canopen_state_t canopen_sdo_config(canopen_t *canopen, canopen_msg_t *msg, uint8_t node_id, canopen_callback callback)
{
  assert(canopen != NULL);
  assert(msg != NULL);
  assert(node_id < 128);

  memset(msg, 0, sizeof(canopen_msg_t));

  msg->id = (canopen->role == CANOPEN_CLIENT) ? SDO_TX : SDO_RX;
  msg->id += node_id;
  msg->type = (canopen->role == CANOPEN_CLIENT) ? TYPE_SDO_TX : TYPE_SDO_RX;
  msg->dlc = COB_SIZE_SDO;

  msg->node = get_node_index(canopen, node_id);
  if (msg->node == NULL)
    return CANOPEN_ERROR;

  uint32_t id = (canopen->role == CANOPEN_CLIENT) ? SDO_RX : SDO_TX;
  id += canopen->node_id;

  return canopen_config_callback(canopen, id, 1, callback);
}

canopen_state_t canopen_sdo_transmit(canopen_t *canopen, canopen_msg_t *msg,
                                     uint8_t cmd, uint16_t index,
                                     uint8_t sub_index, uint32_t data)
{
  assert(canopen != NULL);
  assert(msg != NULL);
  // assert(msg->node != NULL); // TODO надо вернуть!

  msg->frame.sdo.cmd = cmd;
  msg->frame.sdo.index = index;
  msg->frame.sdo.sub_index = sub_index;
  msg->frame.sdo.data = data;

  if (cmd != SDO_REQ_ABORT)
    msg->node->status.bit.sdo_pending = 1;

  fifo_state_t fifo_state = fifo_push(&canopen->fifo_tx, msg);

  return (fifo_state == FIFO_FULL) ? CANOPEN_ERROR : CANOPEN_OK;
}

canopen_state_t canopen_client_process_sdo(canopen_t *canopen, canopen_msg_t *msg)
{
  if (!msg->node->status.bit.sdo_pending)
    return CANOPEN_ERROR;

  msg->node = get_node_index(canopen, msg->id);
  if (msg->node == NULL)
    return CANOPEN_ERROR;

  if (msg->frame.sdo.cmd == SDO_REQ_ABORT)
  {
    msg->node->status.bit.sdo_pending = 0;
    msg->node->sdo_timestamp = 0;
    canopen_sdo_callback(canopen, msg);
    return CANOPEN_ERROR;
  }

  // Сброс таймера
  msg->node->sdo_timestamp = 0;
  msg->node->status.bit.sdo_pending = 0;
  canopen_sdo_callback(canopen, msg);
  return CANOPEN_OK;
}

canopen_state_t canopen_server_process_sdo(canopen_t *canopen, canopen_msg_t *msg)
{
  switch (msg->frame.sdo.cmd)
  {
  case SDO_REQ_INITIATE_UPLOAD:
    canopen_server_process_sdo_upload_initiate(canopen, msg);
    break;

  case SDO_REQ_WRITE_1BYTE:
  case SDO_REQ_WRITE_2BYTE:
  case SDO_REQ_WRITE_3BYTE:
  case SDO_REQ_WRITE_4BYTE:
    canopen_server_process_download_expedited(canopen, msg);
    break;

  case SDO_REQ_ABORT:
    msg->node->status.bit.sdo_pending = 0;
    break;

  default:
    canopen_server_send_abort(canopen, msg, SDO_ABORT_INVALID_CS);
    break;
  }

  return CANOPEN_OK;
}

static canopen_state_t canopen_server_process_sdo_upload_initiate(canopen_t *canopen, canopen_msg_t *msg)
{
  uint16_t data_size = object_dictionary_get_size(msg->frame.sdo.index, msg->frame.sdo.sub_index);
  if (data_size == 0)
    return canopen_server_send_abort(canopen, msg, SDO_ABORT_OBJ_NOT_EXIST);
  else if (data_size <= 4)
    object_dictionary_read(msg->frame.sdo.index, msg->frame.sdo.sub_index, &msg->frame.sdo.data, sizeof(uint32_t));
  SDO_SET_SERVER_ID(msg);
  // return canopen_sdo_transmit(canopen, msg, SDO_RESP_UPLOAD_DATA, msg->frame.sdo.index, msg->frame.sdo.sub_index, msg->frame.sdo.data);
}

static canopen_state_t canopen_server_process_download_expedited(canopen_t *canopen, canopen_msg_t *msg)
{
  uint8_t data_size = SDO_GET_SIZE_FROM_CMD(msg);
  object_dictionary_write(msg->frame.sdo.index, msg->frame.sdo.sub_index, &msg->frame.sdo.data, data_size);
  SDO_SET_SERVER_ID(msg);
  // return canopen_sdo_transmit(canopen, msg, SDO_RESP_DOWNLOAD_OK, msg->frame.sdo.index, msg->frame.sdo.sub_index, 0);
}

static canopen_state_t canopen_server_send_abort(canopen_t *canopen, canopen_msg_t *msg, uint32_t abort_code)
{
  msg->id = (msg->id - SDO_TX) + SDO_RX;
  // return canopen_sdo_transmit(canopen, msg, SDO_REQ_ABORT, msg->frame.sdo.index, msg->frame.sdo.sub_index, abort_code);
}