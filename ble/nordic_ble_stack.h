/**
 * @file ble/nordic_ble_stack.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

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

    ble_stack(uint8_t conn_cfg_tag):
        ble::stack(),
        connection_configuration_tag_(conn_cfg_tag)
    {}

    std::errc init() override;
    std::errc enable() override;
    std::errc disable() override;
    bool      is_enabled() const override;

private:
    /// See doc/nordic_ble_conn_cfg.h
    /// for evolving documentation on this tag's usage and meaning.
    uint8_t const connection_configuration_tag_;

    uint32_t set_configuration(uintptr_t    ram_base_address,
                               uint8_t      total_link_count,
                               uint8_t      peripheral_link_count,
                               uint16_t     mtu_size,
                               uint8_t      gatt_uuid_count,
                               uint32_t     gatt_table_size,
                               bool         service_changed);
};

} // namespace nordic




