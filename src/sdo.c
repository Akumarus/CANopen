#include "sdo.h"
#include "obj.h"
#include "port.h"

static canopen_node_t *get_node_index(canopen_t *canopen, uint8_t node_id);

#define SDO_DEFAULT_TIMEOUT_MS 1000

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

  return canopen_config_callback(canopen, msg->id, 1, callback);
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
  if (!msg->node->status.bit.sdo_pending)
    return CANOPEN_ERROR;

  msg->node = get_node_index(canopen, msg->id);
  if (msg->node == NULL)
    return CANOPEN_ERROR;

  // TODO Abort CMD

  // Сброс таймера
  msg->node->sdo_timestamp = 0;
  msg->node->status.bit.sdo_pending = 0;
  canopen_sdo_callback(canopen, msg);
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
