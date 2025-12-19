#ifndef NMT_H
#define NMT_H

#include "def.h"
#include "can_open.h"

typedef enum
{
    NMT_CS_START_REMOTE_NODE = 0x01,
    NMT_CS_STOP_REMOTE_NODE = 0x02,
    NMT_CS_ENTER_PRE_OPERATION = 0x80,
    NMT_CS_RESET_NODE = 0x81,
    NMT_CS_RESET_COMMUNICATION = 0x82,
} canopen_nmt_cmd_t;

#endif // NMT_H