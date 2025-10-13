#include "can_open.h"
#include "port.h"
#include <string.h>

void CANopen_init(CANopen *canopen, uint32_t ide)
{
  if (canopen == NULL)
    return;

  memset(canopen, 0, sizeof(CANopen));
  canopen->ide = ide;
  can_init();
}


void CANopen_pdo_config(CANopen *canopen, CANopen_PDO *pdo, uint32_t id, uint32_t dlc)
{
  pdo->id = id;
  pdo->dlс = dlc;
  pdo->ide = canopen->ide;
  memset(pdo->data, 0, sizeof(pdo->data));
}


CANopen_Status CANopen_config_filter_list_16b(CANopen *canopen, uint16_t id1, uint16_t id2, uint16_t id3, uint16_t id4, uint8_t fifo)
{
  if (canopen->bank_count >= MAX_BANK_COUNT)
  {
    canopen->status = NO_BANK;
    return canopen->status;
  }
  
  CANopenFilterConfig filter = {0};
  filter.bank      = canopen->bank_count++;
  filter.mode      = COB_FILTERMODE_IDLIST;
  filter.scale     = COB_FILTERSCALE_16BIT;
  filter.id_high   = id1;
  filter.id_low    = id2;
  filter.mask_high = id3;
  filter.mask_low  = id4;
  filter.fifo      = fifo;
  filter.active    = 1;  
  filter.end_bank  = MAX_BANK_COUNT; // TODO Не реализовано по дефолту 14
  can_conf_filter(&filter);
  canopen->status = OK;
  return canopen->status;
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


void CANopen_send_pdo(CANopen_PDO *pdo)
{
  can_send_packet(pdo->id, COB_RTR_DATA, pdo->ide, pdo->dlс, pdo->data);
}

void CANopen_send_sdo(CANopen *canopen, CANopen_SDO *sdo)
{
  can_send_packet(sdo->id, COB_RTR_DATA, canopen->ide, COB_SIZE_DEF, sdo->data);
}

#define MAX_CALLBACKS 10
static CAN_Handler pdo_rxcallbacks[MAX_CALLBACKS] = {0};
static uint8_t callback_count = 0;

CANopen_Status CANopen_register_pdo_rx_callback(CANopen *canopen, uint32_t id, canopen_pdo_rxcallback callback)
{
  if(callback_count >= MAX_CALLBACKS)
    return NO_CALLBACK;

  /* Проверка, зарегистрирована ли PDO */
  for (uint8_t i = 0; i < callback_count; i++)
  {
    if (pdo_rxcallbacks[i].id == id)
    {
      pdo_rxcallbacks[i].callback = callback;
      return OK; // TODO Другой статус
    }
  }

  /* Добавить новый обработчик */
  pdo_rxcallbacks[callback_count].id = id;
  pdo_rxcallbacks[callback_count].callback = callback;
  callback_count++;

  /* Настройка фильтра для приема */
  CANopen_config_filter_list_16b(canopen, id, 0, 0, 0, 0);
  return OK;
}

void canopen_process_rx_message(uint32_t id, uint8_t *data, uint8_t dlc)
{
  for (uint8_t i = 0; i < callback_count; i++)
  {
    if ((pdo_rxcallbacks[i].id == id) && (pdo_rxcallbacks[i].callback != NULL))
    {
      pdo_rxcallbacks->callback(id, data, dlc);
      return;
    }
  }
}


