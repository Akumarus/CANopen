#include "sdo.h"

// void sdo_client_send(CANopen *canopen, uint8_t node_id, canopen_message_t *msg)
// {
//   msg->type = TYPE_SDO;
//   msg->id = SDO_TX + node_id;
//   msg->dlc = 8;
//   can_fifo_push(canopen->tx_buff, msg);
// }

// void sdo_client_upload(CANopen *canopen, uint8_t node_id, uint16_t index, uint8_t sub_index)
// {
//   canopen_message_t msg;
//   msg.frame.sdo.cmd = SDO_CLIENT_INITIATE_UPLOAD;
//   msg.frame.sdo.index = index;
//   msg.frame.sdo.sub_index = sub_index;
//   msg.frame.sdo.data = 0;

//   sdo_client_send(canopen, node_id, &msg);
// }