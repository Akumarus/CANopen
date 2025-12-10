#include "pdo.h"
#include "fifo.h"

// FIFO_CAN_State canopen_send_pdo(CANopen *canopen, canopen_message_t *msg)
// {
//   if (canopen == NULL || msg == NULL)
//     return FIFO_CAN_ERROR;

//   FIFO_CAN_State fifo_state;
//   fifo_state = can_fifo_push(&canopen->tx_fifo, msg);
//   canopen->info.tx_pdo_count++;
//   if (fifo_state == FIFO_CAN_FULL)
//     canopen->info.tx_pdo_lost_count++;
//   return fifo_state;
// }

// CANopen_State canopen_config_pdo_tx(CANopen *canopen, uint32_t id, canopen_message_t *msg, uint32_t dlc)
// {
//   CANopen_State res = CANOPEN_ERROR;

//   if (msg == NULL || canopen == NULL)
//     return res;

//   if (is_valid_id(canopen, id) != CANOPEN_OK)
//     return res;

//   msg->id = id;
//   msg->dlc = dlc;
//   msg->type = TYPE_PDO;
//   memset(msg->frame.pdo.data, 0, sizeof(canopen_message_t));

//   res = CANOPEN_OK;
//   return res;
// }
