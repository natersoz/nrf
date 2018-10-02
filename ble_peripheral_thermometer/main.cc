/**
 * Copyright (c) 2014 - 2017, Nordic Semiconductor ASA
 * All rights reserved. Blah, blah, blah.
 *
 * @brief Health Thermometer Service Sample Application main file.
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "nordic_common.h"
#include "ble.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_bas.h"
#include "ble_conn_params.h"
#include "ble_conn_state.h"
#include "ble_dis.h"
#include "ble_err.h"
#include "ble_hci.h"
#include "ble_hts.h"
#include "ble_srv_common.h"

#include "nrf.h"
#include "nrf_ble_gatt.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "nrf_sdh_soc.h"

#include "fds.h"
#include "peer_manager.h"
#include "sensorsim.h"

#include "app_timer.h"
#include "clocks.h"
#include "leds.h"
#include "buttons.h"
#include "logger.h"
#include "segger_rtt_output_stream.h"
#include "rtc_observer.h"

#define DEVICE_NAME                     "Nordic_HTS"                                /**< Name of device. Will be included in the advertising data. */
#define MANUFACTURER_NAME               "NordicSemiconductor"                       /**< Manufacturer. Will be passed to Device Information Service. */
#define MODEL_NUM                       "NS-HTS-EXAMPLE"                            /**< Model number. Will be passed to Device Information Service. */
#define MANUFACTURER_ID                 0x1122334455                                /**< Manufacturer ID, part of System ID. Will be passed to Device Information Service. */
#define ORG_UNIQUE_ID                   0x667788                                    /**< Organizational Unique ID, part of System ID. Will be passed to Device Information Service. */

#define APP_BLE_OBSERVER_PRIO           3                                           /**< Application's BLE observer priority. You shouldn't need to modify this value. */
#define APP_BLE_CONN_CFG_TAG            1                                           /**< A tag identifying the SoftDevice BLE configuration. */

#define APP_ADV_INTERVAL                40                                          /**< The advertising interval (in units of 0.625 ms. This value corresponds to 25 ms). */
#define APP_ADV_TIMEOUT_IN_SECONDS      180                                         /**< The advertising timeout in units of seconds. */

#define MIN_BATTERY_LEVEL               81                                          /**< Minimum battery level as returned by the simulated measurement function. */
#define MAX_BATTERY_LEVEL               100                                         /**< Maximum battery level as returned by the simulated measurement function. */
#define BATTERY_LEVEL_INCREMENT         1                                           /**< Value by which the battery level is incremented/decremented for each call to the simulated measurement function. */

#define TEMP_TYPE_AS_CHARACTERISTIC     0                                           /**< Determines if temperature type is given as characteristic (1) or as a field of measurement (0). */

#define MIN_CELCIUS_DEGREES             3688                                        /**< Minimum temperature in celcius for use in the simulated measurement function (multiplied by 100 to avoid floating point arithmetic). */
#define MAX_CELCIUS_DEGRESS             3972                                        /**< Maximum temperature in celcius for use in the simulated measurement function (multiplied by 100 to avoid floating point arithmetic). */
#define CELCIUS_DEGREES_INCREMENT       36                                          /**< Value by which temperature is incremented/decremented for each call to the simulated measurement function (multiplied by 100 to avoid floating point arithmetic). */

#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(500, UNIT_1_25_MS)            /**< Minimum acceptable connection interval (0.5 seconds) */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(1000, UNIT_1_25_MS)           /**< Maximum acceptable connection interval (1 second). */
#define SLAVE_LATENCY                   0                                           /**< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)             /**< Connection supervisory timeout (4 seconds). */

#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000)                       /**< Time from initiating event (connect or start of indication) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(30000)                      /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                           /**< Number of attempts before giving up the connection parameter negotiation. */

