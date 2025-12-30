#ifndef OBJ_H
#define OBJ_H

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

typedef enum {
    OD_TYPE_BOOL = 0x00,
    OD_TYPE_INT8 = 0x01,
    OD_TYPE_INT16 = 0x02,
    OD_TYPE_INT32 = 0x03,
    OD_TYPE_UINT8 = 0x04,
    OD_TYPE_UINT16 = 0x05,
    OD_TYPE_UINT32 = 0x06,
    OD_TYPE_FLOAT32 = 0x07,
    OD_TYPE_STRING = 0x08,
} od_data_t;

typedef enum {
    OD_RO = 0x00,
    OD_WO = 0x01,
    OD_WR = 0x02,
} od_access_t;

typedef union
{
    int8_t i8;
    int16_t i16;
    int32_t i32;
    uint8_t u8;
    uint16_t u16;
    uint32_t u32;
    float f32;
} od_limit_t;

typedef struct
{
    char *name;
    void *pdata;
    uint16_t idx;
    uint8_t sidx;
    od_data_t type;
    od_access_t access;
    od_limit_t min_val;
    od_limit_t max_val;
} co_od_t;

uint32_t co_od_read(uint16_t index, uint8_t sub_index, void *data, uint32_t size);
uint32_t co_od_write(uint16_t index, uint8_t sub_index, void *data, uint32_t size);
uint32_t co_od_size(uint16_t index, uint8_t sub_index);
uint16_t co_od_count(void);

#endif // OBJ_H