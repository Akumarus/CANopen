#ifndef SDO_H
#define SDO_H

#include "co.h"
#include "def.h"

/*
 * Формат команды SDO (биты 7-0):
 *
 * Бит 7-5: Команда (CCS - Client Command Specifier)
 *   001 = Download segment (0x20-0x27, 0x2B, 0x2F)
 *   010 = Initiate upload (0x40)
 *   011 = Upload segment (0x60)
 *   100 = Abort transfer (0x80)
 *
 * Бит 4: Toggle (для сегментированной передачи)
 *
 * Бит 3-2: n - количество байтов, которых НЕ хватает до 4 байт
 *   n = 0: 4 байта данных (0x00)
 *   n = 1: 3 байта данных (0x04)
 *   n = 2: 2 байта данных (0x08)
 *   n = 3: 1 байт данных  (0x0C)
 *
 * Бит 1: e (expedited) - ускоренная передача
 *   1 = Данные в первом сообщении
 *
 * Бит 0: s (size indicated) - указан размер
 *   1 = Размер указан
 *
 * Для expedited write (ускоренная запись):
 *   CCS = 001 (биты 7-5 = 001)
 *   e = 1 (бит 1 = 1)
 *   s = 1 (бит 0 = 1)
 *   n = (4 - размер_данных) (биты 3-2)
 *
 * Примеры:
 * SDO_CLIENT_WRITE_4BYTE = 0x23
 *   0 0 1 0 0 0 1 1
 *   │ │ │ │ │ │ │ └─ s=1 (size указан)
 *   │ │ │ │ │ │ └─── e=1 (expedited)
 *   │ │ │ │ │ └───── n=0 (4-4=0, все 4 байта)
 *   │ │ │ │ │        00 = 0 байт не хватает
 *   │ │ │ └───────── бит4=0 (toggle, не используется)
 *   │ │ └─────────── CCS=001 (initiate download)
 *
 * SDO_CLIENT_WRITE_1BYTE = 0x2F
 *   0 0 1 0 1 1 1 1
 *   │ │ │ │ │ │ │ └─ s=1
 *   │ │ │ │ │ │ └─── e=1
 *   │ │ │ │ │ └───── n=3 (4-1=3, не хватает 3 байт)
 *   │ │ │ │ │        11 = 3 байта не хватает
 *   │ │ │ └───────── бит4=0
 *   │ │ └─────────── CCS=001
 */

typedef enum {
    /** Основные команды ----------------------------------------------*/
    SDO_REQ_DOWNLOAD = 0x20, // 0010 0000 - Начало записи
    SDO_REQ_UPLOAD = 0x40,   // 0100 0000 - Начало чтения
    SDO_REQ_DOWNLOAD_SEGMENT = 0x00,
    SDO_REQ_UPLOAD_SEGMENT = 0x60,
    SDO_REQ_ABORT = 0x80,
    /** Команды Expedited Write ---------------------------------------*/
    SDO_REQ_WRITE_1BYTE = 0x2F, // 0010 1111 (expedited + size + n=3)
    SDO_REQ_WRITE_2BYTE = 0x2B, // 0010 1011 (expedited + size + n=2)
    SDO_REQ_WRITE_3BYTE = 0x27, // 0010 0111 (expedited + size + n=1)
    SDO_REQ_WRITE_4BYTE = 0x23, // 0010 0011 (expedited + size + n=0)
    /** Ответы сервера ------------------------------------------------*/
    SDO_RESP_DOWNLOAD_OK = 0x60,
    SDO_RESP_UPLOAD_DATA = 0x40,
    /** Флаги ---------------------------------------------------------*/
    SDO_FLAG_SIZE_INDICATED = 0x01, // Бит указания размера
    SDO_FLAG_EXPEDITED = 0x02,      // Бит expedited
    SDO_FLAG_TOGGLE = 0x10,
    SDO_SIZE_N_MASK = 0x0C,
} co_sdo_cmd_t;