#define SEC_PARAM_BOND                  1                                           /**< Perform bonding. */
#define SEC_PARAM_MITM                  0                                           /**< Man In The Middle protection not required. */
#define SEC_PARAM_LESC                  0                                           /**< LE Secure Connections not enabled. */
#define SEC_PARAM_KEYPRESS              0                                           /**< Keypress notifications not enabled. */
#define SEC_PARAM_IO_CAPABILITIES       BLE_GAP_IO_CAPS_NONE                        /**< No I/O capabilities. */
#define SEC_PARAM_OOB                   0                                           /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE          7                                           /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE          16                                          /**< Maximum encryption key size. */

#define DEAD_BEEF                       0xDEADBEEF                                  /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

#define APP_FEATURE_NOT_SUPPORTED       BLE_GATT_STATUS_ATTERR_APP_BEGIN + 2        /**< Reply when unsupported features are requested. */


BLE_BAS_DEF(m_bas);                                                                 /**< Structure used to identify the battery service. */
BLE_HTS_DEF(m_hts);                                                                 /**< Structure used to identify the health thermometer service. */
NRF_BLE_GATT_DEF(m_gatt);                                                           /**< GATT module instance. */
BLE_ADVERTISING_DEF(m_advertising);                                                 /**< Advertising module instance. */

static uint16_t          m_conn_handle = BLE_CONN_HANDLE_INVALID;                   /**< Handle of the current connection. */
static bool              m_hts_meas_ind_conf_pending = false;                       /**< Flag to keep track of when an indication confirmation is pending. */
static sensorsim_cfg_t   m_battery_sim_cfg;                                         /**< Battery Level sensor simulator configuration. */
static sensorsim_state_t m_battery_sim_state;                                       /**< Battery Level sensor simulator state. */
static sensorsim_cfg_t   m_temp_celcius_sim_cfg;                                    /**< Temperature simulator configuration. */
static sensorsim_state_t m_temp_celcius_sim_state;                                  /**< Temperature simulator state. */

static ble_uuid_t m_adv_uuids[] =                                                   /**< Universally unique service identifiers. */
{
    { BLE_UUID_HEALTH_THERMOMETER_SERVICE, BLE_UUID_TYPE_BLE },
    { BLE_UUID_BATTERY_SERVICE, BLE_UUID_TYPE_BLE },
    { BLE_UUID_DEVICE_INFORMATION_SERVICE, BLE_UUID_TYPE_BLE }
};


static void advertising_start(bool erase_bonds);
static void temperature_measurement_send(void);


/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num   Line number of the failing ASSERT call.
 * @param[in] file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t *p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}

/**@brief Function for handling Peer Manager events.
 *
 * @param[in] p_evt  Peer Manager event.
 */
