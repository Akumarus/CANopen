#ifndef OBJ_H
#define OBJ_H

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

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

typedef union {
  int8_t   i8;
  int16_t  i16;
  int32_t  i32;
  uint8_t  u8;
  uint16_t u16;
  uint32_t u32;
  float    f32;
} od_limit_type_t;

typedef struct {
  const char* name;
  uint16_t index;
  uint8_t sub_index;
  od_data_type_t data_type;
  od_access_type_t access;
  void* data_ptr;
  od_limit_type_t min_value;
  od_limit_type_t max_value;
} od_type;

uint32_t object_dictionary_read(uint16_t index, uint8_t sub_index, void* data, uint32_t size);
uint32_t object_dictionary_write(uint16_t index, uint8_t sub_index, void* data, uint32_t size);
uint32_t object_dictionary_get_size(uint16_t index, uint8_t sub_index);
uint16_t object_dictionary_get_count(void);

#endif // OBJ_H