typedef enum {
    SDO_ABORT_TOGGLE_BIT = 0x05030000,
    SDO_ABORT_TIMEOUT = 0x05040000,
    SDO_ABORT_INVALID_CS = 0x05040001,
    SDO_ABORT_READONLY = 0x06010002,
    SDO_ABORT_WRITEONLY = 0x06010003,
    SDO_ABORT_OBJ_NOT_EXIST = 0x06020000,
    SDO_ABORT_SUB_NOT_EXIST = 0x06090011,
    SDO_ABORT_INVALID_VALUE = 0x06090030,
    SDO_ABORT_VALUE_TOO_HIGH = 0x06090032,
    SDO_ABORT_VALUE_TOO_LOW = 0x06090036,
    SDO_ABORT_GENERAL = 0x08000000,
} co_abort_code_t;

co_res_t co_subscribe_sdo(co_obj_t *co, uint8_t node_id, co_hdl_t callback);
co_res_t co_subscribe_sdo1(co_obj_t *co, uint8_t node_id, co_hdl_t callback);
co_res_t co_transmite_sdo(co_obj_t *co, uint8_t node_id, co_sdo_t *data, uint8_t dlc);
co_res_t co_server_process_sdo(co_obj_t *co, co_msg_t *msg);
co_res_t co_client_process_sdo(co_obj_t *co, co_msg_t *msg);

/**  Запросы прервать операцию  ------------------------------------------------------------*/
static inline co_res_t co_abort_sdo(co_obj_t *co, uint8_t node_id, uint16_t idx, uint8_t sidx,
                                    uint32_t data) {
    co_sdo_t sdo = {SDO_REQ_ABORT, idx, sidx, data};
    return co_transmite_sdo(co, node_id, &sdo, 8);
}

/**  Запросы на чтение из od 8/16/32 бит ---------------------------------------------------*/
static inline co_res_t co_read_sdo_8bit(co_obj_t *co, uint8_t node_id, uint16_t idx, uint8_t sidx) {
    co_sdo_t sdo = {SDO_REQ_UPLOAD, idx, sidx, 0};
    return co_transmite_sdo(co, node_id, &sdo, 5);
}

static inline co_res_t co_read_sdo_16bit(co_obj_t *co, uint8_t node_id, uint16_t idx,
                                         uint8_t sidx) {
    co_sdo_t sdo = {SDO_REQ_UPLOAD, idx, sidx, 0};
    return co_transmite_sdo(co, node_id, &sdo, 6);
}

static inline co_res_t co_read_sdo_32bit(co_obj_t *co, uint8_t node_id, uint16_t idx,
                                         uint8_t sidx) {
    co_sdo_t sdo = {SDO_REQ_UPLOAD, idx, sidx, 0};
    return co_transmite_sdo(co, node_id, &sdo, 8);
}

/**  Запросы на запись в od 8/16/32 бит ----------------------------------------------------*/
static inline co_res_t co_write_sdo_8bit(co_obj_t *co, uint8_t node_id, uint16_t idx, uint8_t sidx,
                                         uint32_t data) {
    co_sdo_t sdo = {SDO_REQ_WRITE_1BYTE, idx, sidx, data};
    return co_transmite_sdo(co, node_id, &sdo, 5);
}

static inline co_res_t co_write_sdo_16bit(co_obj_t *co, uint8_t node_id, uint16_t idx, uint8_t sidx,
                                          uint32_t data) {
    co_sdo_t sdo = {SDO_REQ_WRITE_2BYTE, idx, sidx, data};
    return co_transmite_sdo(co, node_id, &sdo, 6);
}

static inline co_res_t co_write_sdo_32bit(co_obj_t *co, uint8_t node_id, uint16_t idx, uint8_t sidx,
                                          uint32_t data) {
    co_sdo_t sdo = {SDO_REQ_WRITE_4BYTE, idx, sidx, data};
    return co_transmite_sdo(co, node_id, &sdo, 8);
}
#endif // SDO_H