static void pm_evt_handler(pm_evt_t const *p_evt)
{
    switch (p_evt->evt_id)
    {
    case PM_EVT_BONDED_PEER_CONNECTED:
        {
            logger::instance().info("Connected to a previously bonded device.");
        }
        break;

    case PM_EVT_CONN_SEC_SUCCEEDED:
        {
            logger::instance().info(
                "Connection secured: role: %d, conn_handle: 0x%x, procedure: %d.",
                ble_conn_state_role(p_evt->conn_handle),
                p_evt->conn_handle,
                p_evt->params.conn_sec_succeeded.procedure);

            // Send a single temperature measurement if indication is enabled.
            // NOTE: For this to work, make sure ble_hts_on_ble_evt() is called before
            // pm_evt_handler() in ble_evt_dispatch().
            bool is_indication_enabled = false;
            uint32_t const error_code = ble_hts_is_indication_enabled(
                &m_hts, &is_indication_enabled);
            APP_ERROR_CHECK(error_code);
            if (is_indication_enabled)
            {
                temperature_measurement_send();
            }
        }
        break;

    case PM_EVT_CONN_SEC_FAILED:
        {
            /* Often, when securing fails, it shouldn't be restarted, for security reasons.
             * Other times, it can be restarted directly.
             * Sometimes it can be restarted, but only after changing some Security Parameters.
             * Sometimes, it cannot be restarted until the link is disconnected and reconnected.
             * Sometimes it is impossible, to secure the link, or the peer device does not support it.
             * How to handle this error is highly application dependent. */
        }
        break;

    case PM_EVT_CONN_SEC_CONFIG_REQ:
        {
            // Reject pairing request from an already bonded peer.
            pm_conn_sec_config_t conn_sec_config = { .allow_repairing = false };
            pm_conn_sec_config_reply(p_evt->conn_handle, &conn_sec_config);
        }
        break;

    case PM_EVT_STORAGE_FULL:
        {
            // Run garbage collection on the flash.
            uint32_t const error_code = fds_gc();
            if (error_code == FDS_ERR_BUSY || error_code == FDS_ERR_NO_SPACE_IN_QUEUES)
            {
                // Retry.
            }
            else
            {
                APP_ERROR_CHECK(error_code);
            }
        }
        break;

    case PM_EVT_PEERS_DELETE_SUCCEEDED:
        {
            advertising_start(false);
        }
        break;

    case PM_EVT_LOCAL_DB_CACHE_APPLY_FAILED:
        {
            // The local database has likely changed, send service changed indications.
            pm_local_database_has_changed();
        }
        break;

    case PM_EVT_PEER_DATA_UPDATE_FAILED:
        {
            // Assert.
            APP_ERROR_CHECK(p_evt->params.peer_data_update_failed.error);
        }
        break;

    case PM_EVT_PEER_DELETE_FAILED:
        {
            // Assert.
            APP_ERROR_CHECK(p_evt->params.peer_delete_failed.error);
        }
        break;

    case PM_EVT_PEERS_DELETE_FAILED:
        {
            // Assert.
            APP_ERROR_CHECK(p_evt->params.peers_delete_failed_evt.error);
        }
        break;

    case PM_EVT_ERROR_UNEXPECTED:
        {
            // Assert.
            APP_ERROR_CHECK(p_evt->params.error_unexpected.error);
        }
        break;

    case PM_EVT_CONN_SEC_START:
    case PM_EVT_PEER_DATA_UPDATE_SUCCEEDED:
    case PM_EVT_PEER_DELETE_SUCCEEDED:
    case PM_EVT_LOCAL_DB_CACHE_APPLIED:
    case PM_EVT_SERVICE_CHANGED_IND_SENT:
    case PM_EVT_SERVICE_CHANGED_IND_CONFIRMED:
    default:
        break;
    }
}

static void hts_sim_measurement(ble_hts_meas_t *p_meas)
{
    static ble_date_time_t time_stamp = { 2012, 12, 5, 11, 50, 0 };

    uint32_t celciusX100;

    p_meas->temp_in_fahr_units = false;
    p_meas->time_stamp_present = true;
    p_meas->temp_type_present  = (TEMP_TYPE_AS_CHARACTERISTIC ? false : true);

    celciusX100 = sensorsim_measure(&m_temp_celcius_sim_state, &m_temp_celcius_sim_cfg);

    p_meas->temp_in_celcius.exponent = -2;
    p_meas->temp_in_celcius.mantissa = celciusX100;
    p_meas->temp_in_fahr.exponent    = -2;
    p_meas->temp_in_fahr.mantissa    = (32 * 100) + ((celciusX100 * 9) / 5);
    p_meas->time_stamp               = time_stamp;
    p_meas->temp_type                = BLE_HTS_TEMP_TYPE_FINGER;

    // update simulated time stamp
    time_stamp.seconds += 27;
    if (time_stamp.seconds > 59)
    {
        time_stamp.seconds -= 60;
        time_stamp.minutes++;
        if (time_stamp.minutes > 59)
        {
            time_stamp.minutes = 0;
        }
    }
}


/**@brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */
static void gap_params_init(void)
{
    ble_gap_conn_sec_mode_t sec_mode;
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    uint32_t error_code = sd_ble_gap_device_name_set(&sec_mode,
                                                     (const uint8_t *)DEVICE_NAME,
                                                     strlen(DEVICE_NAME));
    APP_ERROR_CHECK(error_code);

    error_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_GENERIC_THERMOMETER);
    APP_ERROR_CHECK(error_code);

    ble_gap_conn_params_t   gap_conn_params;
    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    error_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(error_code);
}


