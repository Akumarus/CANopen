#ifndef PDO_H
#define PDO_H

#include "co.h"
#include "def.h"

/* MASTER: ID для общения с Slave (NodeID = N)    */
/* ┌────────────────┬─────────────┬─────────────┐ */
/* │ Направление    │ Канал PDO   │ CAN ID      │ */
/* ├────────────────┼─────────────┼─────────────┤ */
/* │ Master ← Slave │ PDO1 TX     │ 0x180 + N   │ */
/* │ Master → Slave │ PDO1 RX     │ 0x200 + N   │ */
/* │ Master ← Slave │ PDO2 TX     │ 0x280 + N   │ */
/* │ Master → Slave │ PDO2 RX     │ 0x300 + N   │ */
/* │ Master ← Slave │ PDO3 TX     │ 0x380 + N   │ */
/* │ Master → Slave │ PDO3 RX     │ 0x400 + N   │ */
/* │ Master ← Slave │ PDO4 TX     │ 0x480 + N   │ */
/* │ Master → Slave │ PDO4 RX     │ 0x500 + N   │ */
/* └────────────────┴─────────────┴─────────────┘ */

/* ┌────────────────┬─────────────┬─────────────┐ */
/* │ Направление    │ Канал PDO   │ CAN ID      │ */
/* ├────────────────┼─────────────┼─────────────┤ */
/* │ Slave → Master │ PDO1 TX     │ 0x180 + M   │ */
/* │ Slave ← Master │ PDO1 RX     │ 0x200 + M   │ */
/* │ Slave → Master │ PDO2 TX     │ 0x280 + M   │ */
/* │ Slave ← Master │ PDO2 RX     │ 0x300 + M   │ */
/* │ Slave → Master │ PDO3 TX     │ 0x380 + M   │ */
/* │ Slave ← Master │ PDO3 RX     │ 0x400 + M   │ */
/* │ Slave → Master │ PDO4 TX     │ 0x480 + M   │ */
/* │ Slave ← Master │ PDO4 RX     │ 0x500 + M   │ */
/* └────────────────┴─────────────┴─────────────┘ */

typedef enum {
    PDO1 = 1,
    PDO2 = 2,
    PDO3 = 3,
    PDO4 = 4,
} pdo_num_t;

co_res_t co_rpdo_subscribe(co_obj_t *co, pdo_num_t pdo_num, uint8_t node_id, co_hdl_t callback);
co_res_t co_tpdo_publish(co_obj_t *co, pdo_num_t pdo_num, uint8_t node_id, co_pdo_t *data,
                         uint8_t dlc);

co_res_t co_pdo_send(co_obj_t *co, co_msg_t *msg, co_pdo_t *data);
co_res_t co_config_pdo(co_obj_t *co, pdo_num_t pdo_num, uint8_t node_id, co_hdl_t callback);
co_res_t co_pdo_cfg_tx(co_obj_t *co, pdo_num_t pdo_num, uint8_t node_id, co_msg_t *msg,
                       uint8_t dlc);

/* PDO Messages -------------------------------------------------*/
inline co_res_t co_config_pdo1(co_obj_t *co, uint8_t node_id, co_hdl_t callback) {
    return co_config_pdo(co, PDO1, node_id, callback);
}

inline co_res_t co_config_pdo2(co_obj_t *co, uint8_t node_id, co_hdl_t callback) {
    return co_config_pdo(co, PDO2, node_id, callback);
}

inline co_res_t co_config_pdo3(co_obj_t *co, uint8_t node_id, co_hdl_t callback) {
    return co_config_pdo(co, PDO3, node_id, callback);
}

inline co_res_t co_config_pdo4(co_obj_t *co, uint8_t node_id, co_hdl_t callback) {
    return co_config_pdo(co, PDO4, node_id, callback);
}

#define co_pdo1_cfg_tx(co, msg, node_id, dlc) co_pdo_cfg_tx(co, PDO1, node_id, msg, dlc)
#define co_pdo2_cfg_tx(co, msg, node_id, dlc) co_pdo_cfg_tx(co, PDO2, node_id, msg, dlc)
#define co_pdo3_cfg_tx(co, msg, node_id, dlc) co_pdo_cfg_tx(co, PDO3, node_id, msg, dlc)
#define co_pdo4_cfg_tx(co, msg, node_id, dlc) co_pdo_cfg_tx(co, PDO4, node_id, msg, dlc)

#endif // PDO_H
