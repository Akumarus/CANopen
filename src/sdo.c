#include "sdo.h"
#include "obj.h"
#include "port.h"

static canopen_node_t *get_node_index(canopen_t *canopen, uint8_t node_id);

#define SDO_DEFAULT_TIMEOUT_MS 1000

__attribute__((weak)) void canopen_sdo_callback(canopen_t *canopen, canopen_msg_t *msg)
{
}

canopen_state_t canopen_sdo_generate(canopen_t *canopen, canopen_msg_t *msg, uint8_t node_id)
{
  assert(canopen != NULL);
  assert(msg != NULL);
  assert(node_id < 128);

  msg->id = (canopen->role == CANOPEN_SERVER) ? SDO_TX : SDO_RX;
  msg->id += node_id;
  msg->dlc = COB_SIZE_SDO;
  msg->type = TYPE_SDO_TX;

  msg->node = get_node_index(canopen, node_id);
  if (msg->node == NULL)
    return CANOPEN_ERROR;

  msg->node->sdo_timestamp = port_get_timestamp();

  return CANOPEN_OK;
}

canopen_state_t canopen_sdo_transmit(canopen_t *canopen, canopen_msg_t *msg)
{
  assert(canopen != NULL);
  assert(msg != NULL);
  assert(msg->node != NULL);

  canopen->info.sdo_tx_counter++;
  fifo_state_t fifo_state = fifo_push(&canopen->fifo_tx, msg);
  if (fifo_state == FIFO_FULL)
    return CANOPEN_ERROR;

  return CANOPEN_OK;
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
