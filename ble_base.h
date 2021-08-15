#ifndef BLE_BASE_H
#define BLE_BASE_H

#include "app_timer.h"

#define APP_BLE_OBSERVER_PRIO 3     /**< Application's BLE observer priority. You shouldn't need to modify this value. */
#define APP_BLE_CONN_CFG_TAG 1      /**< A tag identifying the SoftDevice BLE configuration. */
#define MANUFACTURER_NAME "ECO_NRF" /**< Manufacturer. Will be passed to Device Information Service. */

#define SLAVE_LATENCY 0                                  /**< Slave latency. */
#define CONN_SUP_TIMEOUT MSEC_TO_UNITS(4000, UNIT_10_MS) /**< Connection supervisory timeout (4 seconds). */

#define APP_ADV_INTERVAL MSEC_TO_UNITS(25, UNIT_0_625_MS) /**< The advertising interval (in units of 0.625 ms. This value corresponds to 187.5 ms). */
#define APP_ADV_DURATION 18000                            /**< The advertising duration (180 seconds) in units of 10 milliseconds. */

#define APP_ADV_FAST_INTERVAL 40    /**< The advertising interval (in units of 0.625 ms. This value corresponds to 25 ms). */
#define APP_ADV_FAST_DURATION 3000  /**< The advertising duration of fast advertising in units of 10 milliseconds. */
#define APP_ADV_SLOW_INTERVAL 800   /**< Slow advertising interval (in units of 0.625 ms. This value corresponds to 0.5 seconds). */
#define APP_ADV_SLOW_DURATION 18000 /**< The advertising duration of slow advertising in units of 10 milliseconds. */

#define MIN_CONN_INTERVAL MSEC_TO_UNITS(10, UNIT_1_25_MS) /**< Minimum acceptable connection interval (0.5 seconds). */
#define MAX_CONN_INTERVAL MSEC_TO_UNITS(20, UNIT_1_25_MS) /**< Maximum acceptable connection interval (1 second). */

#define FIRST_CONN_PARAMS_UPDATE_DELAY                                                                                                                         \
    APP_TIMER_TICKS(5000) /**< Time from initiating event (connect or start of indication) to first time sd_ble_gap_conn_param_update is called (5 seconds).   \
                           */
#define NEXT_CONN_PARAMS_UPDATE_DELAY APP_TIMER_TICKS(30000) /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT 3                       /**< Number of attempts before giving up the connection parameter negotiation. */

ret_code_t advertising_start();
ret_code_t advertising_stop();
void       disconnect(uint16_t conn_handle, void* p_context);
ret_code_t ble_base_init();

#endif