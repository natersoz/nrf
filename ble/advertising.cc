/**
 * @file ble/advertising.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "ble/advertising.h"
#include "ble/gap_types.h"
#include "ble/nordic_advertising.h"

#include "logger.h"
#include "project_assert.h"

#include "ble_gap.h"                                    // Nordic softdevice
#include "nrf_error.h"

namespace ble
{

advertising::advertising(uint16_t advertising_interval)
    : allocator_(data_, sizeof(data_)),
      advertising_data(allocator_),
      interval(advertising_interval)
{
    memset(this->data_, 0, sizeof(this->data_));
    advertising_data.reserve(sizeof(this->data_));
}

void advertising::start()
{
    logger &logger = logger::instance();

    logger.debug("adv_data: %p, %u", &*this->advertising_data.begin(), this->advertising_data.size());
    logger.write_data(logger::level::debug,
                      &*this->advertising_data.begin(),
                      this->advertising_data.size(),
                      true);

    uint32_t sd_result = sd_ble_gap_adv_data_set(
        &*this->advertising_data.begin(),               // advertising data
        this->advertising_data.size(),
        nullptr,                                        // scan response data
        0u);

    ASSERT(sd_result == NRF_SUCCESS);

    /// @todo Still don't know what this config_tag means - for now nothing.
    uint8_t const config_tag = 1u;

    nordic::ble_advertising_params_t advertising_params;
    advertising_params.interval = this->interval;

    sd_result = sd_ble_gap_adv_start(&advertising_params, config_tag);
    ASSERT(sd_result == NRF_SUCCESS);
}

void advertising::stop()
{
    sd_ble_gap_adv_stop();
}

}
