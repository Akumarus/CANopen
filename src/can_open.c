#include "can_open.h"
#include "port.h"
#include <string.h>


static uint8_t find_free_bank(CANopen *canopen, uint16_t id, uint8_t fifo);

void canopen_init(CANopen *canopen, uint32_t ide)
{
  if (canopen == NULL)
    return;

  canopen->ide = ide;
  canopen->info.bank_count = 0;
  canopen->info.status.all = 0;
  canopen->info.callbacks_count = 0;
  
  for (uint8_t i = 0; i < MAX_CALLBACKS; i++)
  {
    canopen->callbacks[i].id = 0;
    canopen->callbacks[i].callback = NULL;
  }

  for (uint8_t i = 0; i < MAX_BANK_COUNT; i++)
    canopen->bank_list[i].fifo_assignment = 0xFF;

  CAN_FIFO_config tx_fifo_cnfg = {.message = canopen->tx_buff, .size = CAN_FIFO_SIZE};
  CAN_FIFO_config rx_fifo_cnfg = {.message = canopen->rx_buff, .size = CAN_FIFO_SIZE};
  can_fifo_init(&canopen->tx_fifo, tx_fifo_cnfg);
  can_fifo_init(&canopen->rx_fifo, rx_fifo_cnfg);

  can_init();
}


CANopen_State canopen_config_filter_list_16b(CANopen *canopen, uint16_t id, uint8_t fifo)
{
  CANopen_State res = CANOPEN_ERROR;

  if (canopen == NULL)
    return res;
    
  if (is_valid_id(canopen, id) != CANOPEN_OK)
    return res;
  
  if (is_valid_fifo(canopen, fifo) != CANOPEN_OK)
    return res;

  uint8_t bank_num = find_free_bank(canopen, id, fifo);
  
  if (is_valid_bank(canopen, bank_num) != CANOPEN_OK)
    return res;

  CANopenFilterConfig filter = {0};
  filter.bank      =  bank_num;
  filter.mode      =  COB_FILTERMODE_IDLIST;
  filter.scale     =  COB_FILTERSCALE_16BIT;
  filter.id_high   =  canopen->bank_list[bank_num].ids[0];
  filter.id_low    = (canopen->bank_list[bank_num].used_count > 1) ? canopen->bank_list[bank_num].ids[1] : 0;
  filter.mask_high = (canopen->bank_list[bank_num].used_count > 2) ? canopen->bank_list[bank_num].ids[2] : 0;
  filter.mask_low  = (canopen->bank_list[bank_num].used_count > 3) ? canopen->bank_list[bank_num].ids[3] : 0;
  filter.fifo      = fifo;
  filter.active    = 1;
  filter.end_bank  = MAX_BANK_COUNT;
  can_conf_filter(&filter);
  return CANOPEN_OK;
}


static uint8_t find_free_bank(CANopen *canopen, uint16_t id, uint8_t fifo) 
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

// void CANopen_config_filter_mask(CANopen *canopen, uint32_t id1,  uint32_t mask, uint8_t fifo)
// {
//   CANopenFilterConfig filter;
//   filter.mode = COB_FILTERMODE_IDMASK;
//   filter.scale = COB_FILTERSCALE_16BIT;
//   filter.id_high = id1;
//   filter.id_low = 0;
//   filter.mask_high = mask;
//   filter.mask_low = 0;
//   filter.fifo = fifo;
//   filter.active = 1;  // TODO Не реализовано
//   filter.end_bank = 14; // TODO по дефолту 14
//   can_conf_filter(&filter);
// }

void canopen_process_tx(CANopen *canopen) 
{
  if (can_fifo_is_empty(&canopen->tx_fifo))
    return;

  canopen_message_t frame = {0};
  if (can_fifo_pop(&canopen->tx_fifo, &frame) == FIFO_CAN_OK)
  {
    can_send_packet(frame.id, COB_RTR_DATA, canopen->ide, frame.dlc, frame.frame.row.data);
    if (frame.type == TYPE_PDO)
      canopen->info.tx_sended_pdo_count++;
  }
}


// void CANopen_send_sdo(CANopen *canopen, CANopen_SDO *sdo)
// {
//   can_send_packet(sdo->id, COB_RTR_DATA, canopen->ide, COB_SIZE_DEF, sdo->data);
// }


CANopen_State canopen_config_sdo_tx(CANopen *canopen, uint32_t id, canopen_message_t *msg)
{
  CANopen_State res = CANOPEN_ERROR;

  if (msg == NULL || canopen == NULL)
    return res;
  
  if (is_valid_id(canopen, id) != CANOPEN_OK)
    return res;

  msg->id = id;
  msg->dlc = 8;
  memset(&msg->frame.sdo.data, 0, sizeof(cob_frame_t));

  res = CANOPEN_OK;
  return res;
}


CANopen_State canopen_config_callback(CANopen *canopen, uint32_t id, uint8_t fifo, canopen_callback callback)
{
  CANopen_State res = CANOPEN_ERROR;

  if(canopen->info.callbacks_count >= MAX_CALLBACKS)
    return res;

  /* Проверка, зарегистрирована ли PDO */
  for (uint8_t i = 0; i < canopen->info.callbacks_count; i++)
  {
    if (canopen->callbacks[i].id == id)
    {
      canopen->callbacks[i].callback = callback;
      return res; // TODO Другой статус
    }
  }

  /* Добавить новый обработчик */
  canopen->callbacks[canopen->info.callbacks_count].id = id;
  canopen->callbacks[canopen->info.callbacks_count].callback = callback;
  canopen->info.callbacks_count++;

  /* Настройка фильтра для приема */
  canopen_config_filter_list_16b(canopen, id, fifo); // TODO Пользовательская вещь?

  res = CANOPEN_OK;
  return res;
}

void canopen_process_rx_message(CANopen *canopen, uint32_t id, uint8_t *data, uint8_t dlc)
{
  for (uint8_t i = 0; i < canopen->info.callbacks_count; i++)
  {
    if ((canopen->callbacks[i].id == id) && (canopen->callbacks[i].callback != NULL))
    {
      canopen->callbacks[i].callback(id, data, dlc);
      return;
    }
  }
}

CANopen_State is_valid_id(CANopen *canopen, uint16_t id)
{
  CANopen_State res = CANOPEN_OK;

  if ((canopen->ide == COB_ID_STD) && (id >= MAX_11BIT_ID))
  {
    canopen->info.status.bit.invalid_id = 1;
    res = CANOPEN_ERROR;
  }

  return res;
}

CANopen_State is_valid_fifo(CANopen *canopen, uint8_t fifo)
{
  CANopen_State res = CANOPEN_OK;

  if ((fifo != COB_RX_FIFO0) && ((fifo != COB_RX_FIFO1)))
  {
    canopen->info.status.bit.invalid_fifo = 1;
    res = CANOPEN_ERROR;
  }
  
  return res;
}

CANopen_State is_valid_bank(CANopen *canopen, uint8_t bank)
{
  CANopen_State res = CANOPEN_OK;

  if (bank == 0xFF)
  {
    canopen->info.status.bit.filter_banks_full = 1;
    return CANOPEN_ERROR;
  }

  return res;
}

