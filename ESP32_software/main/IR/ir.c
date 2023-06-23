#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "ir.h"
#include "ir_cfg.h"

/* Error handler for the development phase */
#define IR_ERROR_HANDLER()     do {} while (1)

#define IR_STATE_ON            (1U)
#define IR_STATE_OFF           (0U)

typedef struct
{
    uint8_t current_state;
    uint8_t debounced_state;
    uint8_t debouncing_cnt;
}Ir_Data_t;

/* 
 * Local function protitypes
 */
static void Ir_Task(void *arg);
static void Ir_ProcessData(void);

/*
 * Local objects
 */
static TaskHandle_t Ir_TaskHandle = NULL;
static Ir_Data_t Ir_Data = {.current_state = IR_STATE_OFF, 
                            .debounced_state = IR_STATE_OFF, 
                            .debouncing_cnt = 0U};

/*
 * IR initialization function
 */
void Ir_Init(void)
{
    // OUT pin
    gpio_config_t out_pin = 
    {
        .pin_bit_mask = (1 << IR_OUT_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };

    /* OUT pin configuration */
    if(gpio_config(&out_pin) != ESP_OK)
    {
        IR_ERROR_HANDLER();
    }

    // Create HY_SRF task
    xTaskCreate(Ir_Task, "ir_task", 4096, NULL, IR_TASK_PRIORITY, &Ir_TaskHandle);

}

/*
 * Function returns last debounced IR value (0 = false, 1 = true)
 */
uint8_t Ir_GetLastStatus(void)
{
    return Ir_Data.debounced_state;
}

/*
 * IR task
 */
static void Ir_Task(void *arg)
{
    TickType_t xLastWakeTime;

    xLastWakeTime = xTaskGetTickCount();

    while(1)
    { 
        Ir_ProcessData();
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(IR_TASK_PERIOD));
    }
}

/*
 * IR data processing:
 * - update current value
 * - add value to debouncing queue
 * - update debounced value
 */
static void Ir_ProcessData(void)
{
    Ir_Data.current_state = (gpio_get_level(IR_OUT_PIN) == 1) ? IR_STATE_ON : IR_STATE_OFF;

    if(Ir_Data.current_state != Ir_Data.debounced_state)
    {
        Ir_Data.debouncing_cnt++;

        if(Ir_Data.debouncing_cnt >= IR_DEBOUNCING_QUEUE_SIZE)
        {
            Ir_Data.debouncing_cnt = 0U;
            Ir_Data.debounced_state = Ir_Data.current_state;
        }
    }
    else
    {
        Ir_Data.debouncing_cnt = 0U;
    }
}