/**@brief Function for initializing the GATT module.
 */
static void gatt_init(void)
{
    uint32_t error_code = nrf_ble_gatt_init(&m_gatt, NULL);
    APP_ERROR_CHECK(error_code);
}


/**@brief Function for simulating and sending one Temperature Measurement.
 */
static void temperature_measurement_send(void)
{
    ble_hts_meas_t simulated_meas;

    if (!m_hts_meas_ind_conf_pending)
    {
        hts_sim_measurement(&simulated_meas);

        uint32_t const error_code = ble_hts_measurement_send(&m_hts,
                                                             &simulated_meas);

        switch (error_code)
        {
        case NRF_SUCCESS:
            // Measurement was successfully sent, wait for confirmation.
            m_hts_meas_ind_conf_pending = true;
            break;

        case NRF_ERROR_INVALID_STATE:
            // Ignore error.
            break;

        default:
            ASSERT(error_code == NRF_SUCCESS);
            break;
        }
    }
}


/**@brief Function for handling the Health Thermometer Service events.
 *
 * @details This function will be called for all Health Thermometer Service events which are passed
 *          to the application.
 *
 * @param[in] p_hts  Health Thermometer Service structure.
 * @param[in] p_evt  Event received from the Health Thermometer Service.
 */
static void on_hts_evt(ble_hts_t *p_hts, ble_hts_evt_t *p_evt)
{
    switch (p_evt->evt_type)
    {
    case BLE_HTS_EVT_INDICATION_ENABLED:
        // Indication has been enabled, send a single temperature measurement
        temperature_measurement_send();
        break;

    case BLE_HTS_EVT_INDICATION_CONFIRMED:
        m_hts_meas_ind_conf_pending = false;
        break;

    default:
        // No implementation needed.
        break;
    }
}

/**
 * Function for initializing services that will be used by the application.
 * Initialize the Health Thermometer, Battery and Device Information services.
 */
static void services_init(void)
{
    // Initialize Health Thermometer Service
    ble_hts_init_t   hts_init;
    memset(&hts_init, 0, sizeof(hts_init));

    hts_init.evt_handler                 = on_hts_evt;
    hts_init.temp_type_as_characteristic = TEMP_TYPE_AS_CHARACTERISTIC;
    hts_init.temp_type                   = BLE_HTS_TEMP_TYPE_BODY;

    // Here the sec level for the Health Thermometer Service can be changed/increased.
    BLE_GAP_CONN_SEC_MODE_SET_ENC_NO_MITM(&hts_init.hts_meas_attr_md.cccd_write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&hts_init.hts_meas_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&hts_init.hts_meas_attr_md.write_perm);

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&hts_init.hts_temp_type_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&hts_init.hts_temp_type_attr_md.write_perm);

    uint32_t error_code = ble_hts_init(&m_hts, &hts_init);
    APP_ERROR_CHECK(error_code);

    // Initialize Battery Service.
    ble_bas_init_t   bas_init;
    memset(&bas_init, 0, sizeof(bas_init));

    // Here the sec level for the Battery Service can be changed/increased.
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&bas_init.battery_level_char_attr_md.cccd_write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&bas_init.battery_level_char_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&bas_init.battery_level_char_attr_md.write_perm);

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&bas_init.battery_level_report_read_perm);

    bas_init.evt_handler          = NULL;
    bas_init.support_notification = true;
    bas_init.p_report_ref         = NULL;
    bas_init.initial_batt_level   = 100;

    error_code = ble_bas_init(&m_bas, &bas_init);
    APP_ERROR_CHECK(error_code);

    // Initialize Device Information Service.
    ble_dis_init_t   dis_init;
    memset(&dis_init, 0, sizeof(dis_init));

    /// Noridc!!! Respect the const char* - ugly hack.
    ble_srv_ascii_to_utf8(&dis_init.manufact_name_str, const_cast<char *>(MANUFACTURER_NAME));
    ble_srv_ascii_to_utf8(&dis_init.model_num_str, const_cast<char *>(MODEL_NUM));

    ble_dis_sys_id_t sys_id = {
        .manufacturer_id            = MANUFACTURER_ID,
        .organizationally_unique_id = ORG_UNIQUE_ID
    };
    dis_init.p_sys_id                 = &sys_id;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&dis_init.dis_attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&dis_init.dis_attr_md.write_perm);

    error_code = ble_dis_init(&dis_init);
    APP_ERROR_CHECK(error_code);
}


