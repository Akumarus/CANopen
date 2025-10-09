#ifndef PORT_H
#define PORT_H

#include "stdint.h"

void can_send_packet(uint32_t id, uint32_t rtr, uint32_t ide, uint32_t dlc, uint8_t* data);

#endif // PORT_H