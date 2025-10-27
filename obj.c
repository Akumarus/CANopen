#include "obj.h"
#include "params.h"

static od_type* object_dictionary_find(uint16_t index, uint8_t sub_index)
{
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

void object_dictionary_read_data(uint16_t index, uint8_t sub_index, void* data, uint32_t* size) {
  
  return 0;
}

