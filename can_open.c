#include "can_open.h"
#include "port.h"

void CANopen_init(CANopen *canopen, uint32_t ide)
{
  canopen->ide = ide;
  HAL_CAN_Start(&hcan);
  CAN_FilterTypeDef sFilterConfig;
  sFilterConfig.FilterBank = 0;
  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
  sFilterConfig.FilterIdHigh = 0x0000;
  sFilterConfig.FilterIdLow = 0x0000;
  sFilterConfig.FilterMaskIdHigh = 0x0000;
  sFilterConfig.FilterMaskIdLow = 0x0000;
  sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
  sFilterConfig.FilterActivation = ENABLE;
  sFilterConfig.SlaveStartFilterBank = 14;

  if (HAL_CAN_ConfigFilter(&hcan, &sFilterConfig) != HAL_OK)
    Error_Handler();

  // Активация прерываний
  HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
}

void CANopen_config_filter_list(CANopen *canopen, uint32_t id1, uint32_t id2, uint32_t id3, uint32_t id4, uint8_t fifo)
{
  CANopenFilterConfig filter;
  // filter
  // can_conf_filter();
}


void CANopen_send_pdo(CANopen *canopen, CANopen_PDO *pdo)
{
  can_send_packet(pdo->id, COB_RTR_DATA, canopen->ide, pdo->dlс, pdo->data);
}

void CANopen_send_sdo(CANopen *canopen, CANopen_SDO *sdo)
{
  can_send_packet(sdo->id, COB_RTR_DATA, canopen->ide, COB_SIZE_DEF, sdo->data);
}

void CANopen_read_pdo()
{

}

void CANopen_read_sdo()
{
    
}