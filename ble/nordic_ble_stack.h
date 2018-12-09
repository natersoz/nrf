/**
 * @file ble/nordic_ble_stack.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "ble/att.h"
#include "ble/stack.h"
#include <cstdint>

namespace nordic
{

/**
 * Nordic BLE stack initialization call chain:
 *
 * sd_ble_cfg_set() may be called when
 * - the SoftDevice is enabled;             see sd_softdevice_enable()
 * - the SoftDevice BLE part is disabled;   see sd_ble_enable()
 *
 *  ble_stack_init() {
 *      nrf_sdh_enable_request() {
 *          sdh_request_observer_notify(NRF_SDH_EVT_ENABLE_REQUEST);
 *          sdh_state_observer_notify(NRF_SDH_EVT_STATE_ENABLE_PREPARE);
 *
 *          sd_softdevice_enable(&clock_lf_cfg, app_error_fault_handler);
 *          swi_interrupt_priority_workaround();
 *          softdevices_evt_irq_enable();
 *
 *          sdh_state_observer_notify(NRF_SDH_EVT_STATE_ENABLED);
 *      }
 *
 *      nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG) {
 *          sd_ble_cfg_set(BLE_CONN_CFG_GAP)                 Connection count
 *          sd_ble_cfg_set(BLE_GAP_CFG_ROLE_COUNT)           Connection roles
 *          sd_ble_cfg_set(BLE_CONN_CFG_GATT)                Connection ATT MTU
 *          sd_ble_cfg_set(BLE_COMMON_CFG_VS_UUID)           Custom UUID count
 *          sd_ble_cfg_set(BLE_GATTS_CFG_ATTR_TAB_SIZE)      GATTS attribute table
 *          sd_ble_cfg_set(BLE_GATTS_CFG_SERVICE_CHANGED)    Service Changed characteristic
 *      }
 *
 *      nrf_sdh_ble_enable(&ram_start) {
 *          sd_ble_enable();
 *      }
 *  }
 */
class ble_stack: public ble::stack
{
public:
    virtual ~ble_stack() override           = default;

    ble_stack()                             = delete;
    ble_stack(ble_stack const&)             = delete;
    ble_stack(ble_stack &&)                 = delete;
    ble_stack& operator=(ble_stack const&)  = delete;
    ble_stack& operator=(ble_stack&&)       = delete;

    ble_stack(uint8_t conn_cfg_tag);

    std::errc init(unsigned int peripheral_count,
                   unsigned int central_count) override;

    std::errc enable() override;
    std::errc disable() override;
    bool      is_enabled() const override;

    version get_version() const override;

    std::errc set_mtu_max_size(ble::att::length_t mtu_max_size) override;

    /**
     * Set the softdevice peripheral and central connection counts
     * and the event length. The default values for the link counts are:
     * peripheral: 1, central: 0
     *
     * @param peripheral_link_count The number of peripherals that can be supported
     *                              simultaneously by the softdevice.
     * @param central_link_count    The number of centrals that can be supported
     *                              simultaneously by the softdevice.
     * @note                        For Nodic, the default number of connections is
     *                              BLE_GAP_CONN_COUNT_DEFAULT.
     * @param event_length          The time set aside for this connection on every
     *                              connection interval in 1.25 ms units.
     *                              Minimum event length: BLE_GAP_EVENT_LENGTH_MIN
     *                              See the SoftDevice Specification for details.
     * @return std::errc
     */
    std::errc set_link_count(uint8_t  peripheral_link_count,
                             uint8_t  central_link_count,
                             uint16_t event_length);

    /**
     * Set the softdevice peripheral and central connection counts
     * and the default event length BLE_GAP_EVENT_LENGTH_DEFAULT.
     * Set the softdevice connection counts, peripheral and central, as well
     * as the event length.
     */
    std::errc set_link_count(uint8_t peripheral_link_count,
                             uint8_t central_link_count);

    /**
     * Set the maximum number of 128-bit UUIDs that will be used by the all
     * BLE applications.
     *
     * @param uuid_count The number of 128-bit UUIDs. This is used in a Nordic
     * internal table which contains 128-bit UUIDs and maps them to 16-bit calls.
     *
     * @note Speculation: If all custom 128-bit UUIDs use the same base value
     * and only the 16-bt value changes, this value can be one.
     *
     * @return std::errc The standard error code.
     */
    std::errc set_gatt_custom_uuid_count(uint8_t uuid_count);

    /**
     * @todo I have no idea what this does.
     * @param gatt_table_size Must be a multiple of 4u.
     * @return std::errc The standard error code.
     */
    std::errc set_gatt_table_size(size_t gatt_table_size);

    /**
     * Add the 'service changed' characteristic 0x2a05 to the
     * 'generic_attribute' 0x1801 service. If the services of the GATTS change
     * then this attribute indicates to the client the event.
     *
     * @param service_changed true:  the characteristic is included;
     *                        false: not included.
     * @return std::errc The standard error code.
     */
    std::errc set_service_changed_characteristic(bool service_changed);

    /// The RAM region base address reserved for use by the softdevice.
    static uintptr_t const ram_base_address;

    /// See doc/nordic_ble_conn_cfg.h
    /// for evolving documentation on this tag's usage and meaning.
    uint8_t const connection_configuration_tag;
};

} // namespace nordic




