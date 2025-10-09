#ifndef CAN_OPEN_H
#define CAN_OPEN_H

#include "can.h"

#define COB_SIZE 8 // communication object


typedef struct 
{
  uint32_t id;
  uint8_t  dlс;
  uint8_t  data[COB_SIZE];
} CANopen_PDO;

typedef struct {
  uint32_t ide; // CAN_identifier_type
  uint32_t tx_mailbox;
  CAN_HandleTypeDef can;
} CANopen;

void CANopen_init();
void CANopen_send_pdo();
void CANopen_send_sdo();
void CANopen_read_pdo();
void CANopen_read_sdo();

#endif // CAN_OPEN_H