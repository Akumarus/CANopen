#include "fifo.h"

uint16_t CAN_FIFO_Push(CAN_FIFO *fifo, uint32_t id, uint8_t *data, uint8_t dlc)
{
  if (fifo->count >= CAN_FIFO_SIZE) {
    fifo->lost_messages_count++;
    return 0;
  }
  
  fifo->arr_messages[fifo->head].id = id;
  fifo->arr_messages[fifo->head].dlc = dlc;
  memcpy(fifo->arr_messages[fifo->head].data, data, dlc);
  
  fifo->head = (fifo->head + 1) % CAN_FIFO_SIZE;
  fifo->count++;
  
  return 1;
}

uint16_t CAN_FIFO_Pop(CAN_FIFO *fifo, CAN_Message *msg)
{
  if (fifo->count == 0) {
    return 0;
  }
    
  *msg = fifo->arr_messages[fifo->tail];
  fifo->tail = (fifo->tail + 1) % CAN_FIFO_SIZE;
  fifo->count--;
    
  return 1;
}
