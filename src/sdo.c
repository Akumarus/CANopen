#include "sdo.h"
#include "obj.h"
#include "port.h"

static canopen_node_t *get_node_index(canopen_t *canopen, uint8_t node_id);

#define SDO_DEFAULT_TIMEOUT_MS 1000

__attribute__((weak)) void canopen_sdo_callback(canopen_t *canopen, canopen_msg_t *msg)
{
}

canopen_state_t canopen_sdo_write_32(canopen_t *canopen, canopen_msg_t *msg, uint16_t index, uint8_t sub_index, uint32_t data)
{
  return canopen_sdo_transmit(canopen, msg, SDO_CLIENT_WRITE_4BYTE, index, sub_index, data);
}

canopen_state_t canopen_sdo_write_16(canopen_t *canopen, canopen_msg_t *msg, uint16_t index, uint8_t sub_index, uint16_t data)
{
  return canopen_sdo_transmit(canopen, msg, SDO_CLIENT_WRITE_2BYTE, index, sub_index, data);
}

canopen_state_t canopen_sdo_write_8(canopen_t *canopen, canopen_msg_t *msg, uint16_t index, uint8_t sub_index, uint8_t data)
{
  return canopen_sdo_transmit(canopen, msg, SDO_CLIENT_WRITE_1BYTE, index, sub_index, data);
}

canopen_state_t canopen_sdo_read_32(canopen_t *canopen, canopen_msg_t *msg, uint16_t index, uint8_t sub_index)
{
  return canopen_sdo_transmit(canopen, msg, SDO_CLIENT_INITIATE_UPLOAD, index, sub_index, 0);
}

canopen_state_t canopen_sdo_read_16(canopen_t *canopen, canopen_msg_t *msg, uint16_t index, uint8_t sub_index)
{
  return canopen_sdo_transmit(canopen, msg, SDO_CLIENT_INITIATE_UPLOAD, index, sub_index, 0);
}

canopen_state_t canopen_sdo_read_8(canopen_t *canopen, canopen_msg_t *msg, uint16_t index, uint8_t sub_index)
{
  return canopen_sdo_transmit(canopen, msg, SDO_CLIENT_INITIATE_UPLOAD, index, sub_index, 0);
}

canopen_state_t canopen_sdo_generate(canopen_t *canopen, canopen_msg_t *msg, uint8_t node_id)
{
  assert(canopen != NULL);
  assert(msg != NULL);
  assert(node_id < 128);

  memset(msg, 0, sizeof(canopen_msg_t));

  msg->id = (canopen->role == CANOPEN_SERVER) ? SDO_TX : SDO_RX;
  msg->id += node_id;
  msg->dlc = COB_SIZE_SDO;
  msg->type = TYPE_SDO_TX;

  msg->node = get_node_index(canopen, node_id);
  if (msg->node == NULL)
    return CANOPEN_ERROR;

  return CANOPEN_OK;
}

canopen_state_t canopen_sdo_transmit(canopen_t *canopen, canopen_msg_t *msg,
                                     uint8_t cmd, uint16_t index,
                                     uint8_t sub_index, uint32_t data)
{
  assert(canopen != NULL);
  assert(msg != NULL);
  assert(msg->node != NULL);

  msg->frame.sdo.cmd = cmd;
  msg->frame.sdo.index = index;
  msg->frame.sdo.sub_index = sub_index;
  msg->frame.sdo.data = data;

  if (cmd != SDO_ABORT_TRANSFER)
    msg->node->status.bit.sdo_pending = 1;

  fifo_state_t fifo_state = fifo_push(&canopen->fifo_tx, msg);

  return (fifo_state == FIFO_FULL) ? CANOPEN_ERROR : CANOPEN_OK;
}

canopen_state_t canopen_sdo_process(canopen_t *canopen, canopen_msg_t *msg)
{
  switch (msg->type)
  {
  case TYPE_SDO_TX:
    break;

  case TYPE_SDO_RX: // Ответ от сервера
    msg->node = get_node_index(canopen, msg->id);
    if (msg->node != NULL)
    {
      msg->node->sdo_timestamp = 0;
      msg->node->status.bit.sdo_pending = 0;
    }
    canopen_sdo_callback(canopen, msg);
    break;
  default:
    break;
  }
  return CANOPEN_OK;
}

static canopen_node_t *get_node_index(canopen_t *canopen, uint8_t node_id)
{
  for (uint8_t i = 0; i < NODES_COUNT; i++)
  {
    if (canopen->node[i].id == node_id)
      return &canopen->node[i];
  }
  return NULL;
}
