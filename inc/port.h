#ifndef PORT_H
#define PORT_H

#include "stdint.h"

typedef struct
{
    uint8_t bank;
    uint8_t mode;
    uint8_t scale;
    /* TODO ID -> 32 bits */
    uint16_t id_high;
    uint16_t id_low;
    /* TODO Mask -> 32 bits */
    uint16_t mask_high;
    uint16_t mask_low;
    uint8_t fifo;
    uint8_t active;
    uint8_t end_bank;
} CANopenFilterConfig;


void can_init();
void can_conf_filter(CANopenFilterConfig *filter);
void can_send_packet(uint32_t id, uint32_t rtr, uint32_t ide, uint32_t dlc, uint8_t* data);
void can_enable_IT(void);
void can_disable_IT(void);
uint32_t can_get_free_mailboxes(void);

extern uint32_t mailbox;

#endif // PORT_H