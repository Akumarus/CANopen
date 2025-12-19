#ifndef SDO_H
#define SDO_H

#include "def.h"
#include "can_open.h"

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

typedef enum
{
  /** Основные команды ----------------------------------------------*/
  SDO_REQ_INITIATE_DOWNLOAD = 0x20, // 0010 0000 - Начало записи
  SDO_REQ_INITIATE_UPLOAD = 0x40,   // 0100 0000 - Начало чтения
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
} sdo_cmd_t;

typedef enum
{
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
} canopen_abort_code_t;

/**  Запросы прервать операцию  ------------------------------------------------*/
#define canopen_sdo_abort(canopen, msg, index, sub_index, abort_code) \
  canopen_sdo_transmit(canopen, msg, SDO_REQ_ABORT, index, sub_index, abort_code)

/**  Запросы на запись в od 8/16/32 бит ----------------------------------------*/
#define canopen_sdo_write_32(canopen, msg, index, sub_index, data) \
  canopen_sdo_transmit(canopen, msg, SDO_REQ_WRITE_4BYTE, index, sub_index, data)
#define canopen_sdo_write_16(canopen, msg, index, sub_index, data) \
  canopen_sdo_transmit(canopen, msg, SDO_REQ_WRITE_2BYTE, index, sub_index, data)
#define canopen_sdo_write_8(canopen, msg, index, sub_index, data) \
  canopen_sdo_transmit(canopen, msg, SDO_REQ_WRITE_1BYTE, index, sub_index, data)

/**  Запросы на чтение из od 8/16/32 бит ----------------------------------------*/
#define canopen_sdo_read_32(canopen, msg, index, sub_index) \
  canopen_sdo_transmit(canopen, msg, SDO_REQ_INITIATE_UPLOAD, index, sub_index, 0)
#define canopen_sdo_read_16(canopen, msg, index, sub_index) \
  canopen_sdo_transmit(canopen, msg, SDO_REQ_INITIATE_UPLOAD, index, sub_index, 0)
#define canopen_sdo_read_8(canopen, msg, index, sub_index) \
  canopen_sdo_transmit(canopen, msg, SDO_REQ_INITIATE_UPLOAD, index, sub_index, 0)

canopen_state_t canopen_sdo_config(canopen_t *canopen, canopen_msg_t *msg, uint8_t node_id, canopen_callback callback);
canopen_state_t canopen_sdo_transmit(canopen_t *canopen, canopen_msg_t *msg, uint8_t cmd, uint16_t index, uint8_t sub_index, uint32_t data);
canopen_state_t canopen_client_process_sdo(canopen_t *canopen, canopen_msg_t *msg);
canopen_state_t canopen_server_process_sdo(canopen_t *canopen, canopen_msg_t *msg);
void canopen_sdo_callback(canopen_t *canopen, canopen_msg_t *msg);

#endif // SDO_H