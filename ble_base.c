#include "nrf.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_gq.h"
#include "nrf_ble_qwr.h"
#include "nrf_power.h"
#include "nrf_pwr_mgmt.h"

#include "app_scheduler.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "nrf_sdh_soc.h"

#include "nrf_bootloader_info.h"
#include "nrf_dfu_ble_svci_bond_sharing.h"
#include "nrf_svci_async_function.h"
#include "nrf_svci_async_handler.h"

#include "nrf_log.h"

#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_conn_params.h"
#include "ble_conn_state.h"
#include "ble_dfu.h"

#include "ble_base.h"
#include "nrf_log.h"
#include "utils.h"

#include "ble_lbs.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "boards.h"

NRF_BLE_QWR_DEF(m_qwr);                                                                     /**< Context for the Queued Write module.*/
NRF_BLE_GATT_DEF(m_gatt);                                                                   /**< GATT module instance. */
BLE_ADVERTISING_DEF(m_advertising);                                                         /**< Advertising module instance. */
NRF_BLE_GQ_DEF(m_ble_gatt_queue, NRF_SDH_BLE_PERIPHERAL_LINK_COUNT, NRF_BLE_GQ_QUEUE_SIZE); /**< BLE GATT Queue instance. */
BLE_LBS_DEF(m_lbs);                                                                         /**< LED Button Service instance. */

static ble_gap_addr_t p_addr;
static uint16_t com_current_ble_connection_handle; /**< Handle of the current connection. */

/**
 * @brief 处理BLE事件的回调函数。
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 * @param[in]   p_context   Unused.
 */
static void ble_evt_handler(ble_evt_t const *p_ble_evt, void *p_context)
{
    ret_code_t err_code = NRF_SUCCESS;

    switch (p_ble_evt->header.evt_id)
    {
    case BLE_GAP_EVT_CONNECTED:
        NRF_LOG_INFO("Connected.");

        com_current_ble_connection_handle = p_ble_evt->evt.gap_evt.conn_handle;
        err_code = nrf_ble_qwr_conn_handle_assign(&m_qwr, com_current_ble_connection_handle);
        APP_ERROR_CHECK(err_code);
        break;

    case BLE_GAP_EVT_DISCONNECTED:
        NRF_LOG_INFO("Disconnected.");
        com_current_ble_connection_handle = BLE_CONN_HANDLE_INVALID;

        break; // BLE_GAP_EVT_DISCONNECTED

    case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
    {
        NRF_LOG_DEBUG("PHY update request.");
        ble_gap_phys_t const phys = {
            .rx_phys = BLE_GAP_PHY_AUTO,
            .tx_phys = BLE_GAP_PHY_AUTO,
        };
        err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
        APP_ERROR_CHECK(err_code);
    }
    break;

    case BLE_GATTC_EVT_TIMEOUT:
        // Disconnect on GATT Client timeout event.
        NRF_LOG_DEBUG("GATT Client Timeout.");
        com_current_ble_connection_handle = BLE_CONN_HANDLE_INVALID;
        err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
        APP_ERROR_CHECK(err_code);
        break;

    case BLE_GATTS_EVT_TIMEOUT:
        // Disconnect on GATT Server timeout event.
        NRF_LOG_DEBUG("GATT Server Timeout.");
        err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
        APP_ERROR_CHECK(err_code);
        break;

    default:
        // No implementation needed.
        break;
    }
}