/**@brief Function for initializing the sensor simulators.
 */
static void sensor_simulator_init(void)
{
    m_battery_sim_cfg.min          = MIN_BATTERY_LEVEL;
    m_battery_sim_cfg.max          = MAX_BATTERY_LEVEL;
    m_battery_sim_cfg.incr         = BATTERY_LEVEL_INCREMENT;
    m_battery_sim_cfg.start_at_max = true;

    sensorsim_init(&m_battery_sim_state, &m_battery_sim_cfg);

    // Temperature is in celcius (it is multiplied by 100 to avoid floating point arithmetic).
    m_temp_celcius_sim_cfg.min          = MIN_CELCIUS_DEGREES;
    m_temp_celcius_sim_cfg.max          = MAX_CELCIUS_DEGRESS;
    m_temp_celcius_sim_cfg.incr         = CELCIUS_DEGREES_INCREMENT;
    m_temp_celcius_sim_cfg.start_at_max = false;

    sensorsim_init(&m_temp_celcius_sim_state, &m_temp_celcius_sim_cfg);
}

/**@brief Function for handling the Connection Parameters Module.
 *
 * @details This function will be called for all events in the Connection Parameters Module which
 *          are passed to the application.
 *          @note All this function does is to disconnect. This could have been done by simply
 *                setting the disconnect_on_fail config parameter, but instead we use the event
 *                handler mechanism to demonstrate its use.
 *
 * @param[in] p_evt  Event received from the Connection Parameters Module.
 */
static void on_conn_params_evt(ble_conn_params_evt_t *p_evt)
{
    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        uint32_t error_code = sd_ble_gap_disconnect(m_conn_handle,
                                                    BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(error_code);
    }
}

static void conn_params_error_handler(uint32_t nrf_error)
{
    logger::instance().error("conn params error: %u", nrf_error);
    ASSERT(nrf_error == NRF_SUCCESS);
}

/**@brief Function for initializing the Connection Parameters module.
 */
static void conn_params_init(void)
{
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = on_conn_params_evt;
    cp_init.error_handler                  = conn_params_error_handler;

    uint32_t const error_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(error_code);
}

/**@brief Function for handling advertising events.
 *
 * @details This function will be called for advertising events which are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
    switch (ble_adv_evt)
    {
    case BLE_ADV_EVT_FAST:
        logger::instance().info("Fast advertising.");
        break;

    case BLE_ADV_EVT_IDLE:
        break;

    default:
        break;
    }
}


/**@brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 * @param[in]   p_context   Unused.
 */
