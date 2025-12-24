#ifndef PORT_H
#define PORT_H

#include <stdbool.h>
#include <stdint.h>

#pragma pack(push, 1)
typedef struct {
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
} co_filter_t;
#pragma pack(pop)

void port_can_init(void);
void port_can_init_filter(co_filter_t *filter);
void port_can_send(uint32_t id, uint32_t rtr, uint32_t ide, uint32_t dlс, uint8_t *data);
bool port_can_receive_message(uint32_t *id, uint8_t *data, uint8_t *dlc, uint32_t fifo);
uint32_t port_get_timestamp(void);
uint32_t port_get_free_mailboxes(void);
extern uint32_t mailbox;

#endif // PORT_H