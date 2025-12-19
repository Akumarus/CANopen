#include "nmt.h"
#include "fifo.h"

canopen_state_t canopen_config_nmt(canopen_t *canopen, uint8_t node_id)
{
    assert(canopen != NULL);
    assert(node_id < 128);

    // if (node_id == 0)

    return CANOPEN_OK;
}