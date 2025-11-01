#ifndef FIFO_CAN_H
#define FIFO_CAN_H

#include "def.h"

#define FIFO_DATA_MAX_SIZE 8

typedef enum
{
  FIFO_CAN_ERROR = 0,
  FIFO_CAN_OK,
  FIFO_CAN_FULL,
  FIFO_CAN_EMPTY,
} FIFO_CAN_State;

typedef struct {
  canopen_message_t* message;
  uint16_t size;
} CAN_FIFO_config;

typedef struct {
  canopen_message_t* message;
  uint16_t size;
  uint16_t head;
  uint16_t tail;
  uint16_t count;
  uint32_t lost_messages_count;
} CAN_FIFO;

FIFO_CAN_State can_fifo_init(CAN_FIFO *fifo, CAN_FIFO_config cnfg);
FIFO_CAN_State can_fifo_push(CAN_FIFO *fifo, canopen_message_t *msg);
FIFO_CAN_State can_fifo_pop(CAN_FIFO *fifo, canopen_message_t *msg);
uint16_t can_fifo_is_empty(CAN_FIFO *fifo);
uint16_t can_fifo_is_full(CAN_FIFO *fifo);
uint16_t can_fifo_get_lost_count(CAN_FIFO *fifo);

#endif //FIFO_CAN_H
