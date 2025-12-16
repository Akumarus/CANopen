#ifndef CAN_OPEN_H
#define CAN_OPEN_H

#include "def.h"
#include "fifo.h"

#define NODES_COUNT 5

typedef void (*canopen_callback)(canopen_msg_t *msg);

typedef enum
{
  COB_RX_FIFO0 = 0x00000000U, /*!< CAN receive FIFO 0 */
  COB_RX_FIFO1 = 0x00000001U  /*!< CAN receive FIFO 1 */
} RxFifoType;

typedef enum
{
  COB_FILTERMODE_IDMASK = 0x00000000U,
  COB_FILTERMODE_IDLIST = 0x00000001U
} FileterMode;

typedef enum
{
  COB_FILTERSCALE_16BIT = 0x00000000U,
  COB_FILTERSCALE_32BIT = 0x00000001U,
} FilterScale;

typedef enum
{
  COB_ID_STD = 0x00000000U, /*!< Standard Id */
  COB_ID_EXT = 0x00000004U  /*!< Extended Id */
} Remote_Transmission_Type;

typedef enum
{
  COB_RTR_DATA = 0x00000000U,  /*!< Data frame   */
  COB_RTR_REMOTE = 0x00000002U /*!< Remote frame */
} Identifier_Type;

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
    uint16_t fifo_tx_full : 1;
    uint16_t reserv : 9;
  } bit;
} CANopenStatus;

typedef struct
{
  uint32_t id;
  canopen_callback callback;
} canopen_handler_t;

typedef struct
{
  uint32_t ids[IDS_PER_BANK];
  uint8_t used_count;
  uint8_t fifo_assignment;
} filter_bank_t;

typedef enum
{
  CANOPEN_SERVER = 0,
  CANOPEN_CLIENT,
} canopen_role_t;

typedef struct
{
  uint8_t bank_count;
  uint8_t callbacks_count;
  uint32_t tx_mailbox;
  CANopenStatus status;
  uint32_t sdo_tx_counter;
  uint32_t sdo_rx_counter;
  uint32_t sdo_tx_lost_counter;
  uint32_t sdo_timeout_counter;

  uint32_t fifo_rx_overflow_counter;
  uint32_t fifo_rx_complete_counter;

  uint32_t tx_sended_pdo_count;
  uint32_t tx_pdo_count;
  uint32_t tx_pdo_lost_count;
  uint32_t tx_fifo_full_errors;
  uint32_t tx_busy_mailbox_count;
} canopen_info_t;

typedef struct
{
  uint8_t node_id;
  uint32_t timestamp;
  uint32_t ide;
  fifo_t fifo_tx;
  fifo_t fifo_rx;
  canopen_role_t role;
  canopen_info_t info;
  canopen_node_t node[NODES_COUNT];
  canopen_msg_t buffer_tx[CAN_FIFO_SIZE];
  canopen_msg_t buffer_rx[CAN_FIFO_SIZE];
  filter_bank_t bank_list[MAX_BANK_COUNT];
  canopen_handler_t callbacks[MAX_CALLBACKS];
} canopen_t;

canopen_state_t canopen_init(canopen_t *canopen, canopen_role_t role, uint8_t node_id, uint32_t ide);
canopen_state_t canopen_process_tx(canopen_t *canopen);
canopen_state_t canopen_process_rx(canopen_t *canopen);
canopen_state_t canopen_config_node_id(canopen_t *canopen, uint8_t node_id);
canopen_state_t canopen_config_callback(canopen_t *canopen, uint32_t id, uint8_t fifo, canopen_callback callback);
canopen_state_t canopen_isr_handler(canopen_t *canopen, uint32_t fifo);
// void canopen_config_filter_mask(CANopen *canopen, uint32_t id1, uint32_t mask, uint8_t fifo); // TODO

canopen_state_t is_valid_fifo(canopen_t *canopen, uint8_t fifo);

#endif // CAN_OPEN_H
