#ifndef DEF_H
#define DEF_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#define COB_SIZE_DEF            8
#define COB_SIZE_PDO COB_SIZE_DEF // communication object
#define COB_SIZE_SDO            4 // communication object
#define MAX_BANK_COUNT          14
#define IDS_PER_BANK            4
#define MAX_CALLBACKS           10
#define MAX_11BIT_ID            0x7FF
#define CAN_FIFO_SIZE           32 

typedef enum
{
  TYPE_NONE = 0,
  TYPE_SDO,
  TYPE_PDO,
} msg_type_t;

typedef enum
{
  CANOPEN_OK = 0,
  CANOPEN_ERROR,
} CANopen_State;

typedef union
{
  struct {
    uint8_t  data[COB_SIZE_PDO]; // 1-8 байт
  } row;
  struct {
    uint8_t  data[COB_SIZE_PDO]; // 1-8 байт
  } pdo;
  struct {
    uint8_t  cmd;       // 1 байт
    uint16_t index;     // 2-3 байты
    uint8_t  sub_index; // 4 байт  
    uint32_t data;      // 5-8 байты
  } sdo;
}cob_frame_t;

typedef struct
{
  uint32_t id;
  uint8_t dlc;
  msg_type_t type;
  cob_frame_t frame;
} canopen_message_t;

#endif // DEF_H
