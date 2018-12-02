/**
 * @file ble/advertising.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "ble/gap_types.h"
#include "ble/nordic_ble_gap_advertising.h"
#include "ble/nordic_ble_gap_advertising_params.h"

#include "logger.h"
#include "project_assert.h"

#include "ble.h"                // Nordic softdevice
#include "ble_gap.h"
#include "nrf_error.h"

static_assert(
    ble::gap::advertising_data::max_length == BLE_GAP_ADV_SET_DATA_SIZE_MAX);

namespace nordic
{

ble_gap_advertising::ble_gap_advertising(uint16_t advertising_interval):
    ble::gap::advertising(advertising_interval),
    advertising_handle_(0u)
{
}

void ble_gap_advertising::start()
{
    logger &logger = logger::instance();

    logger.debug("adv_data: %p, %u", this->data.data(), this->data.size());

    logger.write_data(logger::level::debug,
                      this->data.data(), this->data.size(), true);

    this->advertising_handle_ = 0u;
    ble_gap_adv_data_t const nordic_advertising_data = {
        // Advertising data:
        .adv_data = {
            .p_data = this->data.data(),
            .len    = static_cast<uint16_t>(this->data.size())
        },
        // Scan/Response data:
        .scan_rsp_data = {
            .p_data = nullptr,
            .len    = 0u
        }
    };

    nordic::gap_advertising_params_t advertising_params(this->interval);

    uint32_t error_code = sd_ble_gap_adv_set_configure(
        &this->advertising_handle_,
        &nordic_advertising_data,
        &advertising_params);

    ASSERT(error_code == NRF_SUCCESS);

    /// @todo Still don't know what this config_tag means - for now default.
    /// @see more comments in ble_gatts.h:
    /// BLE_CONN_CFGS, BLE_COMMON_CFGS, BLE_GAP_CFGS, BLE_GATTS_CFGS
    uint8_t const config_tag = 1u;

    error_code = sd_ble_gap_adv_start(this->advertising_handle_, config_tag);
    ASSERT(error_code == NRF_SUCCESS);
}

void ble_gap_advertising::stop()
{
    sd_ble_gap_adv_stop(this->advertising_handle_);
}

} // namespace nordic
