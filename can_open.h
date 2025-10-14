#ifndef CAN_OPEN_H
#define CAN_OPEN_H

#include "stdint.h"

#define COB_SIZE_DEF            8
#define COB_SIZE_PDO COB_SIZE_DEF // communication object
#define COB_SIZE_SDO            4 // communication object
#define MAX_BANK_COUNT          14



typedef enum
{
  COB_FILTERMODE_IDMASK = 0x00000000U,
  COB_FILTERMODE_IDLIST = 0x00000001U
}FileterMode;


typedef enum
{
  COB_FILTERSCALE_16BIT = 0x00000000U,
  COB_FILTERSCALE_32BIT = 0x00000001U,
} FilterScale;


typedef enum 
{
  COB_ID_STD = 0x00000000U, /*!< Standard Id */
  COB_ID_EXT = 0x00000004U  /*!< Extended Id */
}Remote_Transmission_Type;

typedef enum
{
  COB_RTR_DATA   = 0x00000000U, /*!< Data frame   */
  COB_RTR_REMOTE = 0x00000002U  /*!< Remote frame */
}Identifier_Type;

typedef enum
{
  OK = 0,
  NO_BANK,
  NO_CALLBACK,
} CANopen_Status;

typedef struct 
{
  uint32_t id;
  uint32_t ide;
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

typedef void (*canopen_pdo_rxcallback)(uint32_t id, uint8_t *data, uint8_t dlc);

typedef struct {
  uint32_t id;
  uint8_t data[COB_SIZE_PDO];
  canopen_pdo_rxcallback callback;
} CAN_Handler;

typedef struct {
  uint32_t ide; // CAN_identifier_type
  uint32_t tx_mailbox;
  uint32_t bank_count;
  CANopen_Status status;
} CANopen;



void canopen_init(CANopen *canopen, uint32_t ide);
void canopen_config_filter_mask(CANopen *canopen, uint32_t id1,  uint32_t mask, uint8_t fifo);
CANopen_Status canopen_config_filter_list_16b(CANopen *canopen, uint16_t id1, uint16_t id2, uint16_t id3, uint16_t id4, uint8_t fifo);
CANopen_Status canopen_register_pdo_rx_callback(CANopen *canopen, uint32_t id, canopen_pdo_rxcallback callback);

void canopen_pdo_config(CANopen *canopen, CANopen_PDO *pdo, uint32_t id, uint32_t dlc);
void canopen_process_rx_message(uint32_t id, uint8_t *data, uint8_t dlc);

void canopen_send_pdo();
void canopen_send_sdo();
void canopen_read_pdo();
void canopen_read_sdo();

#endif // CAN_OPEN_H