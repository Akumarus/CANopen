#ifndef CAN_OPEN_H
#define CAN_OPEN_H

#include "stdint.h"

#define COB_SIZE_DEF            8
#define COB_SIZE_PDO COB_SIZE_DEF // communication object
#define COB_SIZE_SDO            4 // communication object
#define MAX_BANK_COUNT          14
#define IDS_PER_BANK            4
#define MAX_CALLBACKS           10
#define MAX_11BIT_ID            0x7FF

typedef void (*canopen_callback)(uint32_t id, uint8_t *data, uint8_t dlc);

typedef enum
{
  COB_RX_FIFO0 = 0x00000000U, /*!< CAN receive FIFO 0 */
  COB_RX_FIFO1 = 0x00000001U  /*!< CAN receive FIFO 1 */
}RxFifoType;

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
  CANOPEN_OK = 0,
  CANOPEN_ERROR,
} CANopen_State;

typedef union
{
  uint16_t all;
  struct 
  {
    uint16_t inited : 1;
    uint16_t filter_banks_full : 1;
    uint16_t callbacks_full : 1;
    uint16_t invalid_id : 1;
    uint16_t invalid_dlc : 1;
    uint16_t invalid_fifo : 1;
    uint16_t reserv : 10;
  } bit;
}CANopenStatus;

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
  uint32_t ide;
  uint8_t  cmd;
  uint16_t index;
  uint8_t  sub_index;
  uint32_t data;
} CANopen_SDO;

typedef struct {
  uint32_t id;
  canopen_callback callback;
} CAN_Handler;

typedef struct {
  uint32_t ids[IDS_PER_BANK];
  uint8_t used_count;
  uint8_t fifo_assignment;
} FilterBank;

typedef struct {
  uint32_t ide; // CAN_identifier_type
  uint32_t tx_mailbox;
  FilterBank bank_list[MAX_BANK_COUNT];
  uint8_t bank_count;
  CANopenStatus status;
  CAN_Handler callbacks[MAX_CALLBACKS];
  uint8_t callbacks_count;
} CANopen;

void canopen_init(CANopen *canopen, uint32_t ide);
void canopen_config_filter_mask(CANopen *canopen, uint32_t id1,  uint32_t mask, uint8_t fifo);
CANopen_State canopen_config_filter_list_16b(CANopen *canopen, uint16_t id, uint8_t fifo);

CANopen_State canopen_config_callback(CANopen *canopen, uint32_t id, uint8_t fifo, canopen_callback callback);
CANopen_State canopen_config_pdo_tx(CANopen *canopen, uint32_t id, CANopen_PDO *pdo, uint32_t dlc);

void canopen_process_rx_message(CANopen *canopen, uint32_t id, uint8_t *data, uint8_t dlc);

void canopen_send_pdo();
void canopen_send_sdo();
void canopen_read_pdo();
void canopen_read_sdo();

#endif // CAN_OPEN_H