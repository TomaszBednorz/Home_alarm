#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "app.h"
#include "app_cfg.h"
#include "hy_srf.h"
#include "ir.h"
#include "user_can.h"

/* 
 * Local function protitypes
 */
static void App_Task(void *arg);
static void App_ProcessHySrf(void);
static void App_ProcessIr(void);

/*
 * Local objects
 */
static TaskHandle_t App_TaskHandle = NULL;

/*
 * Application initialization
 */
void App_Init(void)
{
    // Create App task
    xTaskCreate(App_Task, "app_task", 4096, NULL, APP_TASK_PRIORITY, &App_TaskHandle);
}

/*
 * App task
 */
static void App_Task(void *arg)
{
    TickType_t xLastWakeTime;

    xLastWakeTime = xTaskGetTickCount();

    while(1)
    {
        App_ProcessHySrf();
        App_ProcessIr();

        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(APP_TASK_PERIOD));
    }
}

/*
 * HY-SRF data processing. The CAN frame is filled
 */
static void App_ProcessHySrf(void)
{
    uint16_t distance = 0U;
    uint8_t can_data[2];

    distance = HySrf_GetLastDistance();

    /* Swap byte order */
    can_data[1] = (uint8_t)(distance & 0x00FF);
    can_data[0] = (uint8_t)((distance & 0xFF00) >> 8U);

    /* Update CAN frame data*/
    UserCan_FillFrame(UserCanFrame1, can_data);
}

/*
 * IR data processing. The CAN frame is filled
 */
static void App_ProcessIr(void)
{
    uint8_t status = 0U;
    uint8_t can_data;

    status = Ir_GetLastStatus();

    /* Load IR status into CAN frame */
    can_data = (status & 0x01);

    /* Update CAN frame data*/
    UserCan_FillFrame(UserCanFrame2, &can_data);
}
