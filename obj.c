#include "obj.h"
#include "params.h"

static od_type* object_dictionary_find(uint16_t index, uint8_t sub_index);

uint32_t object_dictionary_read(uint16_t index, uint8_t sub_index, void* data, uint32_t size) {
  
  if (data == NULL || size == 0)
    return 0;

  /* Находим объект с указанным индексом и саб-индексом */
  od_type* obj = object_dictionary_find(index, sub_index);
  if (obj == NULL)
    return 0;

  /* TODO Валидация */

  /* Размер копировани */
  uint32_t copy_size = (size < obj->data_size) ? size : obj->data_size;
  
  /* Копируем значение и ставим терминальный ноль, если строка */
  memcpy(data, obj, copy_size);
  if (obj->data_type == OD_TYPE_STRING)
     ((char*)data)[copy_size - 1] = '\0';
  
  return copy_size;
}

uint32_t object_dictionary_write(uint16_t index, uint8_t sub_index, void* data, uint32_t size)
{
  if (data == NULL || size == 0)
    return 0;

  od_type* obj = object_dictionary_find(index, sub_index);
  if (obj == NULL)
    return 0;

  /* TODO Валидация */

  /**/
  if (size > obj->data_size)
    return 0;

  /* Копируем значение и ставим терминальный ноль, если строка */
  memcpy(obj->data_ptr, data, size);
  if (obj->data_type == OD_TYPE_STRING)
     ((char*)obj->data_ptr)[size - 1] = '\0';

  return size;
}

uint32_t object_dictionary_get_size(uint16_t index, uint8_t sub_index)
{
  od_type* obj = object_dictionary_find(index, sub_index);
  if (obj == NULL)
    return 0;

  return obj->data_size;
}

uint16_t object_dictionary_get_count(void) {
  return OBJECT_DICTIONARY_SIZE;
}

static od_type* object_dictionary_find(uint16_t index, uint8_t sub_index)
{
  // TODO Нужно сделать быстрый перебор
  for (uint16_t i = 0; i < OBJECT_DICTIONARY_SIZE i++)
  {
    if (object_dictionary[i].index == index &&
        object_dictionary[i].sub_index == sub_index) 
    {
      return &object_dictionary[i];
    }
  }
  return NULL;
}

bool object_dictionary_validate_limits(const od_type* obj, void* data)
{
  if (obj == NULL || data == NULL || !obj->has_limits)
    return true;

  switch (obj->data_type)
  {
    case OD_TYPE_INT8: 
      int8_t value = *(int8_t*)data;
      // return (value >= obj)

    default:
      break;
  }

  return false;
}




