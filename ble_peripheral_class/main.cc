/**
 * @file ble_peripheral_class/main.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "ble.h"
#include "ble_conn_params.h"
#include "ble_conn_state.h"
#include "fds.h"
#include "nrf_ble_gatt.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "peer_manager.h"

#include "app_timer.h"
#include "clocks.h"
#include "leds.h"
#include "buttons.h"
#include "logger.h"
#include "segger_rtt_output_stream.h"
#include "rtc_observer.h"
#include "project_assert.h"

#include "ble/nordic_softdevice_init.h"
#include "ble/advertising.h"

#define APP_FEATURE_NOT_SUPPORTED       BLE_GATT_STATUS_ATTERR_APP_BEGIN + 2    /**< Reply when unsupported features are requested. */

#define APP_BLE_OBSERVER_PRIO           3                                       /**< Application's BLE observer priority. You shouldn't need to modify this value. */

NRF_BLE_GATT_DEF(m_gatt);                                                       /**< GATT module instance. */

static uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID;                        /**< Handle of the current connection. */

/* YOUR_JOB: Declare all services structure your application is using
 *  BLE_XYZ_DEF(m_xyz);
 */

/**
 * Callback function for asserts in the SoftDevice.
 * This function will be called in case of an assert in the SoftDevice.
 *
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param line_num   Line number of the failing ASSERT call.
 * @param file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t *u_file_name)
{
    /// @todo What else can be done here?
    char const *file_name = reinterpret_cast<char const*>(u_file_name);
    assertion_failed(file_name, "softdevice", line_num, "failure");
}

/**@brief Function for handling Peer Manager events.
 *
 * @param[in] p_evt  Peer Manager event.
 */
