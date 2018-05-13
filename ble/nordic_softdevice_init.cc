/**
 * @file nordic_ble_stack_init.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "nordic_softdevice_init.h"
#include "logger.h"

#include "ble.h"
#include "nrf_error.h"

#include <cstring>

namespace nordic
{

uint32_t softdevice_init(uintptr_t    ram_base_address,
                         uint8_t      total_link_count,
                         uint8_t      peripheral_link_count,
                         uint16_t     mtu_size,
                         uint8_t      gatt_uuid_count,
                         uint32_t     gatt_table_size,
                         bool         service_changed)
{
    logger& logger = logger::instance();

    /**
     * @param conn_cfg_tag
     * It appears that conn_cfg_tag is associated with an advertising profile
     * and a conection configuration (in this function).
     * @todo Still don't know what this config_tag means - for now nothing.
     *
     * @note Must be different for all connection configurations added and
     * not @ref BLE_CONN_CFG_TAG_DEFAULT.
     */
    uint8_t const conn_cfg_tag = 1u;

    uint32_t ret_code = NRF_SUCCESS;

    // Overwrite some of the default settings of the BLE stack.
    // If any of the calls to sd_ble_cfg_set() fail, log the error but carry on
    // so that wrong RAM settings can be caught by nrf_sdh_ble_enable()
    // and a meaningful error  message will be printed to the user suggesting
    // the correct value.

    ble_cfg_t ble_cfg;
    memset(&ble_cfg, 0, sizeof(ble_cfg));

    // Configure the connection count.
    ble_cfg.conn_cfg.conn_cfg_tag = conn_cfg_tag;

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
    ble_cfg.conn_cfg.conn_cfg_tag                   = conn_cfg_tag;
    ble_cfg.conn_cfg.params.gatt_conn_cfg.att_mtu   = mtu_size;

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

} // namespace nordic
