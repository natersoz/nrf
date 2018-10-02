/**
 * @file ble/nordic_ble_stack.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "ble/nordic_ble_stack.h"
#include "project_assert.h"
#include "nordic_error.h"
#include "logger.h"

#include "ble_conn_params.h"
#include "nrf_sdh.h"
#include "nrf_sdm.h"
#include "ble_hci.h"

#include <cstdint>
#include <cstring>

/* __data_start__ marks the beginning of the data section within the linker
 * description script. See nrf5x_common.ld. The RAM region before the data
 * section is reserved for use by the softdevice. If any other sections
 * are placed in front of the __data_init__ marker then the marker location
 * needs to change as well.
 */
extern uint32_t __data_start__;
static uintptr_t const ram_base_address = reinterpret_cast<uintptr_t>(&__data_start__);

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

/**
 * Initialize the SoftDevice and the BLE event interrupt.
 * @note When debugging with remote gdb (the normal case), if the image is updated
 * via gdb 'load' and the get 'monitor reset' is not called, this function will fail
 * since the state of the BLE stack is still running.
 * @todo nrf_sdh_enable_request() calls nrf_sdh_enable_request() which uses
 * #define NRF_SDH_CLOCK_LF_SRC in sdk_config.h; NRF_CLOCK_LF_SRC_XTAL (1).
 * MAke this parameterized within this class.
 */
std::errc nordic::ble_stack::init(void)
{
    uint32_t error_code = NRF_SUCCESS;

    // Notify all softdevice registered observers that the softdevice is going
    // to be initialized. If all observers do not acknowledge that they are
    // ready, nrf_sdh_enable_request() will return an error.
    if (error_code == NRF_SUCCESS)
    {
        // If nrf_sdh_enable_request() is successful then sd_softdevice_enable()
        // has been called and softdevice interrupts are enabled.
        error_code = nrf_sdh_enable_request();
        ASSERT(error_code == NRF_SUCCESS);
    }

    if (error_code == NRF_SUCCESS)
    {
        uint8_t   const total_link_count        =    1u;
        uint8_t   const peripheral_link_count   =    1u;
        uint16_t  const mtu_size                =   23u;
        uint8_t   const gatt_uuid_count         =   16u;    /// @todo
        uint32_t  const gatt_table_size         = 2048u;    /// @todo
        bool      const service_changed         = false;

        error_code = this->set_configuration(
            ram_base_address,
            total_link_count,
            peripheral_link_count,
            mtu_size,
            gatt_uuid_count,
            gatt_table_size,
            service_changed);

        ASSERT(error_code == NRF_SUCCESS);
    }

    return nordic_to_system_error(error_code);
}

std::errc nordic::ble_stack::enable()
{
    uint32_t sd_base_address = ram_base_address;
    uint32_t error_code = sd_ble_enable(&sd_base_address);

    logger& logger = logger::instance();
    logger.info("RAM starts at 0x%08x, minimum required: 0x%08x, %s",
                ram_base_address, sd_base_address,
                (ram_base_address >= sd_base_address)? "OK" : "FAIL");

    ASSERT(error_code == NRF_SUCCESS);
    return nordic_to_system_error(error_code);

    /// @todo register the BLE GAP observer here.
    /// gatt_init() can be removed.
    //gatt_init();
}

std::errc nordic::ble_stack::disable()
{
    uint32_t error_code = sd_softdevice_disable();
    return nordic_to_system_error(error_code);
}

bool nordic::ble_stack::is_enabled() const
{
    uint8_t is_enabled = 0u;
    sd_softdevice_is_enabled(&is_enabled);      // Always return NRF_SUCCESS.
    return bool(is_enabled);
}