static void pm_evt_handler(pm_evt_t const * p_evt)
{
    switch (p_evt->evt_id)
    {
        case PM_EVT_BONDED_PEER_CONNECTED:
        {
            logger &logger = logger::instance();
            logger.info("Connected to a previously bonded device.");
        } break;

        case PM_EVT_CONN_SEC_SUCCEEDED:
        {
            logger &logger = logger::instance();
            logger.info("Connection secured: role: %d, conn_handle: 0x%x, procedure: %d.",
                        ble_conn_state_role(p_evt->conn_handle),
                        p_evt->conn_handle,
                        p_evt->params.conn_sec_succeeded.procedure);
        } break;

        case PM_EVT_CONN_SEC_FAILED:
        {
            /* Often, when securing fails, it shouldn't be restarted, for security reasons.
             * Other times, it can be restarted directly.
             * Sometimes it can be restarted, but only after changing some Security Parameters.
             * Sometimes, it cannot be restarted until the link is disconnected and reconnected.
             * Sometimes it is impossible, to secure the link, or the peer device does not support it.
             * How to handle this error is highly application dependent. */
        } break;

        case PM_EVT_CONN_SEC_CONFIG_REQ:
        {
            // Reject pairing request from an already bonded peer.
            pm_conn_sec_config_t conn_sec_config = {.allow_repairing = false};
            pm_conn_sec_config_reply(p_evt->conn_handle, &conn_sec_config);
        } break;

        case PM_EVT_STORAGE_FULL:
        {
            // Run garbage collection on the flash.
            ret_code_t const error_code = fds_gc();
            if (error_code == FDS_ERR_BUSY || error_code == FDS_ERR_NO_SPACE_IN_QUEUES)
            {
                // Retry.
            }
            else
            {
                ASSERT(error_code == NRF_SUCCESS);
            }
        } break;

        case PM_EVT_PEERS_DELETE_SUCCEEDED:
        {
            /// @todo restart advertising.
//            advertising_start(false);
        } break;

        case PM_EVT_LOCAL_DB_CACHE_APPLY_FAILED:
        {
            // The local database has likely changed, send service changed indications.
            pm_local_database_has_changed();
        } break;

        case PM_EVT_PEER_DATA_UPDATE_FAILED:
        {
            ASSERT(p_evt->params.peer_data_update_failed.error == NRF_SUCCESS);
        } break;

        case PM_EVT_PEER_DELETE_FAILED:
        {
            ASSERT(p_evt->params.peer_delete_failed.error == NRF_SUCCESS);
        } break;

        case PM_EVT_PEERS_DELETE_FAILED:
        {
            ASSERT(p_evt->params.peers_delete_failed_evt.error == NRF_SUCCESS);
        } break;

        case PM_EVT_ERROR_UNEXPECTED:
        {
            ASSERT(p_evt->params.error_unexpected.error == NRF_SUCCESS);
        } break;

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

/**
 * Convert from milliseconds to BLE conection interval units of 1.250 msec.
 *
 * @param interval_msec A time interval expressed in milliseconds.
 *
 * @return uint16_t The number of 1.25 msec ticks used by the BLE connection
 * interval tick count.
 */
static uint16_t connection_interval_msec(uint32_t interval_msec)
{
    interval_msec *= 1000u;
    interval_msec /= 1250u;
    return static_cast<uint16_t>(interval_msec);
}

/**
 * Convert from milliseconds to BLE supervisory timeout tick count.
 *
 * @param interval_msec The supervisory timeout expressed in milliseconds.
 *
 * @return uint16_t The number of 10 msec ticks used by the BLE
 * supervisory timeout tick count.
 */
static uint16_t supvisory_timeout_msec(uint32_t interval_msec)
{
    interval_msec /= 10u;
    return static_cast<uint16_t>(interval_msec);
}

/**
 * Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile)
 * parameters of the device including the device name, appearance,
 * and the preferred connection parameters.
 */
static void gap_params_init(void)
{
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    char const *DEVICE_NAME = "ble_class";
    ret_code_t error_code = sd_ble_gap_device_name_set(&sec_mode,
                                                     (const uint8_t *)DEVICE_NAME,
                                                     strlen(DEVICE_NAME));
    ASSERT(error_code == NRF_SUCCESS);

    /* YOUR_JOB: Use an appearance value matching the application's use case.
       error_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_);
     */

    memset(&gap_conn_params, 0, sizeof(gap_conn_params));

    gap_conn_params.min_conn_interval = connection_interval_msec(100);
    gap_conn_params.max_conn_interval = connection_interval_msec(200);
    gap_conn_params.slave_latency     = 0;
    gap_conn_params.conn_sup_timeout  = supvisory_timeout_msec(4000);

    error_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    ASSERT(error_code == NRF_SUCCESS);
}

/**@brief Function for initializing the GATT module.
 */
static void gatt_init(void)
{
    ret_code_t error_code = nrf_ble_gatt_init(&m_gatt, NULL);
    ASSERT(error_code == NRF_SUCCESS);
}

/**@brief Function for handling the YYY Service events.
 * YOUR_JOB implement a service handler function depending on the event the service you are using can generate
 *
 * @details This function will be called for all YY Service events which are passed to
 *          the application.
 *
 * @param[in]   p_yy_service   YY Service structure.
 * @param[in]   p_evt          Event received from the YY Service.
 *
 *
static void on_yys_evt(ble_yy_service_t     * p_yy_service,
                       ble_yy_service_evt_t * p_evt)
{
    switch (p_evt->evt_type)
    {
        case BLE_YY_NAME_EVT_WRITE:
            APPL_LOG("[APPL]: charact written with value %s. ", p_evt->params.char_xx.value.p_str);
            break;

        default:
            // No implementation needed.
            break;
    }
}
*/

/** Initialize application services. */
static void services_init(void)
{
    /* YOUR_JOB: Add code to initialize the services used by the application.
       ret_code_t                         error_code;
       ble_xxs_init_t                     xxs_init;
       ble_yys_init_t                     yys_init;

       // Initialize XXX Service.
       memset(&xxs_init, 0, sizeof(xxs_init));

       xxs_init.evt_handler                = NULL;
       xxs_init.is_xxx_notify_supported    = true;
       xxs_init.ble_xx_initial_value.level = 100;

       error_code = ble_bas_init(&m_xxs, &xxs_init);

       // Initialize YYY Service.
       memset(&yys_init, 0, sizeof(yys_init));
       yys_init.evt_handler                  = on_yys_evt;
       yys_init.ble_yy_initial_value.counter = 0;

       error_code = ble_yy_service_init(&yys_init, &yy_init);
     */
}


/**
 * @todo This, along with the entire ble_conn_params.c functionality needs to
 * be refactored into a class which handles BLE connection events.
 * @see nordic::ble_gap_event_observer, as it will be the interface which
 * gets connection notificiations.
 *
 * This function is called for all events in the Connection Parameters Module
 * which are passed to the application.
 * @note All this function does is to disconnect.
 * This could have been done by simply setting the disconnect_on_fail config
 * parameter, but instead we use the event handler mechanism to demonstrate its use.
 *
 * @param p_evt Event received from the Connection Parameters Module.
 */
static void on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        ret_code_t error_code = sd_ble_gap_disconnect(m_conn_handle,
                                                    BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        ASSERT(error_code == NRF_SUCCESS);
    }
}


/**
 * Function for handling a Connection Parameters error.
 *
 * @param nrf_error  Error code containing information about what went wrong.
 */
static void conn_params_error_handler(uint32_t nrf_error)
{
    ASSERT(nrf_error == NRF_SUCCESS);
}

/**
 * @see comments in on_conn_params_evt() for how this is going to be refactored.
 */
static void conn_params_init(rtc &rtc)
{
    ble_conn_params_init_t cp_init;
    memset(&cp_init, 0, sizeof(cp_init));

    // Time from initiating event (connect or start of notification)
    // to first time sd_ble_gap_conn_param_update is called (5 seconds).
    uint32_t const first_conn_params_update_delay = rtc.msec_to_ticks(5000u);

    // Time between each call to sd_ble_gap_conn_param_update
    // after the first call (30 seconds).
    uint32_t const next_conn_params_update_delay = rtc.msec_to_ticks(30000u);

    // Number of attempts before giving up the connection parameter negotiation.
    uint8_t const max_conn_params_update_count = 3u;

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = first_conn_params_update_delay;
    cp_init.next_conn_params_update_delay  = next_conn_params_update_delay;
    cp_init.max_conn_params_update_count   = max_conn_params_update_count;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = on_conn_params_evt;
    cp_init.error_handler                  = conn_params_error_handler;

    ret_code_t  error_code = ble_conn_params_init(&cp_init);
    ASSERT(error_code == NRF_SUCCESS);
}

/**@brief Function for handling BLE events.
 *
 * @param  p_ble_evt   Bluetooth stack event.
 * @param  p_context   Unused.
 */
/// @todo This gets removed and replaced by an instance of
/// nordic::ble_gap_event_observer.
static void ble_evt_handler(ble_evt_t const *ble_event, void *context)
{
    ret_code_t error_code = NRF_SUCCESS;
    logger &logger = logger::instance();

    switch (ble_event->header.evt_id)
    {
        case BLE_GAP_EVT_DISCONNECTED:
            logger.info("Disconnected.");
            break;

        case BLE_GAP_EVT_CONNECTED:
            m_conn_handle = ble_event->evt.gap_evt.conn_handle;
            logger.info("Connected, handle: 0x%04x", m_conn_handle);
            break;

#ifndef S140
        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
        {
            logger.debug("PHY update request.");
            ble_gap_phys_t const phys =
            {
                .tx_phys = BLE_GAP_PHY_AUTO,
                .rx_phys = BLE_GAP_PHY_AUTO,
            };
            error_code = sd_ble_gap_phy_update(ble_event->evt.gap_evt.conn_handle, &phys);
            ASSERT(error_code == NRF_SUCCESS);
        } break;
#endif

        case BLE_GATTC_EVT_TIMEOUT:
            // Disconnect on GATT Client timeout event.
            logger.debug("GATT Client Timeout.");
            error_code = sd_ble_gap_disconnect(ble_event->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            ASSERT(error_code == NRF_SUCCESS);
            break;

        case BLE_GATTS_EVT_TIMEOUT:
            // Disconnect on GATT Server timeout event.
            logger.debug("GATT Server Timeout.");
            error_code = sd_ble_gap_disconnect(ble_event->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            ASSERT(error_code == NRF_SUCCESS);
            break;

        case BLE_EVT_USER_MEM_REQUEST:
            error_code = sd_ble_user_mem_reply(ble_event->evt.gattc_evt.conn_handle, NULL);
            ASSERT(error_code == NRF_SUCCESS);
            break;

        case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
        {
            ble_gatts_evt_rw_authorize_request_t  req;
            ble_gatts_rw_authorize_reply_params_t auth_reply;

            req = ble_event->evt.gatts_evt.params.authorize_request;

            if (req.type != BLE_GATTS_AUTHORIZE_TYPE_INVALID)
            {
                if ((req.request.write.op == BLE_GATTS_OP_PREP_WRITE_REQ)     ||
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
                    error_code = sd_ble_gatts_rw_authorize_reply(ble_event->evt.gatts_evt.conn_handle,
                                                               &auth_reply);
                    ASSERT(error_code == NRF_SUCCESS);
                }
            }
        } break; // BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST

        default:
            // No implementation needed.
            break;
    }
}

static void peer_manager_init(void)
{
    ret_code_t error_code = pm_init();
    ASSERT(error_code == NRF_SUCCESS);

    ble_gap_sec_params_t sec_param;
    memset(&sec_param, 0, sizeof(ble_gap_sec_params_t));

    // Security parameters to be used for all security procedures.
    sec_param.bond           = true;            // bonding enable
    sec_param.mitm           = false;           // man-in-the-middle protection
    sec_param.lesc           = false;           // secure connection required
    sec_param.keypress       = false;           // enable keypress notifications
    sec_param.io_caps        = BLE_GAP_IO_CAPS_NONE;
    sec_param.oob            = false;           // out of band authentication
    sec_param.min_key_size   =  7u;
    sec_param.max_key_size   = 16u;
    sec_param.kdist_own.enc  = true;            // long term key and master id
    sec_param.kdist_own.id   = true;            // identity resolving key/address
    sec_param.kdist_peer.enc = true;
    sec_param.kdist_peer.id  = true;

    error_code = pm_sec_params_set(&sec_param);
    ASSERT(error_code == NRF_SUCCESS);

    error_code = pm_register(pm_evt_handler);
    ASSERT(error_code == NRF_SUCCESS);
}

/**
 * Initialize the SoftDevice and the BLE event interrupt.
 */
static void ble_stack_init(void)
{
    // Check to see if all other softdevice clients are ready to start.
    {
        ret_code_t error_code = nrf_sdh_enable_request();
        ASSERT(error_code == NRF_SUCCESS);
    }

    /* __data_start__ marks the beginning of the data section within the linker
     * description script. See nrf5x_common.ld. The RAM region before the data
     * section is reserved for use by the softdevice.. If any other sections
     * are placed in front of the __data_init__ marker then the marker location
     * needs to change as well.
     */
    extern uint32_t __data_start__;
    uintptr_t const ram_base_address        = reinterpret_cast<uintptr_t>(&__data_start__);
    uint8_t   const total_link_count        =    1u;
    uint8_t   const peripheral_link_count   =    1u;
    uint16_t  const mtu_size                =   23u;
    uint8_t   const gatt_uuid_count         =    4u;    /// @todo
    uint32_t  const gatt_table_size         = 2048u;    /// @todo
    bool      const service_changed         = false;

    uint32_t const sd_init_result = nordic::softdevice_init(
        ram_base_address,
        total_link_count,
        peripheral_link_count,
        mtu_size,
        gatt_uuid_count,
        gatt_table_size,
        service_changed);

    ASSERT(sd_init_result == NRF_SUCCESS);

    uint32_t sd_base_address  = ram_base_address;
    ret_code_t const ret_code = sd_ble_enable(&sd_base_address);

    logger& logger = logger::instance();
    logger.info("RAM starts at 0x%08x, minimum required: 0x%08x, %s",
                ram_base_address, sd_base_address,
                (ram_base_address >= sd_base_address)? "OK" : "FAIL");

    ASSERT(ret_code == NRF_SUCCESS);

    // Register a handler for BLE events.
    /// @todo This gets removed and replaced by an instance of
    /// nordic::ble_gap_event_observer.
    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);
}

static segger_rtt_output_stream rtt_os;

static rtc_observable<> rtc_1(1u, 32u);

#include "ble/gap_types.h"
#include "ble/tlv_encode.h"
#include "nordic/device_address.h"

static uint16_t const advertising_interval = ble::advertising::interval_msec(100u);
static ble::advertising advertising(advertising_interval);

static void set_advertising_data(ble::advertising_data_t &data)
{
    uint16_t const uuid16_list[] = {
        BLE_UUID_DEVICE_INFORMATION_SERVICE
    };

    size_t const uuid16_count = sizeof(uuid16_list) / sizeof(uuid16_list[0]);

    ble::tlv_encode(data, ble::gap_type::flags,            ble::le_general_discovery);
    ble::tlv_encode(data, ble::gap_type::local_name_short, "periph_class");
    ble::tlv_encode_address(data, false, nordic::get_device_address());
    ble::tlv_encode(data, ble::gap_type::uuid_service_16_complete, uuid16_list, uuid16_count);
}

int main(void)
{
    lfclk_enable(LFCLK_SOURCE_XO);
    app_timer_init(rtc_1);
    rtc_1.start();

    leds_board_init();
    buttons_board_init();

    logger& logger = logger::instance();
    logger.set_rtc(rtc_1);
    logger.set_level(logger::level::debug);
    logger.set_output_stream(rtt_os);

    logger.info("--- BLE peripheral template ---");

    ble_stack_init();
    gap_params_init();
    gatt_init();
    services_init();
    conn_params_init(rtc_1);
    peer_manager_init();

    set_advertising_data(advertising.advertising_data);
    advertising.start();

    for (;;)
    {
        rtt_os.flush();
        if (rtt_os.write_pending() == 0)
        {
            __WFE();
        }
    }
}

