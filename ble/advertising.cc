/**
 * @file ble/advertising.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "ble/advertising.h"
#include "ble/gap_types.h"
#include "ble/nordic_advertising.h"

#include "project_assert.h"

#include "ble_gap.h"                                    // Nordic softdevice
#include "nrf_error.h"

namespace ble
{

advertising::advertising(uint16_t interval)
{
    this->advertising_params.interval = interval;
}

void advertising::start()
{
    uint32_t sd_result = sd_ble_gap_adv_data_set(
        &*this->advertising_data.begin(),               // advertising data
        this->advertising_data.size(),
        nullptr,                                        // scan response data
        0u);

    ASSERT(sd_result == NRF_SUCCESS);

    /// @todo Still don't know what this config_tag means - for now nothing.
    uint8_t const config_tag = 1u;
    sd_result = sd_ble_gap_adv_start(&this->advertising_params, config_tag);

    ASSERT(sd_result == NRF_SUCCESS);
}

void advertising::stop()
{
    sd_ble_gap_adv_stop();
}

}
