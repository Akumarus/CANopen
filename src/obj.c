#include "obj.h"
#include "params.h"

static co_od_t *co_od_item_find(uint16_t index, uint8_t sub_index);
static bool co_od_item_validate(const co_od_t *obj, void *data);
static uint32_t co_od_item_size(const co_od_t *obj);

uint32_t co_od_read(uint16_t index, uint8_t sub_index, void *data, uint32_t size) {
    assert(data != NULL);
    assert(size != 0);

    co_od_t *obj = co_od_item_find(index, sub_index);
    if (obj == NULL)
        return 0;

    uint32_t obj_size = co_od_item_size(obj);
    uint32_t copy_size = (size < obj_size) ? size : obj_size;

    memcpy(data, obj->pdata, copy_size);
    if (obj->type == OD_TYPE_STRING)
        ((char *)data)[copy_size - 1] = '\0';

    return copy_size;
}

uint32_t co_od_write(uint16_t index, uint8_t sub_index, void *data, uint32_t size) {
    assert(data != NULL);
    assert(size != 0);

    co_od_t *obj = co_od_item_find(index, sub_index);
    if (obj == NULL)
        return 0;

    if (co_od_item_validate(obj, data) == true)
        return 0;

    uint32_t obj_size = co_od_item_size(obj);
    if (size > obj_size)
        return 0;

    memcpy(obj->pdata, data, size);
    if (obj->type == OD_TYPE_STRING)
        ((char *)obj->pdata)[size - 1] = '\0';

    return size;
}

uint32_t co_od_size(uint16_t index, uint8_t sub_index) {
    co_od_t *obj = co_od_item_find(index, sub_index);
    if (obj == NULL)
        return 0;

    uint32_t size = co_od_item_size(obj);
    return size;
}

uint16_t co_od_count(void) { return OBJECT_DICTIONARY_SIZE; }

/* Поиск объекта в словаре */
static co_od_t *co_od_item_find(uint16_t idx, uint8_t sidx) {
    // TODO Нужно сделать быстрый перебор
    for (uint16_t i = 0; i < OBJECT_DICTIONARY_SIZE; i++) {
        if (object_dictionary[i].idx == idx && object_dictionary[i].sidx == sidx) {
            return &object_dictionary[i];
        }
    }
    return NULL;
}

/* Проверка на выход за пределы максимального и минимального значения */
static bool co_od_item_validate(const co_od_t *obj, void *data) {
    if (obj == NULL || data == NULL)
        return true;

    if (obj->type == OD_TYPE_STRING || obj->type == OD_TYPE_BOOL)
        return false;

    switch (obj->type) {
    case OD_TYPE_INT8: {
        int8_t value = *(int8_t *)data;
        int8_t min_val = obj->min_val.i8;
        int8_t max_val = obj->max_val.i8;
        return (value >= min_val && value <= max_val);
    }
    case OD_TYPE_UINT8: {
        uint8_t value = *(uint8_t *)data;
        uint8_t min_val = obj->min_val.u8;
        uint8_t max_val = obj->max_val.u8;
        return (value >= min_val && value <= max_val);
    }
    case OD_TYPE_INT16: {
        int16_t value = *(uint16_t *)data;
        int16_t min_val = obj->min_val.i16;
        int16_t max_val = obj->max_val.i16;
        return (value >= min_val && value <= max_val);
    }
    case OD_TYPE_UINT16: {
        uint16_t value = *(uint16_t *)data;
        uint16_t min_val = obj->min_val.u16;
        uint16_t max_val = obj->max_val.u16;
        return (value >= min_val && value <= max_val);
    }
    case OD_TYPE_INT32: {
        int32_t value = *(int32_t *)data;
        int32_t min_val = obj->min_val.i32;
        int32_t max_val = obj->max_val.i32;
        return (value >= min_val && value <= max_val);
    }
    case OD_TYPE_UINT32: {
        uint32_t value = *(uint32_t *)data;
        uint32_t min_val = obj->min_val.u32;
        uint32_t max_val = obj->max_val.u32;
        return (value >= min_val && value <= max_val);
    }
    case OD_TYPE_FLOAT32: {
        float value = *(float *)data;
        float min_val = obj->min_val.f32;
        float max_val = obj->max_val.f32;
        return (value >= min_val && value <= max_val);
    }
    default:
        break;
    }

    return false;
}

// TODO Можно сдлеть компактней
static uint32_t co_od_item_size(const co_od_t *obj) {
    uint32_t size;
    switch (obj->type) {
    case OD_TYPE_INT8:
        size = sizeof(int8_t);
        break;
    case OD_TYPE_UINT8:
        size = sizeof(uint8_t);
        break;
    case OD_TYPE_INT16:
        size = sizeof(int16_t);
        break;
    case OD_TYPE_UINT16:
        size = sizeof(uint16_t);
        break;
    case OD_TYPE_INT32:
        size = sizeof(int32_t);
        break;
    case OD_TYPE_UINT32:
        size = sizeof(uint32_t);
        break;
    case OD_TYPE_FLOAT32:
        size = sizeof(float);
        break;
    case OD_TYPE_BOOL:
        size = sizeof(bool);
        break;
    case OD_TYPE_STRING:
        size = strlen(obj->pdata);
        break;
    }
    return size;
}
