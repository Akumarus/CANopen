#include "pdo.h"
#include "fifo.h"

#define PDO_TX_BASE(pdo_num) (0x180 + (((pdo_num)-1) * 0x100))
#define PDO_RX_BASE(pdo_num) (0x200 + (((pdo_num)-1) * 0x100))

#define PDO_TX_TYPE(pdo_num) ((co_msg_type_t)(TYPE_PDO1_TX + ((pdo_num)-1)))
#define PDO_RX_TYPE(pdo_num) ((co_msg_type_t)(TYPE_PDO1_RX + ((pdo_num)-1)))

co_res_t co_pdo_send(co_obj_t *co, co_msg_t *msg, co_pdo_t *data)
{
    assert(co != NULL);
    assert(data != NULL);
    assert(msg != NULL);
    assert(msg->dlc <= sizeof(co_pdo_t));
    assert(msg->dlc <= COB_SIZE_PDO);

    co->info.tx_pdo_count++;
    memcpy(&msg->frame.pdo, data, msg->dlc);
    fifo_state_t fifo_state = fifo_push(&co->fifo_tx, msg);
    return (fifo_state == FIFO_OK) ? CANOPEN_OK : CANOPEN_ERROR;
}

co_res_t co_pdo_cfg_tx(co_obj_t *co, pdo_num_t pdo_num, uint8_t node_id, co_msg_t *msg, uint8_t dlc)
{
    assert(co != NULL);
    assert(msg != NULL);
    assert(node_id < 128);
    assert(node_id != 0);
    assert(pdo_num >= PDO_NUM_1 && pdo_num <= PDO_NUM_4);

    if (dlc > COB_SIZE_PDO)
        dlc = COB_SIZE_PDO;

    msg->type = (co->role == CANOPEN_SERVER) ? PDO_TX_TYPE(pdo_num) : PDO_RX_TYPE(pdo_num);
    msg->id = (co->role == CANOPEN_SERVER) ? PDO_TX_BASE(pdo_num) : PDO_RX_BASE(pdo_num);
    msg->id += node_id;
    msg->dlc = dlc;
    memset(&msg->frame.pdo, 0, COB_SIZE_PDO);

    return CANOPEN_OK;
}

co_res_t co_pdo_cfg_rx(co_obj_t *co, pdo_num_t pdo_num, uint8_t node_id, co_hdl_t callback)
{
    assert(co != NULL);
    assert(callback != NULL);
    assert(node_id < 128);
    assert(node_id != 0);

    uint32_t id = (co->role == CANOPEN_SERVER) ? PDO_RX_BASE(pdo_num) : PDO_TX_BASE(pdo_num);
    id += node_id;
    return canopen_config_callback(co, id, 0, callback);
}

co_res_t co_pdo_process_timeout(co_obj_t *co, co_msg_t *msg) { return CANOPEN_OK; }
