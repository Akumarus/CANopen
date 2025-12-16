#include "can_open.h"
#include "port.h"
#include "sdo.h"
#include "pdo.h"

static canopen_state_t canopen_config_filter_list_16b(canopen_t *canopen, uint32_t id, uint8_t fifo);
static uint8_t find_free_bank(canopen_t *canopen, uint16_t id, uint8_t fifo);
static canopen_state_t is_callback_register(canopen_t *canopen, uint32_t id);
static canopen_state_t is_valid_bank(canopen_t *canopen, uint8_t bank);
static msg_type_t canopen_msg_type_from_id(uint32_t id);
static void init_nodes(canopen_t *canopen);
static void init_filters(canopen_t *canopen);
static void init_fifo(canopen_t *canopen);
static void init_callbacks(canopen_t *canopen);

canopen_state_t canopen_init(canopen_t *canopen, canopen_role_t role, uint8_t node_id, uint32_t ide)
{
  if (canopen == NULL)
    return CANOPEN_ERROR;

  canopen->ide = ide;
  canopen->role = role;
  canopen->node_id = node_id;
  canopen->info.bank_count = 0;
  canopen->info.status.all = 0;
  canopen->info.callbacks_count = 0;

  init_fifo(canopen);
  init_nodes(canopen);
  init_filters(canopen);
  init_callbacks(canopen);
  port_can_init();

  return CANOPEN_OK;
}

canopen_state_t canopen_config_node_id(canopen_t *canopen, uint8_t node_id)
{
  for (uint8_t i = 0; i < NODES_COUNT; i++) // TODO быстрый перебор
  {
    if (canopen->node[i].id == 0xFF)
    {
      canopen->node[i].id = node_id;
      return CANOPEN_OK;
    }
  }
  return CANOPEN_ERROR; // TODO Добавить статус
}

canopen_state_t canopen_config_callback(canopen_t *canopen, uint32_t id, uint8_t fifo, canopen_callback callback)
{
  // TODO проверка id
  if (is_valid_fifo(canopen, fifo) != CANOPEN_OK)
  {
    canopen->info.status.bit.invalid_fifo = 1;
    return CANOPEN_ERROR;
  }

  if (canopen->info.callbacks_count >= MAX_CALLBACKS)
  {
    canopen->info.status.bit.callbacks_full = 1;
    return CANOPEN_ERROR;
  }

  if (is_callback_register(canopen, id))
    return CANOPEN_ERROR;

  canopen->callbacks[canopen->info.callbacks_count].id = id;
  canopen->callbacks[canopen->info.callbacks_count].callback = callback;
  canopen->info.callbacks_count++;

  if (canopen_config_filter_list_16b(canopen, id, fifo) != CANOPEN_OK)
    return CANOPEN_ERROR;

  return CANOPEN_OK;
}

canopen_state_t canopen_process_tx(canopen_t *canopen)
{
  if (canopen == NULL)
    return CANOPEN_ERROR;

  if (port_get_free_mailboxes() == 0)
  {
    canopen->info.tx_busy_mailbox_count++;
    return CANOPEN_ERROR;
  }

  canopen_msg_t msg = {0};
  if (fifo_pop(&canopen->fifo_tx, &msg) == FIFO_OK)
  {
    port_can_send(msg.id, COB_RTR_DATA, canopen->ide, msg.dlc, msg.frame.row.data);
    if (msg.type == TYPE_PDO1_TX)
      canopen->info.tx_sended_pdo_count++;
    return CANOPEN_OK;
  }

  return CANOPEN_ERROR;
}

canopen_state_t canopen_process_rx(canopen_t *canopen)
{
  if (canopen == NULL)
    return CANOPEN_ERROR;

  while (!fifo_is_empty(&canopen->fifo_rx))
  {
    canopen_msg_t msg = {0};
    if (fifo_pop(&canopen->fifo_rx, &msg) != FIFO_OK)
      return CANOPEN_ERROR;

    // // TODO Нужно вынести в отдельную функцию
    for (uint8_t i = 0; i < MAX_CALLBACKS; i++)
    {
      if ((canopen->callbacks[i].id == msg.id) &&
          (canopen->callbacks[i].callback != NULL))
      {
        canopen->callbacks[i].callback(&msg);
      }
    }

    switch (msg.type)
    {
    case TYPE_SDO_TX:
    case TYPE_SDO_RX:
      canopen_sdo_process(canopen, &msg);
      break;
    case TYPE_PDO1_TX:
      break;
    case TYPE_NMT:
      break;
    case TYPE_SYNC:
      break;
    case TYPE_EMCY:
      break;
    case TYPE_HEARTBEAT:
      break;
    default:
      break;
    }
  }

  return CANOPEN_OK;
}

