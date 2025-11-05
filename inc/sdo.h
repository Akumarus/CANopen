#ifndef SDO_H
#define SDO_H

#include "def.h"
#include "can_open.h"

typedef enum {
  SDO_STATE_IDLE = 0,
  SDO_STATE_WAIT_RESPONSE,
  SDO_STATE_SEGMENTED_IN_PROGRESS,
  SDO_STATE_COMPLETED,
  SDO_STATE_ERROR,
  SDO_STATE_WAIT_RETRY,
  SDO_STATE_RETRY_SENDING,
  SDO_STATE_SERVER_PROCESSING,
  SDO_STATE_SERVER_WAIT_SEGMENT,
}sdo_state_t;

typedef enum {
    // === КЛИЕНТ → СЕРВЕР (запросы) ===
    SDO_CLIENT_INITIATE_DOWNLOAD      = 0x20,  // Клиент: начало ЗАПИСИ данных в сервер
    SDO_CLIENT_INITIATE_UPLOAD        = 0x40,  // Клиент: начало ЧТЕНИЯ данных из сервера  
    SDO_CLIENT_DOWNLOAD_SEGMENT       = 0x00,  // Клиент: сегмент данных для записи
    SDO_CLIENT_UPLOAD_SEGMENT         = 0x60,  // Клиент: запрос следующего сегмента чтения
    
    // === СЕРВЕР → КЛИЕНТ (ответы) ===  
    SDO_SERVER_INITIATE_DOWNLOAD_RESP = 0x60,  // Сервер: подтверждение начала записи
    SDO_SERVER_INITIATE_UPLOAD_RESP   = 0x40,  // Сервер: ответ с данными при чтении
    SDO_SERVER_DOWNLOAD_SEGMENT_RESP  = 0x20,  // Сервер: подтверждение сегмента записи
    SDO_SERVER_UPLOAD_SEGMENT_RESP    = 0x00,  // Сервер: сегмент данных при чтении
    
    // === ОБЩИЕ ===
    SDO_ABORT_TRANSFER                = 0x80,  // Прерывание передачи (клиент или сервер)

    // === ФЛАГИ ===
    SDO_EXPEDITED_BIT                 = 0x02,  // Expedited transfer
    SDO_SIZE_INDICATED_BIT            = 0x01,  // Size specified
    SDO_TOGGLE_BIT                    = 0x10,  // Toggle bit для сегментированного transfer
} sdo_cmd_t;

typedef struct {
  sdo_state_t state;
  canopen_message_t msg;
} canopen_sdo_message_t;
#endif // SDO_H
