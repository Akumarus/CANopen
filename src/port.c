#include "port.h"
#include "can.h"

uint32_t mailbox = 0;
uint32_t it_mask = CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_RX_FIFO1_MSG_PENDING;

void port_can_init(void)
{
    MX_CAN_Init();
    /*
      TODO Почему-то на другой плате не сразу меняется регистр выхода
      из режима инициализации.

      HAL_CAN_Start(&hcan);
    */
    CLEAR_BIT(hcan.Instance->MCR, CAN_MCR_INRQ);
    HAL_CAN_ActivateNotification(&hcan, it_mask);
}

void port_can_init_filter(co_filter_t *filter)
{
    CAN_FilterTypeDef can_filter = {0};
    can_filter.FilterBank = filter->bank;
    can_filter.FilterMode = filter->mode;
    can_filter.FilterScale = filter->scale;
    can_filter.FilterIdHigh = filter->id_high;
    can_filter.FilterIdLow = filter->id_low;
    can_filter.FilterMaskIdHigh = filter->mask_high;
    can_filter.FilterMaskIdLow = filter->mask_low;
    can_filter.FilterFIFOAssignment = filter->fifo;
    can_filter.FilterActivation = filter->active;
    can_filter.SlaveStartFilterBank = filter->end_bank;
    HAL_CAN_ConfigFilter(&hcan, &can_filter);
}

void port_can_send(uint32_t id, uint32_t rtr, uint32_t ide, uint32_t dlс, uint8_t *data)
{
    CAN_TxHeaderTypeDef txHeader = {0};
    txHeader.StdId = id;
    txHeader.RTR = rtr;
    txHeader.IDE = ide;
    txHeader.DLC = dlс;
    txHeader.TransmitGlobalTime = DISABLE; // TODO  Настроить режим
    HAL_CAN_AddTxMessage(&hcan, &txHeader, data, &mailbox);
}

uint32_t port_get_free_mailboxes(void) { return HAL_CAN_GetTxMailboxesFreeLevel(&hcan); }

uint32_t port_get_timestamp(void) { return HAL_GetTick(); }

bool port_can_receive_message(uint32_t *id, uint8_t *data, uint8_t *dlc, uint32_t fifo)
{
    if (id == NULL || data == NULL || dlc == NULL)
        return false;

    CAN_RxHeaderTypeDef rx_header;
    if (HAL_CAN_GetRxMessage(&hcan, fifo, &rx_header, data) != HAL_OK)
        return false;

    *id = rx_header.StdId;
    *dlc = rx_header.DLC;

    return true;
}