#ifndef FIFO_CAN_H
#define FIFO_CAN_H

#include "can_open.h"

#define CAN_FIFO_SIZE 64

typedef struct {
  uint32_t id;
  uint8_t  data[COB_SIZE_DEF];
  uint8_t  dlc;
} CAN_Message;

typedef struct {
  CAN_Message arr_messages[CAN_FIFO_SIZE];
  uint16_t head;
  uint16_t tail;
  uint16_t count;
  uint32_t lost_messages_count;
} CAN_FIFO;



#endif //FIFO_CAN_H
