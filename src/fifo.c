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

FIFO_CAN_State can_fifo_push(CAN_FIFO *fifo, canopen_message_t *msg)
{
  if (fifo == NULL || msg == NULL)
    return FIFO_CAN_ERROR;

  if (fifo->count >= fifo->size) {
    fifo->lost_messages_count++;
    return FIFO_CAN_FULL;
  }
  
  memcpy(&fifo->message[fifo->head], msg, sizeof(canopen_message_t));
  fifo->head = (fifo->head + 1) % fifo->size;
  fifo->count++;
  
  return FIFO_CAN_OK;
}

FIFO_CAN_State can_fifo_pop(CAN_FIFO *fifo, canopen_message_t *msg)
{
  if (fifo == NULL || msg == NULL)
    return FIFO_CAN_ERROR;

  if (fifo->count == 0) {
    return FIFO_CAN_EMPTY;
  }

  memcpy(msg, &fifo->message[fifo->tail], sizeof(canopen_message_t));
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
