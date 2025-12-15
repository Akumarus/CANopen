#ifndef DEF_H
#define DEF_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

#define COB_SIZE_DEF 8
#define COB_SIZE_PDO COB_SIZE_DEF // communication object
#define COB_SIZE_SDO COB_SIZE_DEF // communication object
#define MAX_BANK_COUNT 14
#define IDS_PER_BANK 4
#define MAX_CALLBACKS 10
#define MAX_11BIT_ID 0x7FF
#define CAN_FIFO_SIZE 32
#define CANOPEN_TIMEOUT 1000

typedef enum
{
  NMT_BROADCAST = 0x000,
  SYNC_MESSAGE = 0x080,
  EMERGENCY_START = 0x080,
  TIME_MESSAGE = 0x100,
  PDO1 = 0x180,
  PDO2 = 0x280,
  PDO3 = 0x380,
  PDO4 = 0x480,
  SDO_RX = 0x580,
  SDO_TX = 0x600,
  HEARTBIT_START = 0x700,
  LSS_TX = 0x7E4, // TODO Посмотреть, что это
  LSS_RX = 0x7E5,
} canopen_fc_t;

typedef enum
{
  TYPE_UNKNOWN = 0,
  TYPE_NMT,
  TYPE_SYNC,
  TYPE_EMCY,
  TYPE_TIMESTAMP,
  TYPE_PDO1_TX,
  TYPE_PDO1_RX,
  TYPE_PDO2_TX,
  TYPE_PDO2_RX,
  TYPE_PDO3_TX,
  TYPE_PDO3_RX,
  TYPE_PDO4_TX,
  TYPE_PDO4_RX,
  TYPE_SDO_TX,
  TYPE_SDO_RX,
  TYPE_HEARTBEAT,
  TYPE_RTR,
  TYPE_LSS_TX,
  TYPE_LSS_RX,
} msg_type_t;

typedef enum
{
  CANOPEN_OK = 0,
  CANOPEN_ERROR,
} canopen_state_t;

typedef struct
{
  union
  {
    uint64_t u64;

    struct
    {
      uint32_t low;
      uint32_t high;
    };

    struct
    {
      uint16_t word0;
      uint16_t word1;
      uint16_t word2;
      uint16_t word3;
    };

    struct
    {
      uint8_t byte0;
      uint8_t byte1;
      uint8_t byte2;
      uint8_t byte3;
      uint8_t byte4;
      uint8_t byte5;
      uint8_t byte6;
      uint8_t byte7;
    };
  };
} canopen_pdo_data_t;

static_assert(sizeof(canopen_pdo_data_t) == 8, "canopen_pdo_data_t must be exactly 8 bytes for CAN PDO!");

typedef struct
{
  uint8_t cmd;       // 1 байт
  uint16_t index;    // 2-3 байты
  uint8_t sub_index; // 4 байт
  uint32_t data;     // 5-8 байты
} canopen_sdo_data_t;

#pragma pack(push, 1)
typedef union
{
  struct
  {
    uint8_t data[COB_SIZE_PDO]; // 1-8 байт
  } row;
  struct
  {
    canopen_pdo_data_t data; // 1-8 байт
  } pdo;
  canopen_sdo_data_t sdo;
} cob_frame_t;
#pragma pack(pop)

typedef union
{
  uint16_t all;
  struct
  {
    uint16_t sdo_pending : 1;
    uint16_t pdo_pending : 1;
    uint16_t nmt_pending : 1;
    uint16_t reserve : 13;
  } bit;
} timeout_staus_t;

typedef struct
{
  uint8_t id;
  timeout_staus_t status;
  uint32_t pdo_timestamp;
  uint32_t sdo_timestamp;
  uint32_t nmt_timestamp;
} canopen_node_t;

typedef struct
{
  uint32_t id;
  uint8_t dlc;
  msg_type_t type;
  cob_frame_t frame;
  canopen_node_t *node;
} canopen_msg_t;

#endif // DEF_H
