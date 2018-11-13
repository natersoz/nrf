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

namespace nordic
{

gap_advertising::gap_advertising(uint16_t advertising_interval):
    ble::gap::advertising(advertising_interval)
{
}

void gap_advertising::start()
{
    logger &logger = logger::instance();

    logger.debug("adv_data: %p, %u",
                 this->advertising_data.data(), this->advertising_data.size());
    logger.write_data(logger::level::debug,
                      this->advertising_data.data(),
                      this->advertising_data.size(),
                      true);

    uint32_t sd_result = sd_ble_gap_adv_data_set(
        this->advertising_data.data(),              // advertising data
        this->advertising_data.size(),
        nullptr,                                    // scan response data
        0u);

    ASSERT(sd_result == NRF_SUCCESS);

    /// @todo Still don't know what this config_tag means - for now default.
    /// @see more comments in ble_gatts.h:
    /// BLE_CONN_CFGS, BLE_COMMON_CFGS, BLE_GAP_CFGS, BLE_GATTS_CFGS
    uint8_t const config_tag = 1u;

    nordic::gap_advertising_params_t advertising_params;
    advertising_params.interval = this->interval;

    sd_result = sd_ble_gap_adv_start(&advertising_params, config_tag);
    ASSERT(sd_result == NRF_SUCCESS);
}

void gap_advertising::stop()
{
    sd_ble_gap_adv_stop();
}

} // namespace nordic
