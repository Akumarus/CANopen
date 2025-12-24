#include "filter.h"
#include "port.h"

static uint8_t co_find_free_bank(co_bank_t *bank, uint16_t id, uint8_t fifo);

co_res_t co_cnf_filter_list_16b(co_bank_t *bank, uint32_t id, uint8_t fifo)
{
    assert(bank != NULL);
    assert((fifo == 0) || (fifo == 1));

    uint8_t num = co_find_free_bank(bank, id, fifo);

    co_filter_t filter = {0};
    filter.bank = num;
    filter.mode = COB_FILTER_MODE_IDLIST;
    filter.scale = COB_FILTER_SIZE_16BIT;
    filter.id_high = bank[num].ids[0];
    filter.id_low = (bank[num].used > 1) ? bank[num].ids[1] : 0;
    filter.mask_high = (bank[num].used > 2) ? bank[num].ids[2] : 0;
    filter.mask_low = (bank[num].used > 3) ? bank[num].ids[3] : 0;
    filter.fifo = fifo;
    filter.active = 1;
    filter.end_bank = MAX_BANK_COUNT;
    port_can_init_filter(&filter);

    return CANOPEN_OK;
}

// // void CANopen_config_filter_mask(CANopen *canopen, uint32_t id1,  uint32_t
// mask, uint8_t fifo)
// // {
// //   CANopenFilterConfig filter;
// //   filter.mode = COB_FILTERMODE_IDMASK;
// //   filter.scale = COB_FILTERSCALE_16BIT;
// //   filter.id_high = id1;
// //   filter.id_low = 0;
// //   filter.mask_high = mask;
// //   filter.mask_low = 0;
// //   filter.fifo = fifo;
// //   filter.active = 1;  // TODO Не реализовано
// //   filter.end_bank = 14; // TODO по дефолту 14
// //   can_conf_filter(&filter);
// // }

static uint8_t co_find_free_bank(co_bank_t *bank, uint16_t id, uint8_t fifo)
{
    uint8_t bank_num = 0xFF;
    for (uint8_t i = 0; i < MAX_BANK_COUNT; i++) {
        if ((bank[i].fifo == fifo && bank[i].used < IDS_PER_BANK) || bank[i].used == 0) {
            bank[i].fifo = fifo;
            bank[i].ids[bank[i].used] = id << 5;
            bank[i].used++;
            bank_num = i;
            break;
        }
    }
    return bank_num;
}
