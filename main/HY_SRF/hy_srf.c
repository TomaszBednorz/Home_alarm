#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/gptimer.h"
#include <rom/ets_sys.h>

#include "hy_srf.h"
#include "hy_srf_cfg.h"

/* Error handler for the development phase */
#define HY_SRF_ERROR_HANDLER()     do {} while (1)

#define HY_SRF_SOUND_SPEED         (0.0343f)  /* cm/us */
#define HY_SRF_RESET_TIMER_VAL     (0U)

typedef struct
{
    float averaged_value;
    float averaging_queue[IR_AVERAGING_QUEUE_SIZE];
}HySrf_Data_t;

typedef enum
{
    HY_SRF_START_MEASUREMENT = 0U,
    HY_SRF_STOP_MEASUREMENT
}HySrf_StateMachine_t;

/* 
 * Local function protitypes
 */
static void HySrf_ISR(void *arg);
static void HySrf_Task(void *arg);
static void HySrf_ProcessData(void);

/*
 * Local objects
 */
static TaskHandle_t HySrf_TaskHandle = NULL;
static gptimer_handle_t HySrf_Gptimer = NULL;
static uint64_t HySrf_TimerValue;
static HySrf_StateMachine_t HySrf_StateMachine = HY_SRF_START_MEASUREMENT;
static HySrf_Data_t HySrf_Data = {0};

/*
 * HY-SRF05 initialization function
 */
void HySrf_Init(void)
{
    // TRIG pin
    gpio_config_t trig_pin = 
    {
        .pin_bit_mask = (1 << HY_SFR_TRIG_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };

    // ECHO pin
    gpio_config_t echo_pin = 
    {
        .pin_bit_mask = (1 << HY_SFR_ECHO_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_ANYEDGE
    };

    // Timer
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 1 * 1000 * 1000, // 1MHz, 1 tick = 1us
    };

    /* Timer configuration */
    if(gptimer_new_timer(&timer_config, &HySrf_Gptimer) != ESP_OK)
    {
        HY_SRF_ERROR_HANDLER();
    }

    if(gptimer_enable(HySrf_Gptimer) != ESP_OK)
    {
        HY_SRF_ERROR_HANDLER();
    }

    if(gptimer_set_raw_count(HySrf_Gptimer, HY_SRF_RESET_TIMER_VAL) != ESP_OK)
    {
        HY_SRF_ERROR_HANDLER();
    }

    /* TRIG pin configuration */
    if(gpio_config(&trig_pin) != ESP_OK)
    {
        HY_SRF_ERROR_HANDLER();
    }

    gpio_set_level(HY_SFR_TRIG_PIN, 0);

    /* ECHO pin  configuration */
    if(gpio_config(&echo_pin) != ESP_OK)
    {
        HY_SRF_ERROR_HANDLER();
    }

    if(gpio_install_isr_service(0) != ESP_OK)
    {
        HY_SRF_ERROR_HANDLER();
    }

    if(gpio_isr_handler_add(HY_SFR_ECHO_PIN, HySrf_ISR, (void *)HY_SFR_ECHO_PIN) != ESP_OK)
    {
        HY_SRF_ERROR_HANDLER();
    }

    // Create HY_SRF task
    xTaskCreate(HySrf_Task, "hy_srf_task", 4096, NULL, HY_SFR_TASK_PRIORITY, &HySrf_TaskHandle);
}

/*
 * Function returns last averaged distance value
 */
uint16_t HySrf_GetLastDistance(void)
{
    return HySrf_Data.averaged_value;
}

/*
 * HY-SRF05 task
 */
static void HySrf_Task(void *arg)
{
    TickType_t xLastWakeTime;

    xLastWakeTime = xTaskGetTickCount();

    gpio_set_level(HY_SFR_TRIG_PIN, 1);
    ets_delay_us(10);
    gpio_set_level(HY_SFR_TRIG_PIN, 0);

    while(1)
    {
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(HY_SFR_TASK_PERIOD));

        /* Process last measured data */
        HySrf_ProcessData();

        gpio_set_level(HY_SFR_TRIG_PIN, 1);
        ets_delay_us(10);
        gpio_set_level(HY_SFR_TRIG_PIN, 0);
        
    }
}

/*
 * Hy-SRF data processing:
 * - update last value and translate to cm
 * - add last value to averaging queue
 * - update averaged value
 */
static void HySrf_ProcessData(void)
{
    float queue_sum = 0.0f;

    for(uint8_t i = 0; i < (IR_AVERAGING_QUEUE_SIZE - 1); i++)
    {
        /* Move each queue item one position further */
        HySrf_Data.averaging_queue[i+1] = HySrf_Data.averaging_queue[i];

        queue_sum += HySrf_Data.averaging_queue[i+1];
    }

    HySrf_Data.averaging_queue[0] = (float)HySrf_TimerValue * HY_SRF_SOUND_SPEED / 2.0f;
    queue_sum += HySrf_Data.averaging_queue[0];

    HySrf_Data.averaged_value = queue_sum / IR_AVERAGING_QUEUE_SIZE;
}

/*
 * HY-SRF05 Interrupt Service Routine
 */
static void HySrf_ISR(void *arg)
{
    switch (HySrf_StateMachine)
    {
    case HY_SRF_START_MEASUREMENT:
        if(gptimer_start(HySrf_Gptimer) != ESP_OK)
        {
            HY_SRF_ERROR_HANDLER();
        }
        HySrf_StateMachine = HY_SRF_STOP_MEASUREMENT;
        break;
    case HY_SRF_STOP_MEASUREMENT:
        if(gptimer_get_raw_count(HySrf_Gptimer, &HySrf_TimerValue) != ESP_OK)
        {
            HY_SRF_ERROR_HANDLER();
        }
        if(gptimer_stop(HySrf_Gptimer) != ESP_OK)
        {
            HY_SRF_ERROR_HANDLER();
        }
        if(gptimer_set_raw_count(HySrf_Gptimer, HY_SRF_RESET_TIMER_VAL) != ESP_OK)
        {
            HY_SRF_ERROR_HANDLER();
        }

        HySrf_StateMachine = HY_SRF_START_MEASUREMENT;
        break;  
    default:
        break;
    }
}