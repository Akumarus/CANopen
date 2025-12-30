#ifndef DEF_H
#define DEF_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define COB_SIZE_DEF 8
#define COB_SIZE_PDO COB_SIZE_DEF // communication object
#define COB_SIZE_SDO COB_SIZE_DEF // communication object
#define MAX_BANK_COUNT 14

#define MAX_CALLBACKS 10
#define MAX_11BIT_ID 0x7FF
#define CAN_FIFO_SIZE 32
#define CANOPEN_TIMEOUT 1000

typedef enum {
    COB_ID_NMT = 0x000,
    COB_ID_SYNC_MESSAGE = 0x080,
    COB_ID_EMERGENCY_START = 0x080,
    COB_ID_TIME_MESSAGE = 0x100,
    COB_ID_PDO1 = 0x180,
    COB_ID_PDO2 = 0x280,
    COB_ID_PDO3 = 0x380,
    COB_ID_PDO4 = 0x480,
    COB_ID_SDO_RX = 0x580,
    COB_ID_SDO_TX = 0x600,
    COB_ID_HEARTBEAT = 0x700,
    COB_ID_LSS_TX = 0x7E4, // TODO Посмотреть, что это
    COB_ID_LSS_RX = 0x7E5,
} canopen_fc_t;

typedef enum {
    TYPE_NMT = 0x000,
    TYPE_SYNC = 0x080,
    TYPE_EMCY = 0x080,
    TYPE_TIMESTAMP = 0x100,
    TYPE_PDO1_TX = 0x180,
    TYPE_PDO1_RX = 0x200,
    TYPE_PDO2_TX = 0x280,
    TYPE_PDO2_RX = 0x300,
    TYPE_PDO3_TX = 0x380,
    TYPE_PDO3_RX = 0x400,
    TYPE_PDO4_TX = 0x480,
    TYPE_PDO4_RX = 0x500,
    TYPE_SDO_TX = 0x580,
    TYPE_SDO_RX = 0x600,
    TYPE_HEARTBEAT = 0x700,
    TYPE_RTR = 0x000,
    TYPE_LSS_TX = 0x7E5,
    TYPE_LSS_RX = 0x7E4,
} co_msg_type_t;

typedef enum {
    NMT_STATE_BOOTUP = 0x00,
    NMT_STATE_STOPPED = 0x04,
    NMT_STATE_OPERATIONAL = 0x05,
    NMT_STATE_PRE_OPERATIONAL = 0x7F,
    NMT_STATE_RESETING = 0x80
} co_nmt_state_t;

typedef enum {
    NMT_CS_START = 0x01,
    NMT_CS_STOP = 0x02,
    NMT_CS_PREOP = 0x80,
    NMT_CS_RESET = 0x81,
    NMT_CS_COM_RESET = 0x82,
} co_nmt_cmd_t;

typedef enum {
    CANOPEN_OK = 0,
    CANOPEN_ERROR,
} co_res_t;

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
} co_pdo_t;

static_assert(sizeof(co_pdo_t) == 8, "co_pdo_data_t must be exactly 8 bytes for CAN PDO!");

typedef struct
{
    uint8_t cmd;   // 1 байт
    uint16_t idx;  // 2-3 байты
    uint8_t sidx;  // 4 байт
    uint32_t data; // 5-8 байты
} co_sdo_t;

typedef struct
{
    uint8_t cmd;
    uint8_t node_id;
} co_nmt_t;

#pragma pack(push, 1)
typedef union
{
    uint8_t row[COB_SIZE_PDO];
    co_nmt_t nmt;
    co_pdo_t pdo;
    co_sdo_t sdo;
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
    bool online;
    uint32_t time;
    uint32_t timeout;
} co_timeout_t;

typedef struct
{
    uint8_t id;
    co_timeout_t pdo;
    co_timeout_t sdo;
    co_timeout_t heartbeat;
    co_nmt_state_t state;
    // canopen_nmt_state_t nmt_state; // TODO расширить и на PDO + SDO
} co_node_t;

typedef struct
{
    uint32_t id;
    uint8_t dlc;
    co_msg_type_t type;
    cob_frame_t frame;
    // co_node_t *node;
} co_msg_t;

#endif // DEF_H
