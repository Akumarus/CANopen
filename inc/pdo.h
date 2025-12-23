#ifndef PDO_H
#define PDO_H

#include "can_open.h"
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
    PDO_NUM_1 = 1,
    PDO_NUM_2 = 2,
    PDO_NUM_3 = 3,
    PDO_NUM_4 = 4,
} pdo_num_t;

/* PDO Messages -------------------------------------------------*/
#define co_pdo1_cfg_tx(co, msg, node_id, dlc) co_pdo_cfg_tx(co, PDO_NUM_1, node_id, msg, dlc)
#define co_pdo2_cfg_tx(co, msg, node_id, dlc) co_pdo_cfg_tx(co, PDO_NUM_2, node_id, msg, dlc)
#define co_pdo3_cfg_tx(co, msg, node_id, dlc) co_pdo_cfg_tx(co, PDO_NUM_3, node_id, msg, dlc)
#define co_pdo4_cfg_tx(co, msg, node_id, dlc) co_pdo_cfg_tx(co, PDO_NUM_4, node_id, msg, dlc)

#define co_pdo1_cfg_rx(co, node_id, callback) co_pdo_cfg_rx(co, PDO_NUM_1, node_id, callback)
#define co_pdo2_cfg_rx(co, node_id, callback) co_pdo_cfg_rx(co, PDO_NUM_2, node_id, callback)
#define co_pdo3_cfg_rx(co, node_id, callback) co_pdo_cfg_rx(co, PDO_NUM_3, node_id, callback)
#define co_pdo4_cfg_rx(co, node_id, callback) co_pdo_cfg_rx(co, PDO_NUM_4, node_id, callback)

co_res_t co_pdo_send(co_obj_t *co, co_msg_t *msg, co_pdo_t *data);
co_res_t co_pdo_cfg_rx(co_obj_t *co, pdo_num_t pdo_num, uint8_t node_id, co_hdl_t callback);
co_res_t co_pdo_cfg_tx(co_obj_t *co, pdo_num_t pdo_num, uint8_t node_id, co_msg_t *msg,
                       uint8_t dlc);
#endif // PDO_H
