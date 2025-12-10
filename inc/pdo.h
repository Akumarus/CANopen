#ifndef PDO_H
#define PDO_H

#include "def.h"
#include "can_open.h"

typedef enum
{
  PDO_NUM_1 = 1,
  PDO_NUM_2 = 2,
  PDO_NUM_3 = 3,
  PDO_NUM_4 = 4,
} pdo_number_t;

#define canopen_config_pdo1_tx(canopen, msg, node_id, dlc) \
  canopen_config_pdo_tx(canopen, msg, PDO_NUM_1, node_id, dlc)
#define canopen_config_pdo2_tx(canopen, msg, node_id, dlc) \
  canopen_config_pdo_tx(canopen, msg, PDO_NUM_2, node_id, dlc)
#define canopen_config_pdo3_tx(canopen, msg, node_id, dlc) \
  canopen_config_pdo_tx(canopen, msg, PDO_NUM_3, node_id, dlc)
#define canopen_config_pdo4_tx(canopen, msg, node_id, dlc) \
  canopen_config_pdo_tx(canopen, msg, PDO_NUM_4, node_id, dlc)

#define canopen_config_pdo1_rx(canopen, node_id, callback) \
  canopen_config_pdo_rx(canopen, PDO_NUM_1, node_id, callback)
#define canopen_config_pdo2_rx(canopen, node_id, callback) \
  canopen_config_pdo_rx(canopen, PDO_NUM_2, node_id, callback)
#define canopen_config_pdo3_rx(canopen, node_id, callback) \
  canopen_config_pdo_rx(canopen, PDO_NUM_3, node_id, callback)
#define canopen_config_pdo4_rx(canopen, node_id, callback) \
  canopen_config_pdo_rx(canopen, PDO_NUM_4, node_id, callback)

canopen_state_t canopen_send_pdo(canopen_t *canopen, canopen_msg_t *msg, uint8_t *data, uint8_t size);

canopen_state_t canopen_config_pdo_rx(canopen_t *canopen, pdo_number_t pdo_num, uint8_t node_id, canopen_callback callback);
canopen_state_t canopen_config_pdo_tx(canopen_t *canopen, canopen_msg_t *msg, pdo_number_t pdo_num, uint8_t node_id, uint8_t dlc);
#endif // PDO_H
