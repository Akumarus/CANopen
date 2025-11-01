#include "fifo.h"
#include <string.h>

FIFO_CAN_State can_fifo_init(CAN_FIFO *fifo, CAN_FIFO_config cnfg)
{
  if (fifo == NULL || cnfg.message == NULL || cnfg.size == 0)
    return FIFO_CAN_ERROR;

  memset(fifo, 0, sizeof(CAN_FIFO));

  fifo->message = cnfg.message;
  fifo->size = cnfg.size;
  return FIFO_CAN_OK;
}

FIFO_CAN_State can_fifo_push(CAN_FIFO *fifo, uint32_t id, uint8_t *data, uint8_t dlc, uint8_t msg_type)
{
  if (fifo == NULL || data == NULL || dlc > FIFO_DATA_MAX_SIZE)
    return FIFO_CAN_ERROR;

  if (fifo->count >= fifo->size) {
    fifo->lost_messages_count++;
    return FIFO_CAN_FULL;
  }
  
  fifo->message[fifo->head].id = id;
  fifo->message[fifo->head].dlc = dlc;
  fifo->message[fifo->head].type = msg_type;
  memcpy(fifo->message[fifo->head].data, data, dlc);
  
  fifo->head = (fifo->head + 1) % fifo->size;
  fifo->count++;
  
  return FIFO_CAN_OK;
}

FIFO_CAN_State can_fifo_pop(CAN_FIFO *fifo, CAN_Message *msg)
{
  if (fifo == NULL || msg == NULL)
    return FIFO_CAN_ERROR;

  if (fifo->count == 0) {
    return FIFO_CAN_EMPTY;
  }

  memcpy(msg, &fifo->message[fifo->tail], sizeof(CAN_Message));
  fifo->tail = (fifo->tail + 1) % fifo->size;
  fifo->count--;

  return FIFO_CAN_OK;
}

uint16_t can_fifo_is_empty(CAN_FIFO *fifo) {
  return fifo->count == 0;
}

uint16_t can_fifo_is_full(CAN_FIFO *fifo) {
  return fifo->count >= fifo->size;
}

uint16_t can_fifo_get_lost_count(CAN_FIFO *fifo) {
  return fifo->lost_messages_count;
}
