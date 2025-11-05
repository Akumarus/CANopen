#include "sdo.h"

FIFO_CAN_State canopen_send_sdo(CANopen *canopen, uint8_t node_id, canopen_message_t *msg)
{
  if (canopen == NULL || msg == NULL)
    return FIFO_CAN_ERROR;

  FIFO_CAN_State fifo_state;
  msg->type = TYPE_SDO;
  msg->id = SDO_TX + node_id;
  msg->dlc = 8;
  fifo_state = can_fifo_push(&canopen->tx_fifo, msg);
  canopen->info.tx_pdo_count++;
  if (fifo_state == FIFO_CAN_FULL)
    canopen->info.tx_pdo_lost_count++;
  return fifo_state;
}

void canopen_read_sdo(uint8_t node_id, uint16_t index, uint8_t sub_index, uint32_t data)
{
  canopen_message_t msg;
  msg.frame.sdo.cmd = 0x40;
  msg.frame.sdo.index = index;
  msg.frame.sdo.sub_index = sub_index;
  msg.frame.sdo.data = data;
  
}


