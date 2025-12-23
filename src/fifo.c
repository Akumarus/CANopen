#include "fifo.h"
#include <string.h>

fifo_state_t fifo_init(fifo_t *fifo, fifo_config_t config)
{
    if (fifo == NULL || config.message == NULL || config.size == 0)
        return FIFO_ERROR;

    memset(fifo, 0, sizeof(fifo_t));

    fifo->message = config.message;
    fifo->size = config.size;
    return FIFO_OK;
}

fifo_state_t fifo_push(fifo_t *fifo, co_msg_t *msg)
{
    if (fifo == NULL || msg == NULL)
        return FIFO_ERROR;

    if (fifo->count >= fifo->size) {
        fifo->lost_msg_count++;
        return FIFO_FULL;
    }

    memcpy(&fifo->message[fifo->head], msg, sizeof(co_msg_t));
    fifo->head = (fifo->head + 1) % fifo->size;
    fifo->count++;

    return FIFO_OK;
}

fifo_state_t fifo_pop(fifo_t *fifo, co_msg_t *msg)
{
    if (fifo == NULL || msg == NULL)
        return FIFO_ERROR;

    if (fifo->count == 0) {
        return FIFO_EMPTY;
    }

    memcpy(msg, &fifo->message[fifo->tail], sizeof(co_msg_t));
    fifo->tail = (fifo->tail + 1) % fifo->size;
    fifo->count--;

    return FIFO_OK;
}

uint16_t fifo_is_empty(fifo_t *fifo) { return fifo->count == 0; }
uint16_t fifo_get_lost(fifo_t *fifo) { return fifo->lost_msg_count; }
uint16_t fifo_is_full(fifo_t *fifo) { return fifo->count >= fifo->size; }
