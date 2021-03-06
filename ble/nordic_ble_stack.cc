/**
 * @file ble/nordic_ble_stack.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "ble/nordic_ble_stack.h"
#include "ble/att.h"
#include "project_assert.h"
#include "std_error.h"
#include "nordic_error.h"
#include "logger.h"

#include <ble.h>
#include <nrf_sdh.h>
#include <nrf_sdm.h>
#include <ble_hci.h>

#include <cstdint>
#include <cstring>

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
 * @details @see nrf5x_common.ld.
 * __data_start__ marks the beginning of the 'data' section within the linker
 * description file. The RAM region before the 'data' section is reserved for
 * use by the softdevice.
 *
 * @note If any other sections are placed in front of the __data_init__ marker
 * then the marker location needs to change as well.
 */
extern uint32_t __data_start__;

namespace nordic
{

uintptr_t const ble_stack::ram_base_address = reinterpret_cast<uintptr_t>(&__data_start__);

ble_stack::ble_stack(uint8_t conn_cfg_tag):
    ble::stack(),
    connection_configuration_tag(conn_cfg_tag)
{
}

/**
 * Initialize the SoftDevice and the BLE event interrupt.
 *
 * @note When debugging with remote gdb (the normal case), if the image is
 * updated via gdb 'load' and the get 'monitor reset' is not called,
 * this function will fail since the state of the BLE stack is still running.
 *
 * @todo nrf_sdh_enable_request() calls nrf_sdh_enable_request() which uses
 * #define NRF_SDH_CLOCK_LF_SRC in sdk_config.h; NRF_CLOCK_LF_SRC_XTAL (1).
 * These should be parameterized within this class.
 */
std::errc ble_stack::init(unsigned int peripheral_count,
                          unsigned int central_count)
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

    // Set default settings. The user can override these by calling the same
    // functions with their application specific settings.
    // Note: the BLE MYU is set to maximum by default to accomodate client
    // requests that are in the valid range. Otherwise we cannot fill the request.
    std::errc error_link_count      = set_link_count(peripheral_count, central_count);
    std::errc error_custom_uuid     = set_gatt_custom_uuid_count(8u);
    std::errc error_table_size      = set_gatt_table_size(2048u);
    std::errc error_service_changed = set_service_changed_characteristic(false);
    std::errc error_mtu_max_size    = set_mtu_max_size(ble::att::mtu_length_maximum);

    if (is_failure(error_link_count))       { return error_link_count; }
    if (is_failure(error_custom_uuid))      { return error_custom_uuid; }
    if (is_failure(error_table_size))       { return error_table_size; }
    if (is_failure(error_service_changed))  { return error_service_changed; }
    if (is_failure(error_mtu_max_size))     { return error_mtu_max_size; }

    return errc_success;
}

ble::stack::version ble_stack::get_version() const
{
    uint32_t sd_version = 0u;
    sd_version |= SD_VARIANT_ID;
    sd_version <<= 8u;
    sd_version |= SD_MAJOR_VERSION;
    sd_version <<= 8u;
    sd_version |= SD_MINOR_VERSION;
    sd_version <<= 8u;
    sd_version |= SD_BUGFIX_VERSION;

    ble_version_t nordic_version;
    uint32_t const error_code = sd_ble_version_get(&nordic_version);
    if (error_code != NRF_SUCCESS)
    {
        memset(&nordic_version, 0, sizeof(nordic_version));
    }

    version const version_info = {
        .link_layer_version = nordic_version.version_number,
        .company_id         = nordic_version.company_id,
        .vendor_specific    = {
            nordic_version.subversion_number,
            sd_version
        }
    };

    return version_info;
}

std::errc ble_stack::enable()
{
    uint32_t sd_base_address = ram_base_address;
    uint32_t error_code = sd_ble_enable(&sd_base_address);

    logger& logger = logger::instance();
    if (ram_base_address >= sd_base_address)
    {
        logger.info("RAM starts at 0x%08x, minimum required: 0x%08x, OK",
                    ram_base_address, sd_base_address);
    }
    else
    {
        logger.error("RAM starts at 0x%08x, minimum required: 0x%08x, FAIL",
                     ram_base_address, sd_base_address);
    }

    ASSERT(error_code == NRF_SUCCESS);
    return nordic_to_system_error(error_code);
}

std::errc ble_stack::disable()
{
    uint32_t error_code = sd_softdevice_disable();
    return nordic_to_system_error(error_code);
}

bool ble_stack::is_enabled() const
{
    uint8_t is_enabled = 0u;
    sd_softdevice_is_enabled(&is_enabled);      // Always returns NRF_SUCCESS.
    return bool(is_enabled);
}

std::errc ble_stack::set_mtu_max_size(ble::att::length_t mtu_max_size)
{
    ble_cfg_t ble_cfg;
    memset(&ble_cfg, 0, sizeof(ble_cfg));

    ble_cfg.conn_cfg.conn_cfg_tag = this->connection_configuration_tag;
    ble_cfg.conn_cfg.params.gatt_conn_cfg.att_mtu = mtu_max_size;

    uint32_t const error_code = sd_ble_cfg_set(BLE_CONN_CFG_GATT,
                                               &ble_cfg,
                                               ram_base_address);
    if (error_code != NRF_SUCCESS)
    {
        logger::instance().error("set_mtu_max_size(%u): failed: %u",
                                 mtu_max_size, error_code);
    }
    else
    {
        this->constraints_.att_mtu_maximum_length = mtu_max_size;
    }

    return nordic_to_system_error(error_code);
}