static void ble_evt_handler(ble_evt_t const *p_ble_evt, void *p_context)
{
    switch (p_ble_evt->header.evt_id)
    {
    case BLE_GAP_EVT_CONNECTED:
        logger::instance().info("Connected.");
        m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
        break;

    case BLE_GAP_EVT_DISCONNECTED:
        logger::instance().info("Disconnected.");
        m_conn_handle               = BLE_CONN_HANDLE_INVALID;
        m_hts_meas_ind_conf_pending = false;
        break;

    case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
        {
            logger::instance().debug("PHY update request.");
            ble_gap_phys_t const phys =
            {
                .tx_phys = BLE_GAP_PHY_AUTO,
                .rx_phys = BLE_GAP_PHY_AUTO,
            };

            uint32_t const error_code = sd_ble_gap_phy_update(
                p_ble_evt->evt.gap_evt.conn_handle,
                &phys);

            APP_ERROR_CHECK(error_code);
        }
        break;

    case BLE_GATTC_EVT_TIMEOUT:
        {
            // Disconnect on GATT Client timeout event.
            logger::instance().debug("GATT Client Timeout.");

            uint32_t const error_code = sd_ble_gap_disconnect(
                p_ble_evt->evt.gattc_evt.conn_handle,
                BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);

            APP_ERROR_CHECK(error_code);
        }
        break;

    case BLE_GATTS_EVT_TIMEOUT:
        {
            // Disconnect on GATT Server timeout event.
            logger::instance().debug("GATT Server Timeout.");

            uint32_t const error_code = sd_ble_gap_disconnect(
                p_ble_evt->evt.gatts_evt.conn_handle,
                BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);

            APP_ERROR_CHECK(error_code);
        }
        break;

    case BLE_EVT_USER_MEM_REQUEST:
        {
            uint32_t const error_code = sd_ble_user_mem_reply(m_conn_handle, NULL);
            APP_ERROR_CHECK(error_code);
        }
        break;

    case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
        {
            ble_gatts_evt_rw_authorize_request_t  req;
            ble_gatts_rw_authorize_reply_params_t auth_reply;

            req = p_ble_evt->evt.gatts_evt.params.authorize_request;

            if (req.type != BLE_GATTS_AUTHORIZE_TYPE_INVALID)
            {
                if ((req.request.write.op == BLE_GATTS_OP_PREP_WRITE_REQ) ||
                    (req.request.write.op == BLE_GATTS_OP_EXEC_WRITE_REQ_NOW) ||
                    (req.request.write.op == BLE_GATTS_OP_EXEC_WRITE_REQ_CANCEL))
                {
                    if (req.type == BLE_GATTS_AUTHORIZE_TYPE_WRITE)
                    {
                        auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_WRITE;
                    }
                    else
                    {
                        auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_READ;
                    }
                    auth_reply.params.write.gatt_status = APP_FEATURE_NOT_SUPPORTED;

                    uint32_t const error_code =
                        sd_ble_gatts_rw_authorize_reply(
                            p_ble_evt->evt.gatts_evt.conn_handle, &auth_reply);

                    APP_ERROR_CHECK(error_code);
                }
            }
        }
        break;

    default:
        // No implementation needed.
        break;
    }
}


/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void)
{
    uint32_t error_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(error_code);

    // Configure the BLE stack using the default settings.
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    error_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(error_code);

    // Enable BLE stack.
    error_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(error_code);

    // Register a handler for BLE events.
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);
}

/**@brief Function for the Peer Manager initialization.
 */
static void peer_manager_init(void)
{
    uint32_t error_code = pm_init();
    APP_ERROR_CHECK(error_code);

    ble_gap_sec_params_t sec_param;
    memset(&sec_param, 0, sizeof(ble_gap_sec_params_t));

    // Security parameters to be used for all security procedures.
    sec_param.bond           = SEC_PARAM_BOND;
    sec_param.mitm           = SEC_PARAM_MITM;
    sec_param.lesc           = SEC_PARAM_LESC;
    sec_param.keypress       = SEC_PARAM_KEYPRESS;
    sec_param.io_caps        = SEC_PARAM_IO_CAPABILITIES;
    sec_param.oob            = SEC_PARAM_OOB;
    sec_param.min_key_size   = SEC_PARAM_MIN_KEY_SIZE;
    sec_param.max_key_size   = SEC_PARAM_MAX_KEY_SIZE;
    sec_param.kdist_own.enc  = 1;
    sec_param.kdist_own.id   = 1;
    sec_param.kdist_peer.enc = 1;
    sec_param.kdist_peer.id  = 1;

    error_code = pm_sec_params_set(&sec_param);
    APP_ERROR_CHECK(error_code);

    error_code = pm_register(pm_evt_handler);
    APP_ERROR_CHECK(error_code);
}


