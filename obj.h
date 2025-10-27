#ifndef OBJ_H
#define OBJ_H

#include <stdint.h>
#include <stddef.h>
// #include <string.h>

typedef enum {
  OD_TYPE_BOOL    = 0x00,
  OD_TYPE_INT8    = 0x01,
  OD_TYPE_INT16   = 0x02,
  OD_TYPE_INT32   = 0x03,
  OD_TYPE_UINT8   = 0x04,
  OD_TYPE_UINT16  = 0x05,
  OD_TYPE_UINT32  = 0x06,
  OD_TYPE_FLOAT32 = 0x07,
  OD_TYPE_STRING  = 0x08,
} od_data_type_t;

typedef enum {
  OD_RO = 0x00,
  OD_WO = 0x01,
  OD_WR = 0x02,
} od_access_type_t;

typedef struct {
  uint16_t index;
  uint8_t sub_index;
  od_data_type_t data_type;
  od_access_type_t access;
  void* data_ptr;
  uint32_t data_size;
  const char* name;
} od_type;

#endif // OBJ_H