std::errc ble_stack::set_link_count(uint8_t  peripheral_link_count,
                                    uint8_t  central_link_count,
                                    uint16_t event_length)
{
    logger& logger = logger::instance();

    ble_cfg_t ble_cfg;
    memset(&ble_cfg, 0, sizeof(ble_cfg));

    // Note: any of the configuration settings which are part of the
    // struct ble_conn_cfg_t member conn_cfg require the conn_cfg_tag.
    // For other settings, this tag must not be set.
    ble_cfg.conn_cfg.conn_cfg_tag = this->connection_configuration_tag;

    ble_cfg.conn_cfg.params.gap_conn_cfg.conn_count = peripheral_link_count +
                                                      central_link_count;

    ble_cfg.conn_cfg.params.gap_conn_cfg.event_length = event_length;

    logger.debug("set_link_count(%u, %u), event_length: %u",
                 peripheral_link_count, central_link_count,
                 event_length);

    uint32_t const error_cfg_gap = sd_ble_cfg_set(BLE_CONN_CFG_GAP,
                                                  &ble_cfg,
                                                  ram_base_address);
    if (error_cfg_gap != NRF_SUCCESS)
    {
        logger.error("sd_ble_cfg_set(BLE_CONN_CFG_GAP), conn_count: %u, event_length: %u: failed: %u",
                     ble_cfg.conn_cfg.params.gap_conn_cfg.conn_count,
                     ble_cfg.conn_cfg.params.gap_conn_cfg.event_length,
                     error_cfg_gap);
    }

    // Configure the connection roles.
    memset(&ble_cfg, 0, sizeof(ble_cfg));

    /// @todo For now if using a central, using only one SM for all centrals.
    uint8_t const central_sm_count = (central_link_count == 0u)
        ? 0u : BLE_GAP_ROLE_COUNT_CENTRAL_SEC_DEFAULT;

    /// @todo The advertising 'set' count is hard-coded to
    /// BLE_GAP_ADV_SET_COUNT_DEFAULT (1) for now.
    ble_cfg.gap_cfg.role_count_cfg.adv_set_count        = BLE_GAP_ADV_SET_COUNT_DEFAULT;
    ble_cfg.gap_cfg.role_count_cfg.periph_role_count    = peripheral_link_count;
    ble_cfg.gap_cfg.role_count_cfg.central_role_count   = central_link_count;
    ble_cfg.gap_cfg.role_count_cfg.central_sec_count    = central_sm_count;

    /// @todo What does this do?
    ble_cfg.gap_cfg.role_count_cfg.qos_channel_survey_role_available = false;

    uint32_t const error_cfg_role = sd_ble_cfg_set(BLE_GAP_CFG_ROLE_COUNT,
                                                   &ble_cfg,
                                                   ram_base_address);
    if (error_cfg_role != NRF_SUCCESS)
    {
        logger.error("sd_ble_cfg_set(BLE_GAP_CFG_ROLE_COUNT): adv: %u, periph: %u, central: %u, sec: %u: failed: %u",
                     ble_cfg.gap_cfg.role_count_cfg.adv_set_count,
                     ble_cfg.gap_cfg.role_count_cfg.periph_role_count,
                     ble_cfg.gap_cfg.role_count_cfg.central_role_count,
                     ble_cfg.gap_cfg.role_count_cfg.central_sec_count,
                     error_cfg_role);
    }

    uint32_t const error_code = (error_cfg_gap == NRF_SUCCESS) ? error_cfg_role
                                                               : error_cfg_gap;
    return nordic_to_system_error(error_code);
}

std::errc ble_stack::set_link_count(uint8_t peripheral_link_count,
                                    uint8_t central_link_count)
{
    return set_link_count(peripheral_link_count,
                          central_link_count,
                          BLE_GAP_EVENT_LENGTH_DEFAULT);
}

std::errc ble_stack::set_gatt_custom_uuid_count(uint8_t uuid_count)
{
    // Configure number of custom UUIDS.
    ble_cfg_t ble_cfg;
    memset(&ble_cfg, 0, sizeof(ble_cfg));

    ble_cfg.common_cfg.vs_uuid_cfg.vs_uuid_count = uuid_count;
    uint32_t const error_code = sd_ble_cfg_set(BLE_COMMON_CFG_VS_UUID,
                                               &ble_cfg,
                                               ram_base_address);
    if (error_code != NRF_SUCCESS)
    {
        logger::instance().error("set_gatt_custom_uuid_count(): failed: %u",
                                 error_code);
    }

    return nordic_to_system_error(error_code);
}

/// @todo What exactly does this do?
std::errc ble_stack::set_gatt_table_size(size_t gatt_table_size)
{
    ble_cfg_t ble_cfg;
    memset(&ble_cfg, 0, sizeof(ble_cfg));

    ble_cfg.gatts_cfg.attr_tab_size.attr_tab_size = gatt_table_size;

    uint32_t const error_code = sd_ble_cfg_set(BLE_GATTS_CFG_ATTR_TAB_SIZE,
                                               &ble_cfg,
                                               ram_base_address);
    if (error_code != NRF_SUCCESS)
    {
        logger::instance().error("set_gatt_table_size(%u): failed: %u",
                                 gatt_table_size, error_code);
    }

    return nordic_to_system_error(error_code);
}

std::errc ble_stack::set_service_changed_characteristic(bool service_changed)
{
    ble_cfg_t ble_cfg;
    memset(&ble_cfg, 0x00, sizeof(ble_cfg));

    ble_cfg.gatts_cfg.service_changed.service_changed = service_changed;
    uint32_t const error_code = sd_ble_cfg_set(BLE_GATTS_CFG_SERVICE_CHANGED,
                                               &ble_cfg,
                                               ram_base_address);
    if (error_code != NRF_SUCCESS)
    {
        logger::instance().error("set_service_changed_characteristic(%u) %u",
                                 service_changed, error_code);
    }

    return nordic_to_system_error(error_code);
}

} // namespace nordic
