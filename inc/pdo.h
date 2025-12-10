#ifndef PDO_H
#define PDO_H

#include "def.h"
#include "can_open.h"

canopen_state_t canopen_send_pdo(canopen_t *canopen, canopen_msg_t *msg);
canopen_state_t canopen_config_pdo_tx(canopen_t *canopen, canopen_msg_t *msg, uint32_t id, uint32_t dlc);

#endif // PDO_H
