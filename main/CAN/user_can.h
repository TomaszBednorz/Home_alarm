#ifndef __USER_CAN__
#define __USER_CAN__

#include <stdint.h>
#include "user_can_cfg.h"

typedef enum
{
    #define USER_CAN_CFG_FRAME(name, id, period, offset, dlc)   name,
        USER_CAN_CFG_TABLE
    #undef USER_CAN_CFG_FRAME
    UserCanFrameMax
}UserCan_Frame_t;


void UserCan_Init(void);
void UserCan_FillFrame(UserCan_Frame_t frame_id, uint8_t* data);


#endif