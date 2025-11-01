#ifndef PARAMS_H
#define PARAMS_H

#include "obj.h"

typedef struct {
  uint32_t error_register;
  uint8_t node_id;
  uint32_t heartbeat_time;
  char device_name[32];
  uint32_t serial_number;
  uint16_t product_code;
  uint32_t baudrate;
  uint8_t operating_mode;
  int32_t target_position;
  int32_t actual_position;
  int16_t target_velocity;
  int16_t actual_velocity;
  uint16_t control_word;
  uint16_t status_word;
  int8_t temperature;
  uint32_t error_history[4];
} device_data_t;

device_data_t device_data;

od_type object_dictionary[3] = {
/* -------------------------------------------------------------------------------------------------------------------------- */
/* | Parameter name   | Index | Sub index |   Data type     | Flag  |       Data pointer           | Min |        Max        |*/
/* -------------------------------------------------------------------------------------------------------------------------- */
  {"Error register  ", 0x1000,    0x00,     OD_TYPE_UINT32,   OD_RO, &device_data.error_register,    {0}, {.u32 = 0xFFFFFFFF}},
  {"Error history[0]", 0x1001,    0x00,     OD_TYPE_INT8,     OD_RO, &device_data.error_history[0],  {0}, {.i8 = 127}        },
  {"Device Name     ", 0x1008,    0x00,     OD_TYPE_STRING,   OD_RO, &device_data.device_name,       {0}, {0}                }
};

#define OBJECT_DICTIONARY_SIZE (sizeof(object_dictionary) / sizeof(od_type))

#endif // PARAMS_H