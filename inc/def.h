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
    NMT = 0x000,
    SYNC_MESSAGE = 0x080,
    EMERGENCY_START = 0x080,
    TIME_MESSAGE = 0x100,
    PDO1 = 0x180,
    PDO2 = 0x280,
    PDO3 = 0x380,
    PDO4 = 0x480,
    SDO_RX = 0x580,
    SDO_TX = 0x600,
    HEARTBEAT = 0x700,
    LSS_TX = 0x7E4, // TODO Посмотреть, что это
    LSS_RX = 0x7E5,
} canopen_fc_t;

typedef enum {
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

typedef struct {
    union {
        uint64_t u64;

        struct {
            uint32_t low;
            uint32_t high;
        };

        struct {
            uint16_t word0;
            uint16_t word1;
            uint16_t word2;
            uint16_t word3;
        };

        struct {
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

typedef struct {
    uint8_t cmd;       // 1 байт
    uint16_t index;    // 2-3 байты
    uint8_t sub_index; // 4 байт
    uint32_t data;     // 5-8 байты
} co_sdo_t;

typedef struct {
    uint8_t cmd;
    uint8_t node_id;
} co_nmt_t;

#pragma pack(push, 1)
typedef union {
    uint8_t row[COB_SIZE_PDO];
    co_nmt_t nmt;
    co_pdo_t pdo;
    co_sdo_t sdo;
} cob_frame_t;
#pragma pack(pop)

typedef union {
    uint16_t all;
    struct {
        uint16_t sdo_pending : 1;
        uint16_t pdo_pending : 1;
        uint16_t nmt_pending : 1;
        uint16_t reserve : 13;
    } bit;
} timeout_staus_t;

typedef struct {
    bool online;
    uint32_t time;
    uint32_t timeout;
} co_timeout_t;

typedef struct {
    uint8_t id;
    co_timeout_t pdo;
    co_timeout_t sdo;
    co_timeout_t heartbeat;
    // canopen_nmt_state_t nmt_state; // TODO расширить и на PDO + SDO
} co_node_t;

typedef struct {
    uint32_t id;
    uint8_t dlc;
    co_msg_type_t type;
    cob_frame_t frame;
    // co_node_t *node;
} co_msg_t;

#endif // DEF_H
