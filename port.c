#include "port.h"
#include "can.h"

uint32_t mailbox;



void can_conf_filter(CANopenFilterConfig *filter)
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

void can_send_packet(uint32_t id, uint32_t rtr, uint32_t ide, uint32_t dlс, uint8_t* data)
{
    CAN_TxHeaderTypeDef txHeader = {0};
    txHeader.StdId = id;
    txHeader.RTR = rtr;
    txHeader.IDE = ide;
    txHeader.DLC = dlс;
    txHeader.TransmitGlobalTime = DISABLE; // TODO  Настроить режим
    HAL_CAN_AddTxMessage(&hcan, &txHeader, data, &mailbox);
}