#ifndef CAN_OPEN_H
#define CAN_OPEN_H

#include "def.h"
#include "fifo.h"
#include "filter.h"

#define NODES_COUNT 5

typedef void (*co_hdl_t)(co_msg_t *msg);

typedef enum {
    COB_RX_FIFO0 = 0x00000000U, /*!< CAN receive FIFO 0 */
    COB_RX_FIFO1 = 0x00000001U  /*!< CAN receive FIFO 1 */
} RxFifoType;

typedef enum {
    COB_ID_STD = 0x00000000U, /*!< Standard Id */
    COB_ID_EXT = 0x00000004U  /*!< Extended Id */
} Remote_Transmission_Type;

typedef enum {
    COB_RTR_DATA = 0x00000000U,  /*!< Data frame   */
    COB_RTR_REMOTE = 0x00000002U /*!< Remote frame */
} Identifier_Type;

typedef union {
    uint16_t all;
    struct {
        uint16_t inited : 1;
        uint16_t filter_banks_full : 1;
        uint16_t callbacks_full : 1;
        uint16_t invalid_id : 1;
        uint16_t invalid_dlc : 1;
        uint16_t invalid_fifo : 1;
        uint16_t fifo_tx_full : 1;
        uint16_t reserv : 9;
    } bit;
} co_status_t;

typedef struct {
    uint32_t id;
    co_hdl_t callback;
} canopen_handler_t;

typedef enum {
    CANOPEN_SERVER = 0,
    CANOPEN_CLIENT,
} co_role_t;

typedef struct {
    uint8_t bank_count;
    uint8_t callbacks_count;
    uint32_t tx_mailbox;
    co_status_t status;
    uint32_t sdo_tx_counter;
    uint32_t sdo_rx_counter;
    uint32_t sdo_tx_lost_counter;
    uint32_t sdo_timeout_counter;

    uint32_t fifo_rx_overflow_counter;
    uint32_t fifo_rx_complete_counter;

    uint32_t tx_sended_pdo_count;
    uint32_t tx_pdo_count;
    uint32_t tx_pdo_lost_count;
    uint32_t tx_fifo_full_errors;
    uint32_t tx_busy_mailbox_count;
} co_info_t;

typedef struct {
    uint8_t node_id;
    uint32_t timestamp;
    uint32_t ide;
    fifo_t fifo_tx;
    fifo_t fifo_rx;
    co_role_t role;
    co_info_t info;
    uint32_t node_count;
    co_node_t node[NODES_COUNT];
    co_msg_t buffer_tx[CAN_FIFO_SIZE];
    co_msg_t buffer_rx[CAN_FIFO_SIZE];
    co_bank_t banks[MAX_BANK_COUNT];
    canopen_handler_t callbacks[MAX_CALLBACKS];
    co_nmt_state_t nmt_state;
    uint32_t heartbeat_interval_ms;
    uint32_t last_heartbeat_time;
    bool heartbeat_enable;
} co_obj_t;

co_res_t co_init(co_obj_t *co, co_role_t role, uint8_t node_id, uint32_t ide);
co_res_t co_config_node_id(co_obj_t *co, uint8_t node_id);
co_res_t co_process_msg_tx(co_obj_t *co);
co_res_t co_process_msg_rx(co_obj_t *co);
co_res_t co_process_timeout(co_obj_t *co);

co_res_t canopen_config_callback(co_obj_t *canopen, uint32_t id, uint8_t fifo, co_hdl_t callback);
co_res_t canopen_get_msg_from_handler(co_msg_t *msg, uint32_t fifo);
co_res_t canopen_send_msg_to_fifo_rx(co_obj_t *canopen, co_msg_t *msg);
// void canopen_config_filter_mask(CANopen *canopen, uint32_t id1, uint32_t
// mask, uint8_t fifo); //
// TODO
co_node_t *get_node_index(co_obj_t *canopen, uint8_t node_id);
uint8_t canopen_get_node_id(co_msg_t *msg);

#endif // CAN_OPEN_H