canopen_state_t canopen_check_timeouts(canopen_t *canopen)
{
  assert(canopen != NULL);

  uint32_t current_time = port_get_timestamp() - canopen->timestamp;

  for (uint8_t i = 0; i < NODES_COUNT; i++)
  {
    canopen_node_t *node = &canopen->node[i];

    if (node->id == 0xFF) // TODO задачть через дефайн или переменную
      break;

    if (node->sdo_timestamp != 0)
    {
      uint32_t elapsed;
      if (current_time >= node->sdo_timestamp)
        elapsed = current_time - node->sdo_timestamp;
      else
        elapsed = (UINT32_MAX - node->sdo_timestamp) + current_time + 1;

      if (elapsed >= 1000) // TODO задачть через дефайн или переменную
      {
        node->sdo_timestamp = 0;
        return CANOPEN_ERROR;
      }
    }
  }

  return CANOPEN_OK;
}

canopen_state_t canopen_isr_handler(canopen_t *canopen, uint32_t fifo)
{
  if (canopen == NULL)
    return CANOPEN_ERROR;

  canopen_msg_t msg;
  uint8_t data[COB_SIZE_DEF];
  uint32_t id;
  uint8_t dlc;

  if (port_can_receive_message(&id, data, &dlc, fifo))
  {
    msg.id = id;
    msg.dlc = dlc;
    msg.type = canopen_msg_type_from_id(id);
    memcpy(msg.frame.row.data, data, dlc);

    if (fifo_push(&canopen->fifo_rx, &msg) != FIFO_OK)
    {
      canopen->info.fifo_rx_overflow_counter++;
      return CANOPEN_ERROR;
    }
    canopen->info.fifo_rx_complete_counter++;
  }
  return CANOPEN_OK;
}

static canopen_state_t canopen_config_filter_list_16b(canopen_t *canopen, uint32_t id, uint8_t fifo)
{
  uint8_t bank_num = find_free_bank(canopen, id, fifo);

  if (is_valid_bank(canopen, bank_num) != CANOPEN_OK)
    return CANOPEN_ERROR;

  canopen_filter_t filter = {0};
  filter.bank = bank_num;
  filter.mode = COB_FILTERMODE_IDLIST;
  filter.scale = COB_FILTERSCALE_16BIT;
  filter.id_high = canopen->bank_list[bank_num].ids[0];
  filter.id_low = (canopen->bank_list[bank_num].used_count > 1) ? canopen->bank_list[bank_num].ids[1] : 0;
  filter.mask_high = (canopen->bank_list[bank_num].used_count > 2) ? canopen->bank_list[bank_num].ids[2] : 0;
  filter.mask_low = (canopen->bank_list[bank_num].used_count > 3) ? canopen->bank_list[bank_num].ids[3] : 0;
  filter.fifo = fifo;
  filter.active = 1;
  filter.end_bank = MAX_BANK_COUNT;
  port_can_init_filter(&filter);

  return CANOPEN_OK;
}

static uint8_t find_free_bank(canopen_t *canopen, uint16_t id, uint8_t fifo)
{
  uint8_t bank_num = 0xFF;
  for (uint8_t i = 0; i < MAX_BANK_COUNT; i++)
  {
    if ((canopen->bank_list[i].fifo_assignment == fifo &&
         canopen->bank_list[i].used_count < IDS_PER_BANK) ||
        canopen->bank_list[i].used_count == 0)
    {
      if (canopen->bank_list[i].fifo_assignment == 0xFF)
        canopen->info.bank_count++;
      canopen->bank_list[i].fifo_assignment = fifo;
      canopen->bank_list[i].ids[canopen->bank_list[i].used_count] = id << 5;
      canopen->bank_list[i].used_count++;
      bank_num = i;
      break;
    }
  }
  return bank_num;
}

static canopen_state_t is_callback_register(canopen_t *canopen, uint32_t id)
{
  for (uint8_t i = 0; i < canopen->info.callbacks_count; i++)
  {
    if (canopen->callbacks[i].id == id && canopen->callbacks[i].callback == NULL)
      return CANOPEN_ERROR;
  }
  return CANOPEN_OK;
}

static void init_callbacks(canopen_t *canopen)
{
  for (uint8_t i = 0; i < MAX_CALLBACKS; i++)
  {
    canopen->callbacks[i].id = 0;
    canopen->callbacks[i].callback = NULL;
  }
}

