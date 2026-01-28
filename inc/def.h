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
    ID_NMT = 0x000,
    ID_SYNC = 0x080,
    ID_EMCY = 0x080,
    ID_TIMESTAMP = 0x100,
    ID_TPDO1 = 0x180,
    ID_RPDO1 = 0x200,
    ID_TPDO2 = 0x280,
    ID_RPDO2 = 0x300,
    ID_TPDO3 = 0x380,
    ID_RPDO3 = 0x400,
    ID_TPDO4 = 0x480,
    ID_RPDO4 = 0x500,
    ID_TSDO = 0x580,
    ID_RSDO = 0x600,
    ID_HEARTBEAT = 0x700,
    ID_RTR = 0x000,
    ID_TLSS = 0x7E5,
    ID_RLSS = 0x7E4,
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
    uint8_t data[COB_SIZE_DEF];
} co_msg_t;

#endif // DEF_H
