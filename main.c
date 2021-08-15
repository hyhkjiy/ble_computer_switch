#include "nrf_nvic.h"
#include <inttypes.h>
#include <stdio.h>
#include <time.h>

#include "app_scheduler.h"
#include "app_timer.h"
#include "ble_types.h"
#include "boards.h"

#include "nrf_delay.h"
#include "nrf_drv_saadc.h"
#include "nrf_drv_wdt.h"
#include "nrf_gpio.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_power.h"
#include "nrf_soc.h"

#include "nrf_drv_clock.h"
#include "nrf_drv_gpiote.h"
#include "nrf_pwr_mgmt.h"

#include "utils.h"
#include "ble_base.h"

#define SCHED_QUEUE_SIZE 20           /**< Maximum number of events in the scheduler queue. */
#define SCHED_MAX_EVENT_DATA_SIZE 192 /**< Maximum size of scheduler events. */

#define TIME_UPDATE_INTERVAL APP_TIMER_TICKS(1000) /**< Time update interval (ticks). */

nrf_drv_wdt_channel_id m_channel_id;   // 看门狗。
APP_TIMER_DEF(m_time_update_timer_id); /**< Time update timer. */

/**
 * @brief 初始化日志模块
 */
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

/**
 * @brief 计时器回调函数（每秒钟调用一次）。
 *
 * @param[in]   p_context   Pointer used for passing some arbitrary information (context) from the
 *                          app_start_timer() call to the timeout handler.
 */
static void time_update_handler(void *p_context)
{
    UNUSED_PARAMETER(p_context);

    nrf_drv_wdt_channel_feed(m_channel_id); // 喂狗
}

/**
 * @brief 处理GPIO输入事件的函数。
 */
static void input_pin_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    ret_code_t err_code;
    NRF_LOG_DEBUG("input_pin_handler: %d, %d", pin, action);
    switch (pin)
    {
    case BOADER_BUTTON_PIN:
    {
        if (nrf_gpio_pin_read(BOADER_BUTTON_PIN))
        {
            // 松开
            nrf_gpio_cfg(BOADER_CONTROL_PIN, NRF_GPIO_PIN_DIR_INPUT, NRF_GPIO_PIN_INPUT_DISCONNECT, NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_S0S1, NRF_GPIO_PIN_NOSENSE);
        }
        else
        {
            // 按下
            nrf_gpio_cfg(BOADER_CONTROL_PIN, NRF_GPIO_PIN_DIR_OUTPUT, NRF_GPIO_PIN_INPUT_DISCONNECT, NRF_GPIO_PIN_PULLDOWN, NRF_GPIO_PIN_S0S1, NRF_GPIO_PIN_NOSENSE);
        }
        break;
    }
    default:
        break;
    }
}

/**
 * @brief 初始化GPIO引脚。
 */
static void gpio_init()
{
    nrf_gpio_cfg(BOADER_CONTROL_PIN, NRF_GPIO_PIN_DIR_INPUT, NRF_GPIO_PIN_INPUT_DISCONNECT, NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_S0S1, NRF_GPIO_PIN_NOSENSE);

    ret_code_t err_code;
    //初始化GPIOTE程序模块
    err_code = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(err_code);

    // 定义GPIOTE配置结构体，配置为下降沿触发（按键是低电平有效），低精度
    nrf_drv_gpiote_in_config_t in_config_hitlo = GPIOTE_CONFIG_IN_SENSE_TOGGLE(false);
    // 开启引脚的上拉电阻
    in_config_hitlo.pull = NRF_GPIO_PIN_PULLUP;

    // 设置充电状态引脚为GPIOTE输入
    err_code = nrf_drv_gpiote_in_init(BOADER_BUTTON_PIN, &in_config_hitlo, input_pin_handler);
    APP_ERROR_CHECK(err_code);
    // 使能充电状态引脚感知功能
    nrf_drv_gpiote_in_event_enable(BOADER_BUTTON_PIN, true);
}

/**
 * @brief 初始化计时器的函数。
 *
 * @details Initializes the timer module. This creates and starts application timers.
 */
static void timers_init(void)
{
    ret_code_t err_code;

    err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_clock_lfclk_request(NULL);

    // Initialize timer module.
    err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    // Create timers.
    err_code = app_timer_create(&m_time_update_timer_id, APP_TIMER_MODE_REPEATED, time_update_handler);
    APP_ERROR_CHECK(err_code);
}

/**
 * @brief 启动程序计时器的函数。
 */
static void application_timers_start(void)
{
    ret_code_t err_code;

    // Start application timers.
    err_code = app_timer_start(m_time_update_timer_id, TIME_UPDATE_INTERVAL, NULL);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling the idle state (main loop).
 *
 * @details If there is no pending log operation, then sleep until next the next event occurs.
 */
static void idle_state_handle(void)
{
    app_sched_execute();
    if (NRF_LOG_PROCESS() == false)
    {
        nrf_pwr_mgmt_run();
    }
}

/**
 * @brief WDT events handler.
 */
void wdt_event_handler(void)
{
    NRF_LOG_WARNING("The system was restarted by WDT!");
    NRF_LOG_PROCESS();
    sd_nvic_SystemReset();
}

static void watch_dog_init()
{
    ret_code_t err_code;
    // Configure WDT.
    nrf_drv_wdt_config_t config = NRF_DRV_WDT_DEAFULT_CONFIG;
    // config.behaviour = NRF_WDT_BEHAVIOUR_PAUSE_SLEEP_HALT;
    // config.reload_value = 3000;
    err_code = nrf_drv_wdt_init(&config, wdt_event_handler);
    APP_ERROR_CHECK(err_code);
    err_code = nrf_drv_wdt_channel_alloc(&m_channel_id);
    APP_ERROR_CHECK(err_code);
}

/**
 * @brief 初始化电源管理模块的函数。
 */
static void power_management_init(void)
{
    ret_code_t err_code;
    err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
}

/*********************************************************************
 *
 *       start_app()
 *
 *  Function description
 *   Application entry point.
 */
int main(void)
{
    ret_code_t err_code;

    // 使用DCDC稳压器。
    NRF_POWER->DCDCEN = 1;

    // 初始化电源管理模块。
    power_management_init();

    // 初始化日志模块。
    log_init();
    NRF_LOG_INFO("Logging on!");

    // 初始化定时器。
    timers_init();

    // 初始化GPIO引脚。
    gpio_init();

    // 初始化看门狗。
    watch_dog_init();

    // 初始化调度器
    APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);

    err_code = ble_base_init();
    APP_ERROR_CHECK(err_code);

    // 启动定时器。
    application_timers_start();
    NRF_LOG_INFO("application timers started!");

    // 开启看门狗。
    nrf_drv_wdt_enable();

    // 开启广播。
    err_code = advertising_start();
    LOG_ERROR("Start advertising", err_code);

    // Enter main loop.
    for (;;)
    {
        idle_state_handle();
    }
}

/*************************** End of file ****************************/