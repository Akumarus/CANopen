#include "pdo.h"
#include "fifo.h"

#define PDO_TX_BASE(pdo_num) (0x180 + (((pdo_num)-1) * 0x100))
#define PDO_RX_BASE(pdo_num) (0x200 + (((pdo_num)-1) * 0x100))

#define PDO_TX_TYPE(pdo_num) ((co_msg_type_t)(TYPE_PDO1_TX + ((pdo_num)-1)))
#define PDO_RX_TYPE(pdo_num) ((co_msg_type_t)(TYPE_PDO1_RX + ((pdo_num)-1)))

co_res_t co_subscribe_pdo(co_obj_t *co, pdo_num_t pdo_num, uint8_t node_id, co_hdl_t callback) {
    assert(co != NULL);
    assert(callback != NULL);
    assert(node_id < 128);
    assert(node_id != 0);
    assert(pdo_num >= PDO1 && pdo_num <= PDO4);

    uint32_t id = (co->role == CANOPEN_SERVER) ? PDO_RX_BASE(pdo_num) : PDO_TX_BASE(pdo_num);
    id += node_id;
    return canopen_config_callback(co, id, 0, callback);
}

co_res_t co_transmite_pdo(co_obj_t *co, pdo_num_t pdo_num, uint8_t node_id, co_pdo_t *data,
                          uint8_t dlc) {
    assert(co != NULL);
    assert(data != NULL);
    assert(node_id < 128);
    assert(node_id != 0);
    assert(dlc > 0 && dlc <= 9);
    assert(pdo_num >= PDO1 && pdo_num <= PDO4);

    co_msg_t msg = {0};
    msg.type = (co->role == CANOPEN_SERVER) ? PDO_TX_TYPE(pdo_num) : PDO_RX_TYPE(pdo_num);
    msg.id = (co->role == CANOPEN_SERVER) ? PDO_TX_BASE(pdo_num) : PDO_RX_BASE(pdo_num);
    msg.id += node_id;
    msg.dlc = dlc;
    memcpy(&msg.frame.pdo, data, msg.dlc);
    fifo_state_t fifo_state = fifo_push(&co->fifo_tx, &msg);
    return (fifo_state == FIFO_OK) ? CANOPEN_OK : CANOPEN_ERROR;
}