/**@brief Clear bond information from persistent storage.
 */
static void delete_bonds(void)
{
    logger::instance().info("Erase bonds!");

    uint32_t const error_code = pm_peers_delete();
    APP_ERROR_CHECK(error_code);
}


/**@brief Function for initializing the Advertising functionality.
 *
 * @details Encodes the required advertising data and passes it to the stack.
 *          Also builds a structure to be passed to the stack when starting advertising.
 */
static void advertising_init(void)
{
    ble_advertising_init_t init;

    memset(&init, 0, sizeof(init));

    init.advdata.name_type               = BLE_ADVDATA_FULL_NAME;
    init.advdata.include_appearance      = true;
    init.advdata.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    init.advdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    init.advdata.uuids_complete.p_uuids  = m_adv_uuids;

    init.config.ble_adv_fast_enabled  = true;
    init.config.ble_adv_fast_interval = APP_ADV_INTERVAL;
    init.config.ble_adv_fast_timeout  = APP_ADV_TIMEOUT_IN_SECONDS;

    init.evt_handler = on_adv_evt;

    uint32_t const error_code = ble_advertising_init(&m_advertising, &init);
    APP_ERROR_CHECK(error_code);

    ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);
}

static void advertising_start(bool erase_bonds)
{
    if (erase_bonds == true)
    {
        delete_bonds();
        // Advertising is started by PM_EVT_PEERS_DELETE_SUCCEEDED event.
    }
    else
    {
        uint32_t const error_code = ble_advertising_start(&m_advertising,
                                                          BLE_ADV_MODE_FAST);
        APP_ERROR_CHECK(error_code);
    }
}

class battery_update_timer_observer : public rtc_observer
{
public:
    battery_update_timer_observer(rtc_observable<> &rtc) :
        rtc_observer(expiration_type::continuous, rtc.msec_to_ticks(2000u)),
        observable_(rtc)
    {}

    void enable()  { this->observable_.attach(*this); }
    void disable() { this->observable_.detach(*this); }

    void expiration_notify() override
    {
        uint8_t const battery_level = (uint8_t)sensorsim_measure(&m_battery_sim_state,
                                                                 &m_battery_sim_cfg);

        uint32_t const error_code = ble_bas_battery_level_update(&m_bas, battery_level);
        if ((error_code != NRF_SUCCESS) &&
            (error_code != NRF_ERROR_INVALID_STATE) &&
            (error_code != NRF_ERROR_RESOURCES) &&
            (error_code != BLE_ERROR_GATTS_SYS_ATTR_MISSING)
           )
        {
            ASSERT(error_code == NRF_SUCCESS);
        }
    }

private:
    rtc_observable<> &observable_;
};


static segger_rtt_output_stream rtt_os;

static rtc_observable<> rtc_1(1u, 32u);

/**@brief Function for application main entry.
 */
int main(void)
{
    lfclk_enable(LFCLK_SOURCE_XO);
    app_timer_init(rtc_1);
    rtc_1.start();

    leds_board_init();
    buttons_board_init();

    logger &logger = logger::instance();
    logger.set_rtc(rtc_1);
    logger.set_level(logger::level::debug);
    logger.set_output_stream(rtt_os);

    logger.info("--- BLE peripheral: thermometer ---");

    ble_stack_init();
    gap_params_init();
    gatt_init();
    advertising_init();
    services_init();
    sensor_simulator_init();
    conn_params_init();
    peer_manager_init();

    // Start execution.
    battery_update_timer_observer battery_update(rtc_1);
    battery_update.enable();

    bool const erase_bonds = false;
    advertising_start(erase_bonds);

    // Enter main loop.
    for (;;)
    {
        rtt_os.flush();
        if (rtt_os.write_pending() == 0)
        {
            __WFE();
        }
    }
}


/**
 * @}
 */
