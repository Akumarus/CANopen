#ifndef FILTER_H
#define FILTER_H

#include "def.h"

#define IDS_PER_BANK 4

typedef enum {
    COB_FILTER_MODE_IDMASK = 0x00000000U,
    COB_FILTER_MODE_IDLIST = 0x00000001U
} co_filter_mode_t;

typedef enum {
    COB_FILTER_SIZE_16BIT = 0x00000000U,
    COB_FILTER_SIZE_32BIT = 0x00000001U,
} co_filte_size_t;

typedef struct {
    uint32_t ids[IDS_PER_BANK];
    uint8_t used;
    uint8_t fifo;
} co_bank_t;

co_res_t co_cnf_filter_list_16b(co_bank_t *bank, uint32_t id, uint8_t fifo);

#endif // FILTER_H