uint32_t nordic::ble_stack::set_configuration(uintptr_t  ram_base_address,
                                              uint8_t    total_link_count,
                                              uint8_t    peripheral_link_count,
                                              uint16_t   mtu_size,
                                              uint8_t    gatt_uuid_count,
                                              uint32_t   gatt_table_size,
                                              bool       service_changed)
{
    uint32_t ret_code = NRF_SUCCESS;
    logger& logger = logger::instance();

    // If any of the calls to sd_ble_cfg_set() fail, log the error but carry on
    // so that wrong RAM settings can be caught by nrf_sdh_ble_enable()
    // and a meaningful error  message will be printed to the user suggesting
    // the correct value.
    ble_cfg_t ble_cfg;
    memset(&ble_cfg, 0, sizeof(ble_cfg));

    // Note: any of the configuration settings which are part of the
    // struct ble_conn_cfg_t member conn_cfg require the conn_cfg_tag.
    // For other settings, this tag must not be set.
    ble_cfg.conn_cfg.conn_cfg_tag = this->connection_configuration_tag_;

    /**
     * The number of concurrent connections the application can create with this configuration.
     * The default and minimum value is @ref BLE_GAP_CONN_COUNT_DEFAULT.
     */
    ble_cfg.conn_cfg.params.gap_conn_cfg.conn_count = total_link_count;

    /**
     * The time set aside for this connection on every connection interval
     * in 1.25 ms units.
     *
     * The event length and the connection interval are the primary parameters
     * for setting the throughput of a connection.
     * See the SoftDevice Specification for details on throughput.
     *
     * The default value is @ref BLE_GAP_EVENT_LENGTH_DEFAULT.
     * The minimum value is @ref BLE_GAP_EVENT_LENGTH_MIN.
     */
    ble_cfg.conn_cfg.params.gap_conn_cfg.event_length = BLE_GAP_EVENT_LENGTH_DEFAULT;

    {
        uint32_t error_code = sd_ble_cfg_set(BLE_CONN_CFG_GAP, &ble_cfg, ram_base_address);
        if (error_code != NRF_SUCCESS)
        {
            logger.error("error: sd_ble_cfg_set(BLE_CONN_CFG_GAP): failed: %u", error_code);
            ret_code = error_code;
        }
    }

    // Configure the connection roles.
    memset(&ble_cfg, 0, sizeof(ble_cfg));
    ble_cfg.gap_cfg.role_count_cfg.periph_role_count = peripheral_link_count;

    {
        uint32_t error_code = sd_ble_cfg_set(BLE_GAP_CFG_ROLE_COUNT, &ble_cfg, ram_base_address);
        if (error_code != NRF_SUCCESS)
        {
            logger.error("error: sd_ble_cfg_set(BLE_GAP_CFG_ROLE_COUNT): failed: %u", error_code);
            ret_code = (ret_code == NRF_SUCCESS)? error_code : ret_code;
        }
    }

    // Configure the maximum ATT MTU.
    memset(&ble_cfg, 0, sizeof(ble_cfg));
    ble_cfg.conn_cfg.conn_cfg_tag = this->connection_configuration_tag_;
    ble_cfg.conn_cfg.params.gatt_conn_cfg.att_mtu = mtu_size;

    {
        uint32_t error_code = sd_ble_cfg_set(BLE_CONN_CFG_GATT, &ble_cfg, ram_base_address);
        if (error_code != NRF_SUCCESS)
        {
            logger.error("error: sd_ble_cfg_set(BLE_CONN_CFG_GATT): failed: %u", error_code);
            ret_code = (ret_code == NRF_SUCCESS)? error_code : ret_code;
        }
    }

    // Configure number of custom UUIDS.
    memset(&ble_cfg, 0, sizeof(ble_cfg));
    ble_cfg.common_cfg.vs_uuid_cfg.vs_uuid_count = gatt_uuid_count;

    {
        uint32_t error_code = sd_ble_cfg_set(BLE_COMMON_CFG_VS_UUID, &ble_cfg, ram_base_address);
        if (error_code != NRF_SUCCESS)
        {
            logger.error("error: sd_ble_cfg_set(BLE_COMMON_CFG_VS_UUID): failed: %u", error_code);
            ret_code = (ret_code == NRF_SUCCESS)? error_code : ret_code;
        }
    }

    // Configure the GATTS attribute table.
    memset(&ble_cfg, 0, sizeof(ble_cfg));
    ble_cfg.gatts_cfg.attr_tab_size.attr_tab_size = gatt_table_size;

    {
        /// @todo What do we do here? I'd rather manage the attribute table/get/set myself ...
        uint32_t error_code = sd_ble_cfg_set(BLE_GATTS_CFG_ATTR_TAB_SIZE,
                                             &ble_cfg,
                                             ram_base_address);
        if (error_code != NRF_SUCCESS)
        {
            logger.error("error: sd_ble_cfg_set(BLE_GATTS_CFG_ATTR_TAB_SIZE): failed: %u", error_code);
            ret_code = (ret_code == NRF_SUCCESS)? error_code : ret_code;
        }
    }

    // Configure Service Changed characteristic.
    memset(&ble_cfg, 0x00, sizeof(ble_cfg));
    ble_cfg.gatts_cfg.service_changed.service_changed = service_changed;

    {
        uint32_t error_code = sd_ble_cfg_set(BLE_GATTS_CFG_SERVICE_CHANGED, &ble_cfg, ram_base_address);
        if (error_code != NRF_SUCCESS)
        {
            logger.error("error: sd_ble_cfg_set(BLE_GATTS_CFG_SERVICE_CHANGED): failed: %u", error_code);
            ret_code = (ret_code == NRF_SUCCESS)? error_code : ret_code;
        }
    }

    return ret_code;
}
