#include "sdo.h"
#include "obj.h"

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

  return CANOPEN_OK;
}

canopen_state_t canopen_sdo_transmit(canopen_t *canopen, canopen_msg_t *msg)
{
  assert(canopen != NULL);
  assert(msg != NULL);
  assert(msg->node != NULL);

  canopen->info.sdo_tx_counter++;
  msg->node->sdo_timeout = SDO_DEFAULT_TIMEOUT_MS;
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

// FIFO_CAN_State canopen_send_sdo(CANopen *canopen, uint8_t node_id, canopen_message_t *msg)
// {
//   if (canopen == NULL || msg == NULL)
//     return FIFO_CAN_ERROR;

//   FIFO_CAN_State fifo_state;
//   msg->type = TYPE_SDO;
//   msg->id = SDO_TX + node_id;
//   msg->dlc = 8;
//   fifo_state = can_fifo_push(&canopen->tx_fifo, msg);
//   canopen->info.tx_pdo_count++;
//   if (fifo_state == FIFO_CAN_FULL)
//     canopen->info.tx_pdo_lost_count++;
//   return fifo_state;
// }

// void canopen_read_sdo(uint8_t node_id, uint16_t index, uint8_t sub_index, uint32_t data)
// {
//   canopen_message_t msg;
//   msg.frame.sdo.cmd = 0x40;
//   msg.frame.sdo.index = index;
//   msg.frame.sdo.sub_index = sub_index;
//   msg.frame.sdo.data = data;

// }

// void sdo_send(CANopen *canopen, uint8_t node_id, canopen_message_t msg) {

// }

// void sdo_server_send_abort(CANopen *canopen, uint8_t node_id, uint16_t index, uint8_t sub_index, uint32_t error_code)
// {
//   canopen_message_t msg;

//   msg.frame.sdo.cmd = SDO_ABORT_TRANSFER;
//   msg.frame.sdo.index = index;
//   msg.frame.sdo.sub_index = sub_index;
//   msg.frame.sdo.data = error_code;

// }

// void sdo_server_process_request(CANopen *canopen, canopen_message_t *msg) {
//   uint8_t sdo_cmd = msg->frame.sdo.cmd & 0xE0; // TODO почему 0xE0
//   uint8_t node_id = msg->id - SDO_RX;

//   switch(sdo_cmd) {
//     case SDO_CLIENT_INITIATE_UPLOAD:
//       break;

//     case SDO_CLIENT_INITIATE_DOWNLOAD:
//       break;

//     case SDO_CLIENT_UPLOAD_SEGMENT:
//       break;

//     case SDO_CLIENT_DOWNLOAD_SEGMENT:
//       break;

//     case SDO_ABORT_TRANSFER:
//       break;

//     default:
//       break;
//   }
// }

// static void sdo_server_upload_request(CANopen *canopen, uint8_t, uint8_t nodeId, canopen_message_t *msg) {
//   uint16_t index = msg->frame.sdo.index;
//   uint8_t sub_index = msg->frame.sdo.index;

//   uint32_t obj_size = object_dictionary_get_size(index, sub_index);
//   if(obj_size == 0) {
//     // TODO send abort
//     return;
//   }

//   // object_dictionary_read(index, sub_index)
// }
