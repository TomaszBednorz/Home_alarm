#ifndef __USER_CAN_CFG__
#define __USER_CAN_CFG__

/*
 * CAN configuration: task priority, task period
 */
#define CAN_TASK_PRIORITY                    (10U)
#define CAN_TASK_PERIOD                      (5U)   /* ms */

/*
 * CAN frame type
 */
#define CAN_FRAME_EXTID                      (0U)   /* 0 = basic ID, 1 = ext ID */

/*
 * Macro converts user time to miliseconds
 */
#define CAN_USER_TIME_TO_MS(period)        (period / CAN_TASK_PERIOD)

/* CAN frames configuration: name, ID, period (ms), offset (ms), dlc */
#define USER_CAN_CFG_TABLE  \
    USER_CAN_CFG_FRAME(UserCanFrame1, 0x05, CAN_USER_TIME_TO_MS(10U), CAN_USER_TIME_TO_MS(0U), 2U)   \
    USER_CAN_CFG_FRAME(UserCanFrame2, 0x10, CAN_USER_TIME_TO_MS(10U), CAN_USER_TIME_TO_MS(5U), 1U)


#endif

