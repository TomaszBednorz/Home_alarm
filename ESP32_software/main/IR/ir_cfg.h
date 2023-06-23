#ifndef __IR_CFG__
#define __IR_CFG__


/*
 * IR configuration: task priority, task period
 */
#define IR_TASK_PRIORITY                 (11U)
#define IR_TASK_PERIOD                   (5U)   /* ms */

/*
 * IR pins configuration
 */
#define IR_OUT_PIN                       (23U)

/*
 * IR debouncing queue size
 */
#define IR_DEBOUNCING_QUEUE_SIZE         (5U)


#endif