/**
 * @brief 初始化BLE模块的函数。
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void)
{
    ret_code_t err_code;

    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    // Configure the BLE stack using the default settings.
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

    // Register a handler for BLE events.
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);
}

/**
 * @brief 处理广播事件。
 *
 * @details This function will be called for advertising events which are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
    ret_code_t err_code;

    switch (ble_adv_evt)
    {
    case BLE_ADV_EVT_DIRECTED_HIGH_DUTY:
        NRF_LOG_INFO("Directed advertising");
        break;

    case BLE_ADV_EVT_FAST:
        NRF_LOG_INFO("Fast advertising");
        break;

    case BLE_ADV_EVT_SLOW:
        NRF_LOG_INFO("Slow advertising");
        break;

    case BLE_ADV_EVT_IDLE:
        // TODO: sleep mode
        NRF_LOG_INFO("BLE_ADV_EVT_IDLE");
        err_code = ble_advertising_start(&m_advertising, BLE_ADV_EVT_SLOW);
        APP_ERROR_CHECK(err_code);
        break;

    default:
        break;
    }
}

void on_advertising_error(uint32_t nrf_error)
{
    LOG_ERROR("Bluetooth advertising", nrf_error);
}

/**
 * @brief 初始化广播功能。
 */
static void advertising_init(void)
{
    ret_code_t err_code;

    // static int8_t tx_power_level = 4;

    // err_code = sd_ble_gap_tx_power_set(BLE_GAP_TX_POWER_ROLE_ADV, 0, tx_power_level);
    // APP_ERROR_CHECK(err_code);

    ble_advdata_manuf_data_t manuf_specific_data;

    // Company id，Nordic id is: 0x0059
    manuf_specific_data.company_identifier = 0x0059;
    manuf_specific_data.data.p_data = p_addr.addr;
    manuf_specific_data.data.size = sizeof(p_addr.addr);

    ble_advertising_init_t init;

    memset(&init, 0, sizeof(init));

    init.advdata.name_type = BLE_ADVDATA_FULL_NAME;
    init.advdata.include_appearance = true;
    init.advdata.flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    init.advdata.uuids_complete.uuid_cnt = 0;
    init.advdata.uuids_complete.p_uuids = NULL;
    init.advdata.p_manuf_specific_data = &manuf_specific_data;
    // init.advdata.p_tx_power_level        = &tx_power_level;  // 发送功率

    init.config.ble_adv_fast_enabled = true;
    init.config.ble_adv_fast_interval = APP_ADV_FAST_INTERVAL;
    init.config.ble_adv_fast_timeout = APP_ADV_FAST_DURATION;
    init.config.ble_adv_slow_enabled = true;
    init.config.ble_adv_slow_interval = APP_ADV_SLOW_INTERVAL;
    init.config.ble_adv_slow_timeout = APP_ADV_SLOW_DURATION;

    init.evt_handler = on_adv_evt;
    init.error_handler = on_advertising_error;

    err_code = ble_advertising_init(&m_advertising, &init);
    APP_ERROR_CHECK(err_code);

    ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);
}

/**
 * @brief 初始化GAP的函数。
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */
static void gap_params_init(void)
{
    ret_code_t err_code;
    ble_gap_conn_params_t gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    err_code = sd_ble_gap_addr_get(&p_addr);
    APP_ERROR_CHECK(err_code);

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    char device_name[12];

    sprintf(device_name, "BLE_%X%X%X%X", p_addr.addr[5], p_addr.addr[4], p_addr.addr[3], p_addr.addr[2]);
    err_code = sd_ble_gap_device_name_set(&sec_mode, (const uint8_t *)device_name, strlen(device_name));
    APP_ERROR_CHECK(err_code);

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}

/**
 * @brief 初始化gatt模块的函数。
 */
static void gatt_init(void)
{
    ret_code_t err_code = nrf_ble_gatt_init(&m_gatt, NULL);
    APP_ERROR_CHECK(err_code);

    err_code = nrf_ble_gatt_att_mtu_periph_set(&m_gatt, NRF_SDH_BLE_GATT_MAX_MTU_SIZE);
    APP_ERROR_CHECK(err_code);
}

/**
 * @brief 开启广播的函数。
 */
ret_code_t advertising_start()
{
    return ble_advertising_start(&m_advertising, BLE_ADV_MODE_SLOW);
}

ret_code_t advertising_stop()
{
    return sd_ble_gap_adv_stop(m_advertising.adv_handle);
}

/**
 * @brief 处理连接参数错误的回调函数。
 *
 * @param[in] nrf_error 错误码。
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

/**
 * @brief 初始化连接参数模块。
 */
