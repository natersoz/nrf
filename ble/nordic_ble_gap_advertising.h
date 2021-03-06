/**
 * @file ble/nordic_ble_gap_advertising.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Base implementation for Nordic softdevce based advertising.
 */

#pragma once

#include "ble/gap_advertising.h"

namespace nordic
{

/**
 * @class nordic::gap_advertising
 * Base implementation for Nordic softdevce based advertising.
 */
class ble_gap_advertising: public ble::gap::advertising
{
public:

    virtual ~ble_gap_advertising()                              = default;
    ble_gap_advertising()                                       = delete;

    ble_gap_advertising(ble_gap_advertising const&)             = delete;
    ble_gap_advertising(ble_gap_advertising &&)                 = delete;
    ble_gap_advertising& operator=(ble_gap_advertising const&)  = delete;
    ble_gap_advertising& operator=(ble_gap_advertising&&)       = delete;

    /**
     * Create the advertising class with a specific intreval based on
     * 0.625 msec tick counts.
     *
     * @param advertising_interval The advertising interval in 0.625 msec ticks.
     */
    ble_gap_advertising(uint16_t advertising_interval = advertising::interval_unspecified);

    /**
     * Start advertising.
     * The advertising data contained in the public member advertising_data
     * will be used when advertising starts.
     */
    virtual void start() override;

    /** Stop advertising. */
    virtual void stop() override;

private:
    uint8_t advertising_handle_;
};

} // namespace nordic
