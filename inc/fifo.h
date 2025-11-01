#ifndef FIFO_CAN_H
#define FIFO_CAN_H

#include "stdint.h"

#define FIFO_DATA_MAX_SIZE 8

typedef enum
{
  TYPE_NONE = 0,
  TYPE_SDO,
  TYPE_PDO,
} FIFO_CAN_type;

typedef enum
{
  FIFO_CAN_ERROR = 0,
  FIFO_CAN_OK,
  FIFO_CAN_FULL,
  FIFO_CAN_EMPTY,
} FIFO_CAN_State;

typedef struct {
  uint32_t id;
  uint8_t  data[FIFO_DATA_MAX_SIZE];
  uint8_t  dlc;
  uint8_t  type;
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

FIFO_CAN_State can_fifo_init(CAN_FIFO *fifo, CAN_FIFO_config cnfg);
FIFO_CAN_State can_fifo_push(CAN_FIFO *fifo, uint32_t id, uint8_t *data, uint8_t dlc, uint8_t msg_type);
FIFO_CAN_State can_fifo_pop(CAN_FIFO *fifo, CAN_Message *msg);
uint16_t can_fifo_is_empty(CAN_FIFO *fifo);
uint16_t can_fifo_is_full(CAN_FIFO *fifo);
uint16_t can_fifo_get_lost_count(CAN_FIFO *fifo);

#endif //FIFO_CAN_H