static void conn_params_init(void)
{
    uint32_t err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail = true;
    cp_init.evt_handler = NULL;
    cp_init.error_handler = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}

/**
 * @brief 获取广播配置
 */
static void advertising_config_get(ble_adv_modes_config_t *p_config)
{
    memset(p_config, 0, sizeof(ble_adv_modes_config_t));

    p_config->ble_adv_fast_enabled = true;
    p_config->ble_adv_fast_interval = APP_ADV_INTERVAL;
    p_config->ble_adv_fast_timeout = APP_ADV_DURATION;
}

static void scheduler_controller(void *p_event_data, uint16_t event_size)
{
    UNUSED_PARAMETER(event_size);

    uint8_t state = *(uint8_t *)p_event_data;

    switch (state)
    {
    case 1: // 短按
        nrf_gpio_cfg(BOADER_CONTROL_PIN, NRF_GPIO_PIN_DIR_OUTPUT, NRF_GPIO_PIN_INPUT_DISCONNECT, NRF_GPIO_PIN_PULLDOWN, NRF_GPIO_PIN_S0S1, NRF_GPIO_PIN_NOSENSE);
        nrf_delay_ms(600);
        nrf_gpio_cfg(BOADER_CONTROL_PIN, NRF_GPIO_PIN_DIR_INPUT, NRF_GPIO_PIN_INPUT_DISCONNECT, NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_S0S1, NRF_GPIO_PIN_NOSENSE);

        break;

    case 2: // 长按
        nrf_gpio_cfg(BOADER_CONTROL_PIN, NRF_GPIO_PIN_DIR_OUTPUT, NRF_GPIO_PIN_INPUT_DISCONNECT, NRF_GPIO_PIN_PULLDOWN, NRF_GPIO_PIN_S0S1, NRF_GPIO_PIN_NOSENSE);
        nrf_delay_ms(4000);
        nrf_gpio_cfg(BOADER_CONTROL_PIN, NRF_GPIO_PIN_DIR_INPUT, NRF_GPIO_PIN_INPUT_DISCONNECT, NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_S0S1, NRF_GPIO_PIN_NOSENSE);
        break;

    default:
        break;
    }
}

/**@brief Function for handling write events to the LED characteristic.
 *
 * @param[in] p_lbs     Instance of LED Button Service to which the write applies.
 * @param[in] led_state Written/desired state of the LED.
 */
static void led_write_handler(uint16_t conn_handle, ble_lbs_t *p_lbs, uint8_t state)
{
    UNUSED_PARAMETER(conn_handle);

    app_sched_event_put(&state, sizeof(state), scheduler_controller);
}

/**
 * @brief 处理QWR服务错误的回调函数。
 */
static void services_error_handler(uint32_t nrf_error)
{
    LOG_ERROR("Qwr", nrf_error);
    APP_ERROR_HANDLER(nrf_error);
}

/**
 * @brief 初始化GATT服务的函数。
 *
 * @details Initialize the Blood Pressure, Battery, and Device Information services.
 */
static void services_init(void)
{
    uint32_t err_code;

    nrf_ble_qwr_init_t qwr_init = {0};
    // Initialize Queued Write Module.
    qwr_init.error_handler = services_error_handler;

    err_code = nrf_ble_qwr_init(&m_qwr, &qwr_init);
    APP_ERROR_CHECK(err_code);

    ble_lbs_init_t init = {0};

    // Initialize LBS.
    init.led_write_handler = led_write_handler;

    err_code = ble_lbs_init(&m_lbs, &init);
    APP_ERROR_CHECK(err_code);
}

ret_code_t ble_base_init()
{
    ret_code_t err_code;

    // 初始化低功耗蓝牙栈。
    ble_stack_init();

    // 初始化GAP参数。
    gap_params_init();

    // 初始化GATT。
    gatt_init();

    // 初始化广播参数。
    advertising_init();

    // 注册GATT服务。
    services_init();

    // 初始化连接参数模块。
    conn_params_init();

    return NRF_SUCCESS;
}