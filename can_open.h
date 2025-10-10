#ifndef CAN_OPEN_H
#define CAN_OPEN_H

#include "can.h"
#define COB_SIZE_DEF            8
#define COB_SIZE_PDO COB_SIZE_DEF // communication object
#define COB_SIZE_SDO            4 // communication object

typedef enum
{
  COB_ID_STD = 0x00000000U, /*!< Standard Id */
  COB_ID_EXT = 0x00000004U  /*!< Extended Id */
}RemoteTransmissionType;

typedef enum
{
  COB_RTR_DATA   = 0x00000000U, /*!< Data frame   */
  COB_RTR_REMOTE = 0x00000002U  /*!< Remote frame */
}IdentifierType;

typedef struct 
{
  uint32_t id;
  uint8_t  dlс;
  uint8_t  data[COB_SIZE_PDO];
} CANopen_PDO;

typedef struct 
{
  uint32_t id;
  uint8_t  cmd;
  uint16_t index;
  uint8_t  sub_index;
  uint8_t  data[COB_SIZE_SDO];
} CANopen_SDO;

typedef struct {
  uint32_t ide; // CAN_identifier_type
  uint32_t tx_mailbox;
  CAN_HandleTypeDef can;
} CANopen;

void CANopen_init(CANopen *canopen, uint32_t ide);
void CANopen_send_pdo();
void CANopen_send_sdo();
void CANopen_read_pdo();
void CANopen_read_sdo();

#endif // CAN_OPEN_H