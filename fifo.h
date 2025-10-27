#ifndef FIFO_CAN_H
#define FIFO_CAN_H

#include "can_open.h"

typedef struct {
  uint32_t id;
  uint8_t  data[COB_SIZE_DEF];
  uint8_t  dlc;
} CAN_Message;

typedef struct {
  CAN_Message* message;
  uint16_t size;
} CAN_FIFO_config;

typedef struct {
  CAN_Message* message;
  uint16_t size;
  uint16_t head;
  uint16_t tail;
  uint16_t count;
  uint32_t lost_messages_count;
} CAN_FIFO;

uint16_t CAN_FIFO_init(CAN_FIFO *fifo, CAN_FIFO_config cnfg);
uint16_t CAN_FIFO_push(CAN_FIFO *fifo, uint32_t id, uint8_t *data, uint8_t dlc);
uint16_t CAN_FIFO_pop(CAN_FIFO *fifo, CAN_Message *msg);

#endif //FIFO_CAN_H
