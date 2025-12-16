#ifndef SDO_H
#define SDO_H

#include "def.h"
#include "can_open.h"

// typedef enum
// {
//   SDO_STATE_IDLE = 0,
//   SDO_STATE_WAIT_RESPONSE,
//   SDO_STATE_SEGMENTED_IN_PROGRESS,
//   SDO_STATE_COMPLETED,
//   SDO_STATE_ERROR,
//   SDO_STATE_WAIT_RETRY,
//   SDO_STATE_RETRY_SENDING,
//   SDO_STATE_SERVER_PROCESSING,
//   SDO_STATE_SERVER_WAIT_SEGMENT,
// } sdo_state_t;

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
  // === КЛИЕНТ --> СЕРВЕР (запросы) ===
  SDO_CLIENT_INITIATE_DOWNLOAD = 0x20, // 0010 0000 - Начало записи
  SDO_CLIENT_INITIATE_UPLOAD = 0x40,   // 0100 0000 - Начало чтения

  // Expedited записи (данные сразу в первом сообщении)
  SDO_CLIENT_WRITE_1BYTE = 0x2F, // 0010 1111 (expedited + size + n=3)
  SDO_CLIENT_WRITE_2BYTE = 0x2B, // 0010 1011 (expedited + size + n=2)
  SDO_CLIENT_WRITE_3BYTE = 0x27, // 0010 0111 (expedited + size + n=1)
  SDO_CLIENT_WRITE_4BYTE = 0x23, // 0010 0011 (expedited + size + n=0)

  // Сегментированные передачи
  SDO_CLIENT_DOWNLOAD_SEGMENT = 0x00, // 0000 0000 - Сегмент данных для записи
  SDO_CLIENT_UPLOAD_SEGMENT = 0x60,   // 0110 0000 - Запрос следующего сегмента чтения

  // === СЕРВЕР → КЛИЕНТ (ответы) ===
  SDO_SERVER_INITIATE_DOWNLOAD_RESP = 0x60, // 0110 0000 - Подтверждение начала записи
  SDO_SERVER_INITIATE_UPLOAD_RESP = 0x40,   // 0100 0000 - Ответ с данными при чтении
  SDO_SERVER_DOWNLOAD_SEGMENT_RESP = 0x20,  // 0010 0000 - Подтверждение сегмента записи
  SDO_SERVER_UPLOAD_SEGMENT_RESP = 0x00,    // 0000 0000 - Сегмент данных при чтении

  // === ОБЩИЕ ===
  SDO_ABORT_TRANSFER = 0x80, // 1000 0000 - Прерывание передачи

  // === ФЛАГИ (для сборки команд) ===
  SDO_EXPEDITED_BIT = 0x02,      // Бит expedited
  SDO_SIZE_INDICATED_BIT = 0x01, // Бит указания размера
  SDO_TOGGLE_BIT = 0x10,         // Бит toggle для сегментированной передачи

  // Маски размера данных (n)
  SDO_SIZE_N_MASK = 0x0C,  // Маска битов n (3-2)
  SDO_SIZE_N_0BYTE = 0x00, // n=0 (4 байта данных)
  SDO_SIZE_N_1BYTE = 0x04, // n=1 (3 байта данных)
  SDO_SIZE_N_2BYTE = 0x08, // n=2 (2 байта данных)
  SDO_SIZE_N_3BYTE = 0x0C, // n=3 (1 байт данных)
} sdo_cmd_t;

#define canopen_sdo_abort(canopen, msg, index, sub_index, abort_code) \
  canopen_sdo_transmit(canopen, msg, SDO_ABORT_TRANSFER, index, sub_index, abort_code);

#define canopen_sdo_write_32(canopen, msg, index, sub_index, data) \
  canopen_sdo_transmit(canopen, msg, SDO_CLIENT_WRITE_4BYTE, index, sub_index, data)

#define canopen_sdo_write_16(canopen, msg, index, sub_index, data) \
  canopen_sdo_transmit(canopen, msg, SDO_CLIENT_WRITE_2BYTE, index, sub_index, data)

#define canopen_sdo_write_8(canopen, msg, index, sub_index, data) \
  canopen_sdo_transmit(canopen, msg, SDO_CLIENT_WRITE_1BYTE, index, sub_index, data)

#define canopen_sdo_read_32(canopen, msg, index, sub_index) \
  canopen_sdo_transmit(canopen, msg, SDO_CLIENT_INITIATE_UPLOAD, index, sub_index, 0)

#define canopen_sdo_read_16(canopen, msg, index, sub_index) \
  canopen_sdo_transmit(canopen, msg, SDO_CLIENT_INITIATE_UPLOAD, index, sub_index, 0);

#define canopen_sdo_read_8(canopen, msg, index, sub_index) \
  canopen_sdo_transmit(canopen, msg, SDO_CLIENT_INITIATE_UPLOAD, index, sub_index, 0);

canopen_state_t canopen_sdo_config(canopen_t *canopen, canopen_msg_t *msg, uint8_t node_id, canopen_callback callback);
canopen_state_t canopen_sdo_transmit(canopen_t *canopen, canopen_msg_t *msg, uint8_t cmd, uint16_t index, uint8_t sub_index, uint32_t data);
canopen_state_t canopen_sdo_process(canopen_t *canopen, canopen_msg_t *msg);
void canopen_sdo_callback(canopen_t *canopen, canopen_msg_t *msg);

#endif // SDO_H