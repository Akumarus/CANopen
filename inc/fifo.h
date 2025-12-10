#ifndef FIFO_CAN_H
#define FIFO_CAN_H

#include "def.h"

typedef enum
{
  FIFO_ERROR = 0,
  FIFO_OK,
  FIFO_FULL,
  FIFO_EMPTY,
} fifo_state_t;

typedef struct
{
  canopen_msg_t *message;
  uint16_t size;
} fifo_config_t;

typedef struct
{
  uint16_t size;
  uint16_t head;
  uint16_t tail;
  uint16_t count;
  uint32_t lost_msg_count;
  canopen_msg_t *message;
} fifo_t;

fifo_state_t fifo_init(fifo_t *fifo, fifo_config_t config);
fifo_state_t fifo_push(fifo_t *fifo, canopen_msg_t *msg);
fifo_state_t fifo_pop(fifo_t *fifo, canopen_msg_t *msg);
uint16_t fifo_is_empty(fifo_t *fifo);
uint16_t fifo_is_full(fifo_t *fifo);
uint16_t fifo_get_lost(fifo_t *fifo);

#endif // FIFO_CAN_H
