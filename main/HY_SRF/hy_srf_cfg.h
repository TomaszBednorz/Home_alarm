#ifndef __HY_SRF_CFG__
#define __HY_SRF_CFG__

/*
 * HY_SFR configuration: task priority, task period
 */
#define HY_SFR_TASK_PRIORITY                 (11U)
#define HY_SFR_TASK_PERIOD                   (25U)   /* ms */

/*\
 * HY-SRF05 pins configuration
 */
#define HY_SFR_TRIG_PIN                      (5U)
#define HY_SFR_ECHO_PIN                      (18U)

/*
 * HY-SRF05 averaging queue size
 */
#define IR_AVERAGING_QUEUE_SIZE              (3U)

#endif