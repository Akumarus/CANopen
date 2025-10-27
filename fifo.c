#include "fifo.h"
#include <string.h>

uint16_t CAN_FIFO_init(CAN_FIFO *fifo, CAN_FIFO_config cnfg)
{
  if (fifo == NULL || cnfg.message == NULL || cnfg.size == 0)
    return 0;

  memset(fifo, 0, sizeof(CAN_FIFO));

  fifo->message = cnfg.message;
  fifo->size = cnfg.size;
  return 1;
}

uint16_t CAN_FIFO_push(CAN_FIFO *fifo, uint32_t id, uint8_t *data, uint8_t dlc)
{
  if (fifo->count >= fifo->size) {
    fifo->lost_messages_count++;
    return 0;
  }
  
  fifo->message[fifo->head].id = id;
  fifo->message[fifo->head].dlc = dlc;
  memcpy(fifo->message[fifo->head].data, data, dlc);
  
  fifo->head = (fifo->head + 1) % fifo->size;
  fifo->count++;
  
  return 1;
}

uint16_t CAN_FIFO_pop(CAN_FIFO *fifo, CAN_Message *msg)
{
  if (fifo->count == 0) {
    return 0;
  }

  *msg = fifo->message[fifo->tail];
  fifo->tail = (fifo->tail + 1) % fifo->size;
  fifo->count--;

  return 1;
}
