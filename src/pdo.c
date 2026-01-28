#include "pdo.h"
#include "fifo.h"

#define PDO_TX_BASE(pdo_num) (0x180 + (((pdo_num)-1) * 0x100))
#define PDO_RX_BASE(pdo_num) (0x200 + (((pdo_num)-1) * 0x100))

#define PDO_TX_TYPE(pdo_num) ((co_msg_type_t)(TYPE_PDO1_TX + ((pdo_num)-1)))
#define PDO_RX_TYPE(pdo_num) ((co_msg_type_t)(TYPE_PDO1_RX + ((pdo_num)-1)))

co_res_t co_subscribe_pdo(co_obj_t *co, uint32_t id, co_hdl_t callback) {
    co_config_filter(co->banks, id, 0);
    co_config_callback(co, id, callback);
    return CANOPEN_OK;
}

co_res_t co_transmite_pdo(co_obj_t *co, uint32_t id, co_pdo_t *data, uint8_t dlc) {
    co_msg_t msg = { .dlc = dlc, .id = id};
    memcpy(&msg.frame, data, dlc);
    fifo_state_t fifo_state = fifo_push(&co->fifo_tx, &msg);
    return (fifo_state == FIFO_OK) ? CANOPEN_OK : CANOPEN_ERROR;
}
