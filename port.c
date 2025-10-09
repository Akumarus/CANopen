#include "port.h"
#include "can.h"

uint32_t mailbox;

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