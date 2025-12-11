#ifndef PDO_H
#define PDO_H

#include "def.h"
#include "can_open.h"

/* MASTER: ID для общения с Slave (NodeID = N)    */
/* ┌────────────────┬─────────────┬─────────────┐ */
/* │ Направление    │ Канал PDO   │ CAN ID      │ */
/* ├────────────────┼─────────────┼─────────────┤ */
/* │ Master ← Slave │ PDO1 TX     │ 0x180 + N   │ */
/* │ Master → Slave │ PDO1 RX     │ 0x200 + N   │ */
/* │ Master ← Slave │ PDO2 TX     │ 0x280 + N   │ */
/* │ Master → Slave │ PDO2 RX     │ 0x300 + N   │ */
/* │ Master ← Slave │ PDO3 TX     │ 0x380 + N   │ */
/* │ Master → Slave │ PDO3 RX     │ 0x400 + N   │ */
/* │ Master ← Slave │ PDO4 TX     │ 0x480 + N   │ */
/* │ Master → Slave │ PDO4 RX     │ 0x500 + N   │ */
/* └────────────────┴─────────────┴─────────────┘ */

/* ┌────────────────┬─────────────┬─────────────┐ */
/* │ Направление    │ Канал PDO   │ CAN ID      │ */
/* ├────────────────┼─────────────┼─────────────┤ */
/* │ Slave → Master │ PDO1 TX     │ 0x180 + M   │ */
/* │ Slave ← Master │ PDO1 RX     │ 0x200 + M   │ */
/* │ Slave → Master │ PDO2 TX     │ 0x280 + M   │ */
/* │ Slave ← Master │ PDO2 RX     │ 0x300 + M   │ */
/* │ Slave → Master │ PDO3 TX     │ 0x380 + M   │ */
/* │ Slave ← Master │ PDO3 RX     │ 0x400 + M   │ */
/* │ Slave → Master │ PDO4 TX     │ 0x480 + M   │ */
/* │ Slave ← Master │ PDO4 RX     │ 0x500 + M   │ */
/* └────────────────┴─────────────┴─────────────┘ */

typedef enum
{
  PDO_NUM_1 = 1,
  PDO_NUM_2 = 2,
  PDO_NUM_3 = 3,
  PDO_NUM_4 = 4,
} pdo_number_t;

typedef enum
{
  PDO_CLIENT = 0,
  PDO_SERVER = 1,
} pdo_role_t;

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

/* Client PDO Messages -------------------------------------------------*/
#define canopen_client_config_pdo1_tx(canopen, msg, node_id, dlc) \
  canopen_config_pdo_tx(canopen, msg, PDO_CLIENT, PDO_NUM_1, node_id, dlc)
#define canopen_client_config_pdo2_tx(canopen, msg, node_id, dlc) \
  canopen_config_pdo_tx(canopen, msg, PDO_CLIENT, PDO_NUM_2, node_id, dlc)
#define canopen_client_config_pdo3_tx(canopen, msg, node_id, dlc) \
  canopen_config_pdo_tx(canopen, msg, PDO_CLIENT, PDO_NUM_3, node_id, dlc)
#define canopen_client_config_pdo4_tx(canopen, msg, node_id, dlc) \
  canopen_config_pdo_tx(canopen, msg, PDO_CLIENT, PDO_NUM_4, node_id, dlc)

#define canopen_client_config_pdo1_rx(canopen, node_id, callback) \
  canopen_config_pdo_rx(canopen, PDO_CLIENT, PDO_NUM_1, node_id, callback)
#define canopen_client_config_pdo2_rx(canopen, node_id, callback) \
  canopen_config_pdo_rx(canopen, PDO_CLIENT, PDO_NUM_2, node_id, callback)
#define canopen_client_config_pdo3_rx(canopen, node_id, callback) \
  canopen_config_pdo_rx(canopen, PDO_CLIENT, PDO_NUM_3, node_id, callback)
#define canopen_client_config_pdo4_rx(canopen, node_id, callback) \
  canopen_config_pdo_rx(canopen, PDO_CLIENT, PDO_NUM_4, node_id, callback)

/* Server PDO Messages -------------------------------------------------*/
#define canopen_server_config_pdo1_tx(canopen, msg, node_id, dlc) \
  canopen_config_pdo_tx(canopen, msg, PDO_SERVER, PDO_NUM_1, node_id, dlc)
#define canopen_server_config_pdo2_tx(canopen, msg, node_id, dlc) \
  canopen_config_pdo_tx(canopen, msg, PDO_SERVER, PDO_NUM_2, node_id, dlc)
#define canopen_server_config_pdo3_tx(canopen, msg, node_id, dlc) \
  canopen_config_pdo_tx(canopen, msg, PDO_SERVER, PDO_NUM_3, node_id, dlc)
#define canopen_server_config_pdo4_tx(canopen, msg, node_id, dlc) \
  canopen_config_pdo_tx(canopen, msg, PDO_SERVER, PDO_NUM_4, node_id, dlc)

#define canopen_server_config_pdo1_rx(canopen, node_id, callback) \
  canopen_config_pdo_rx(canopen, PDO_SERVER, PDO_NUM_1, node_id, callback)
#define canopen_server_config_pdo2_rx(canopen, node_id, callback) \
  canopen_config_pdo_rx(canopen, PDO_SERVER, PDO_NUM_2, node_id, callback)
#define canopen_server_config_pdo3_rx(canopen, node_id, callback) \
  canopen_config_pdo_rx(canopen, PDO_SERVER, PDO_NUM_3, node_id, callback)
#define canopen_server_config_pdo4_rx(canopen, node_id, callback) \
  canopen_config_pdo_rx(canopen, PDO_SERVER, PDO_NUM_4, node_id, callback)

canopen_state_t canopen_send_pdo(canopen_t *canopen, canopen_msg_t *msg, canopen_pdo_data_t *data);

canopen_state_t canopen_config_pdo_rx(canopen_t *canopen, pdo_role_t pdo_dir, pdo_number_t pdo_num, uint8_t node_id, canopen_callback callback);
canopen_state_t canopen_config_pdo_tx(canopen_t *canopen, canopen_msg_t *msg, pdo_role_t pdo_dir, pdo_number_t pdo_num, uint8_t node_id, uint8_t dlc);
#endif // PDO_H
