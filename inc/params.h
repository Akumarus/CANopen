#ifndef PARAMS_H
#define PARAMS_H

#include "obj.h"

typedef struct
{
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

#define OBJECT_DICTIONARY_SIZE 4

extern device_data_t device_data;
extern co_od_t object_dictionary[OBJECT_DICTIONARY_SIZE];

#endif // PARAMS_H