static void init_nodes(canopen_t *canopen)
{
  for (uint8_t i = 0; i < NODES_COUNT; i++)
  {
    canopen->node[i].id = 0xFF;
    canopen->node[i].status.all = 0;
    canopen->node[i].sdo_timestamp = 0;
    canopen->node[i].pdo_timestamp = 0;
    canopen->node[i].nmt_timestamp = 0;
  }
}

static void init_filters(canopen_t *canopen)
{
  for (uint8_t i = 0; i < MAX_BANK_COUNT; i++)
    canopen->bank_list[i].fifo_assignment = 0xFF;
}

static void init_fifo(canopen_t *canopen)
{
  fifo_config_t fifo_tx_config = {.message = canopen->buffer_tx, .size = CAN_FIFO_SIZE};
  fifo_config_t fifo_rx_config = {.message = canopen->buffer_rx, .size = CAN_FIFO_SIZE};
  fifo_init(&canopen->fifo_tx, fifo_tx_config);
  fifo_init(&canopen->fifo_rx, fifo_rx_config);
}

static msg_type_t canopen_msg_type_from_id(uint32_t id)
{
  switch (id)
  {
  case 0x000:
    return TYPE_NMT;
  case 0x080:
    return TYPE_SYNC;
  case 0x100:
    return TYPE_TIMESTAMP;
  case 0x7E4:
    return TYPE_LSS_RX;
  case 0x7E5:
    return TYPE_LSS_TX;
  }

  // uint8_t node_id = id & 0x07F;
  uint16_t base_id = id & 0x780;

  switch (base_id)
  {
  case 0x080:
    if (id >= 0x081 && id <= 0x0FF)
      return TYPE_EMCY;
    break;
  case 0x180:
    return TYPE_PDO1_TX;
  case 0x200:
    return TYPE_PDO1_RX;
  case 0x280:
    return TYPE_PDO1_TX;
  case 0x300:
    return TYPE_PDO1_RX;
  case 0x380:
    return TYPE_PDO1_TX;
  case 0x400:
    return TYPE_PDO1_RX;
  case 0x480:
    return TYPE_PDO1_TX;
  case 0x500:
    return TYPE_PDO1_RX;
  case 0x580:
    return TYPE_SDO_RX;
  case 0x600:
    return TYPE_SDO_TX;
  case 0x700:
    return TYPE_HEARTBEAT;
  }
  return TYPE_UNKNOWN;
}

// // void CANopen_config_filter_mask(CANopen *canopen, uint32_t id1,  uint32_t mask, uint8_t fifo)
// // {
// //   CANopenFilterConfig filter;
// //   filter.mode = COB_FILTERMODE_IDMASK;
// //   filter.scale = COB_FILTERSCALE_16BIT;
// //   filter.id_high = id1;
// //   filter.id_low = 0;
// //   filter.mask_high = mask;
// //   filter.mask_low = 0;
// //   filter.fifo = fifo;
// //   filter.active = 1;  // TODO Не реализовано
// //   filter.end_bank = 14; // TODO по дефолту 14
// //   can_conf_filter(&filter);
// // }

// void canopen_process_rx_message(CANopen *canopen, uint32_t id, uint8_t *data, uint8_t dlc)
// {
//   for (uint8_t i = 0; i < canopen->info.callbacks_count; i++)
//   {
//     if ((canopen->callbacks[i].id == id) && (canopen->callbacks[i].callback != NULL))
//     {
//       canopen->callbacks[i].callback(id, data, dlc);
//       return;
//     }
//   }
// }

// CANopen_State is_valid_id(CANopen *canopen, uint16_t id)
// {
//   CANopen_State res = CANOPEN_OK;

//   if ((canopen->ide == COB_ID_STD) && (id >= MAX_11BIT_ID))
//   {
//     canopen->info.status.bit.invalid_id = 1;
//     res = CANOPEN_ERROR;
//   }

//   return res;
// }

canopen_state_t is_valid_fifo(canopen_t *canopen, uint8_t fifo)
{
  if ((fifo != COB_RX_FIFO0) && ((fifo != COB_RX_FIFO1)))
  {
    canopen->info.status.bit.invalid_fifo = 1;
    return CANOPEN_ERROR;
  }

  return CANOPEN_OK;
}

static canopen_state_t is_valid_bank(canopen_t *canopen, uint8_t bank)
{
  if (bank == 0xFF)
  {
    canopen->info.status.bit.filter_banks_full = 1;
    return CANOPEN_ERROR;
  }

  return CANOPEN_OK;
}
