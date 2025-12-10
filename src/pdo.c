#include "pdo.h"
#include "fifo.h"

/*
 * PDO Base CAN IDs
 * ┌─────────┬────────────┬────────────────────────────┐
 * │ PDO Num │ Base ID    │ Full CAN ID Formula        │
 * ├─────────┼────────────┼────────────────────────────┤
 * │ PDO1 TX │ 0x180      │ 0x180 + NodeID             │
 * │ PDO2 TX │ 0x280      │ 0x280 + NodeID             │
 * │ PDO3 TX │ 0x380      │ 0x380 + NodeID             │
 * │ PDO4 TX │ 0x480      │ 0x480 + NodeID             │
 * │ PDO1 RX │ 0x200      │ 0x200 + NodeID             │
 * │ PDO2 RX │ 0x300      │ 0x300 + NodeID             │
 * │ PDO3 RX │ 0x400      │ 0x400 + NodeID             │
 * │ PDO4 RX │ 0x500      │ 0x500 + NodeID             │
 * └─────────┴────────────┴────────────────────────────┘
 */

static const uint16_t pdo_tx_base_ids[] = {
    0x180, // PDO1 Transmit
    0x280, // PDO2 Transmit
    0x380, // PDO3 Transmit
    0x480, // PDO4 Transmit
};

static const uint16_t pdo_rx_base_ids[] = {
    0x200, // PDO1 Receive
    0x300, // PDO2 Receive
    0x400, // PDO3 Receive
    0x500, // PDO4 Receive
};

static const msg_type_t pdo_tx_types[] = {
    TYPE_PDO1_TX,
    TYPE_PDO2_TX,
    TYPE_PDO3_TX,
    TYPE_PDO4_TX,
};

canopen_state_t canopen_send_pdo(canopen_t *canopen, canopen_msg_t *msg, uint8_t *data, uint8_t size)
{
  if (canopen == NULL || msg == NULL)
    return CANOPEN_ERROR;

  canopen->info.tx_pdo_count++;
  fifo_state_t fifo_state = fifo_push(&canopen->fifo_tx, msg);
  if (fifo_state == FIFO_FULL)
  {
    canopen->info.tx_pdo_lost_count++;
    return CANOPEN_ERROR;
  }

  return CANOPEN_OK;
}

canopen_state_t canopen_config_pdo_tx(canopen_t *canopen, canopen_msg_t *msg, pdo_number_t pdo_num, uint8_t node_id, uint8_t dlc)
{
  // TODO Более конкретные проверки
  if (msg == NULL || canopen == NULL || node_id == 0 || node_id > 127)
    return CANOPEN_ERROR;

  if (dlc > COB_SIZE_PDO)
    dlc = COB_SIZE_PDO;

  uint8_t index = pdo_num - 1;
  msg->id = pdo_tx_base_ids[index] + node_id;
  msg->type = pdo_tx_types[index];
  msg->dlc = dlc;
  memset(msg->frame.pdo.data, 0, COB_SIZE_PDO);

  return CANOPEN_OK;
}

canopen_state_t canopen_config_pdo_rx(canopen_t *canopen, pdo_number_t pdo_num, uint8_t node_id, canopen_callback callback)
{
  if (canopen == NULL || node_id == 0 || node_id > 127)
    return CANOPEN_ERROR;

  uint8_t index = pdo_num - 1;
  uint32_t id = pdo_rx_base_ids[index] + node_id;
  return canopen_config_callback(canopen, id, 0, callback);
}
