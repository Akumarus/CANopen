#include "obj.h"
#include "params.h"

/* Объявление статических функцийы */
static od_type* object_dictionary_find(uint16_t index, uint8_t sub_index);
static bool object_dictionary_validate_limits(const od_type* obj, void* data);
static uint32_t object_dictionary_size(const od_type* obj);

/* Чтение объекта из словаря */
uint32_t object_dictionary_read(uint16_t index, uint8_t sub_index, void* data, uint32_t size) {
  
  // TODO Проверка на sub_index
  if (data == NULL || size == 0)
    return 0;

  od_type* obj = object_dictionary_find(index, sub_index);
  if (obj == NULL)
    return 0;

  uint32_t obj_size = object_dictionary_size(obj);
  uint32_t copy_size = (size < obj_size) ? size : obj_size;

  memcpy(data, obj->data_ptr, copy_size);
  if (obj->data_type == OD_TYPE_STRING)
     ((char*)data)[copy_size - 1] = '\0';
  
  return copy_size;
}

/* Запись значения в объект словаря */
uint32_t object_dictionary_write(uint16_t index, uint8_t sub_index, void* data, uint32_t size)
{
  if (data == NULL || size == 0)
    return 0;

  od_type* obj = object_dictionary_find(index, sub_index);
  if (obj == NULL)
    return 0;

  if(object_dictionary_validate_limits(obj, data) == true)
    return 0;

  uint32_t obj_size = object_dictionary_size(obj);
  if (size > obj_size)
    return 0;

  memcpy(obj->data_ptr, data, size);
  if (obj->data_type == OD_TYPE_STRING)
     ((char*)obj->data_ptr)[size - 1] = '\0';

  return size;
}

/* получить размер объекта в словаре */
uint32_t object_dictionary_get_size(uint16_t index, uint8_t sub_index)
{
  od_type* obj = object_dictionary_find(index, sub_index);
  if (obj == NULL)
    return 0;

  uint32_t size = object_dictionary_size(obj);
  return size;
}

/* КОличество объектов в словаре */
uint16_t object_dictionary_get_count(void) {
  return OBJECT_DICTIONARY_SIZE;
}

/* Поиск объекта в словаре */
static od_type* object_dictionary_find(uint16_t index, uint8_t sub_index)
{
  // TODO Нужно сделать быстрый перебор
  for (uint16_t i = 0; i < OBJECT_DICTIONARY_SIZE; i++)
  {
    if (object_dictionary[i].index == index &&
        object_dictionary[i].sub_index == sub_index) 
    {
      return &object_dictionary[i];
    }
  }
  return NULL;
}

/* Проверка на выход за пределы максимального и минимального значения */
static bool object_dictionary_validate_limits(const od_type* obj, void* data)
{
  if (obj == NULL || data == NULL)
    return true;

  if (obj->data_type == OD_TYPE_STRING || obj->data_type == OD_TYPE_BOOL)
    return false;

  switch (obj->data_type) {
    case OD_TYPE_INT8: {
      int8_t value   = *(int8_t*)data;
      int8_t min_val = obj->min_value.i8;
      int8_t max_val = obj->max_value.i8;
      return (value >= min_val && value <= max_val);
    }
    case OD_TYPE_UINT8: {
      uint8_t value   = *(uint8_t*)data;
      uint8_t min_val = obj->min_value.u8;
      uint8_t max_val = obj->max_value.u8; 
      return (value >= min_val && value <= max_val);
    }
    case OD_TYPE_INT16: {
      int16_t value   = *(uint16_t*)data;
      int16_t min_val = obj->min_value.i16;
      int16_t max_val = obj->max_value.i16;
      return (value >= min_val && value <= max_val);
    }
    case OD_TYPE_UINT16: {
      uint16_t value   = *(uint16_t*)data;
      uint16_t min_val = obj->min_value.u16;
      uint16_t max_val = obj->max_value.u16;
      return (value >= min_val && value <= max_val);
    }
    case OD_TYPE_INT32: {
      int32_t value   = *(int32_t*)data;
      int32_t min_val = obj->min_value.i32;
      int32_t max_val = obj->max_value.i32;
      return (value >= min_val && value <= max_val);
    }
    case OD_TYPE_UINT32: {
      uint32_t value   = *(uint32_t*)data;
      uint32_t min_val = obj->min_value.u32;
      uint32_t max_val = obj->max_value.u32;
      return (value >= min_val && value <= max_val);
    }
    case OD_TYPE_FLOAT32: {
      float value   = *(float*)data;
      float min_val = obj->min_value.f32;
      float max_val = obj->max_value.f32;
      return (value >= min_val && value <= max_val);
    }
    default: break;
  }

  return false;
}

static uint32_t object_dictionary_size(const od_type* obj)
{
  uint32_t size;
  switch (obj->data_type) {
    case OD_TYPE_INT8:    size = sizeof(int8_t);
    case OD_TYPE_UINT8:   size = sizeof(uint8_t);
    case OD_TYPE_INT16:   size = sizeof(int16_t);
    case OD_TYPE_UINT16:  size = sizeof(uint16_t);
    case OD_TYPE_INT32:   size = sizeof(int32_t);
    case OD_TYPE_UINT32:  size = sizeof(uint32_t);
    case OD_TYPE_FLOAT32: size = sizeof(float);
    case OD_TYPE_BOOL:    size = sizeof(bool);
    case OD_TYPE_STRING:  size = strlen(obj->data_ptr);
  }
  return size;
}




