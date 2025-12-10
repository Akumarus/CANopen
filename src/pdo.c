#include "pdo.h"
#include "fifo.h"

canopen_state_t canopen_config_pdo_tx(canopen_t *canopen, canopen_msg_t *msg, uint32_t id, uint32_t dlc)
{
  if (msg == NULL || canopen == NULL)
    return CANOPEN_ERROR;

  // if (is_valid_id(canopen, id) != CANOPEN_OK)
  //   return CANOPEN_ERROR;

  msg->id = id;
  msg->dlc = dlc;
  msg->type = TYPE_PDO;
  memset(msg->frame.pdo.data, 0, sizeof(msg->frame.pdo.data));

  return CANOPEN_OK;
}

canopen_state_t canopen_send_pdo(canopen_t *canopen, canopen_msg_t *msg)
{
  if (canopen == NULL || msg == NULL)
    return CANOPEN_ERROR;

  canopen->info.tx_pdo_count++;
  fifo_state_t fifo_state = fifo_push(&canopen->fifo_tx, msg);
  if (fifo_state == FIFO_FULL)
    canopen->info.tx_pdo_lost_count++;

  return fifo_state;
}
