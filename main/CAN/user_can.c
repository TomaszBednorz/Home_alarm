#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/twai.h"

#include "user_can.h"

/* Error handler for the development phase */
#define CAN_ERROR_HANDLER()     do {} while (1)

#define CAN_MAX_DLC     (8U)

/*
 * Configuration structure
 */
typedef struct
{
    UserCan_Frame_t frame_name;
    twai_message_t message;
    uint32_t period;
    uint32_t counter;
}UserCan_Config_t;


UserCan_Config_t Can_UserConfig[UserCanFrameMax] = 
{
    #define USER_CAN_CFG_FRAME(name, id, time, offset, length) { \
        .frame_name=name, \
        .message={.identifier=id, .data_length_code=length, .data={0}}, \
        .period=time, \
        .counter=offset},
        USER_CAN_CFG_TABLE
    #undef USER_CAN_CFG_FRAME
};

/* 
 * Local function protitypes
 */
static void Can_Task(void *arg);
static bool Can_ProcessFrame(UserCan_Frame_t frame_id);

/*
 * Local objects
 */
TaskHandle_t Can_TaskHandle = NULL;

/*
 * CAN initialization function
 */
void UserCan_Init(void)
{
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_21, GPIO_NUM_22, TWAI_MODE_NO_ACK);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_250KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    //Install CAN driver
    if (twai_driver_install(&g_config, &t_config, &f_config) != ESP_OK) {
        CAN_ERROR_HANDLER();
    }

    //Start CAN driver
    if (twai_start() != ESP_OK) {
        CAN_ERROR_HANDLER();
    }

    // Create CAN task
    xTaskCreate(Can_Task, "can_task", 4096, NULL, CAN_TASK_PRIORITY, &Can_TaskHandle);
}

/*
 * Fill CAN frame with data
 */
void UserCan_FillFrame(UserCan_Frame_t frame_id, uint8_t* data)
{
    uint8_t dlc = Can_UserConfig[frame_id].message.data_length_code;

    for(uint8_t i = 0U; i < dlc; i++)
    {
        Can_UserConfig[frame_id].message.data[i] = data[i];
    }
}

/*
 * CAN task
 */
static void Can_Task(void *arg)
{
    TickType_t xLastWakeTime;
    bool send_frame;

    xLastWakeTime = xTaskGetTickCount();

    while(1)
    {

        for(uint8_t i = 0; i < (uint8_t)UserCanFrameMax; i++)
        {
            send_frame = Can_ProcessFrame((UserCan_Frame_t)i);

            if(false != send_frame)
            {
                if (twai_transmit(&Can_UserConfig[i].message, 0U) != ESP_OK) {
                    CAN_ERROR_HANDLER();
                }
            }
        }

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(CAN_TASK_PERIOD));
    }
}

/*
 * Process CAN frame.
 * Ret: 0 = don't send frame, 1 = send frame (checks period counter)
 */
static bool Can_ProcessFrame(UserCan_Frame_t frame_id)
{
    uint32_t period = Can_UserConfig[frame_id].period;
    uint32_t counter = ++Can_UserConfig[frame_id].counter;
    bool ret;

    if(counter >= period)
    {
        Can_UserConfig[frame_id].counter = 0U;
        ret = true;
    }
    else
    {
        ret = false;
    }

    return ret;
}
