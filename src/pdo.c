#include "pdo.h"
#include "fifo.h"

#define PDO_TX_BASE(pdo_num) (0x180 + (((pdo_num) - 1) * 0x100))
#define PDO_RX_BASE(pdo_num) (0x200 + (((pdo_num) - 1) * 0x100))

#define PDO_TX_TYPE(pdo_num) ((msg_type_t)(TYPE_PDO1_TX + ((pdo_num) - 1)))
#define PDO_RX_TYPE(pdo_num) ((msg_type_t)(TYPE_PDO1_RX + ((pdo_num) - 1)))

canopen_state_t canopen_send_pdo(canopen_t *canopen, canopen_msg_t *msg, canopen_pdo_data_t *data)
{
  if (canopen == NULL || msg == NULL)
    return CANOPEN_ERROR;

  canopen->info.tx_pdo_count++;
  memcpy(&msg->frame.pdo.data, data, msg->dlc);
  fifo_state_t fifo_state = fifo_push(&canopen->fifo_tx, msg);
  if (fifo_state == FIFO_FULL)
  {
    canopen->info.tx_pdo_lost_count++;
    return CANOPEN_ERROR;
  }

  return CANOPEN_OK;
}

canopen_state_t canopen_config_pdo_tx(canopen_t *canopen, canopen_msg_t *msg, pdo_role_t pdo_dir, pdo_number_t pdo_num, uint8_t node_id, uint8_t dlc)
{
  // TODO Более конкретные проверки
  if (msg == NULL || canopen == NULL || node_id == 0 || node_id > 127)
    return CANOPEN_ERROR;

  if (dlc > COB_SIZE_PDO)
    dlc = COB_SIZE_PDO;

  msg->id = (pdo_dir == PDO_SERVER) ? (PDO_TX_BASE(pdo_num) + node_id) : (PDO_RX_BASE(pdo_num) + node_id);
  msg->type = (pdo_dir == PDO_SERVER) ? PDO_TX_TYPE(pdo_num) : PDO_RX_TYPE(pdo_num);
  msg->dlc = dlc;
  memset(&msg->frame.pdo.data, 0, COB_SIZE_PDO);

  return CANOPEN_OK;
}

canopen_state_t canopen_config_pdo_rx(canopen_t *canopen, pdo_role_t pdo_dir, pdo_number_t pdo_num, uint8_t node_id, canopen_callback callback)
{
  if (canopen == NULL || node_id == 0 || node_id > 127)
    return CANOPEN_ERROR;

  uint32_t id = (pdo_dir == PDO_SERVER) ? (PDO_RX_BASE(pdo_num) + node_id) : (PDO_TX_BASE(pdo_num) + node_id);
  return canopen_config_callback(canopen, id, 0, callback);
}
