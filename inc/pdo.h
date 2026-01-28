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

#define RPDO1(node_id) (0x200 + ((node_id) & 0x7F))
#define RPDO2(node_id) (0x300 + ((node_id) & 0x7F))
#define RPDO3(node_id) (0x400 + ((node_id) & 0x7F))
#define RPDO4(node_id) (0x500 + ((node_id) & 0x7F))

#define TPDO1(node_id) (0x180 + ((node_id) & 0x7F))
#define TPDO2(node_id) (0x280 + ((node_id) & 0x7F))
#define TPDO3(node_id) (0x380 + ((node_id) & 0x7F))
#define TPDO4(node_id) (0x480 + ((node_id) & 0x7F))

co_res_t co_subscribe_pdo(co_obj_t *co, uint32_t id, co_hdl_t callback);
co_res_t co_transmite_pdo(co_obj_t *co, uint32_t id, uint8_t *data, uint8_t dlc);

#endif // PDO_H
