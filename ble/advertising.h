/**
 * @file ble/advertising.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * A set of classes for configuring and performing BLE advertising.
 */

#pragma once

#include "ble/address.h"
#include "ble/advertising_data.h"
#include "ble/nordic_advertising.h"
#include "fixed_allocator.h"
#include <cstddef>
#include <cstring>
#include <vector>

namespace ble
{

/**
 * @class advertising
 * A generic BLE advertising class, undirected and connectable.
 */
class advertising
{
public:
    static uint16_t const interval_unspecified = 0xFFFFu;

    virtual ~advertising()                      = default;

    advertising(advertising const&)             = delete;
    advertising(advertising &&)                 = delete;
    advertising& operator=(advertising const&)  = delete;
    advertising& operator=(advertising&&)       = delete;

    advertising();
    advertising(uint16_t interval);

    void set_mode_directed(ble::address const& peer_address);

    /// @{ The advertising interval in 0.625 msec units.
    void set_advertising_interval(uint16_t interval);
    uint16_t get_advertising_interval() const;
    /// @}

    void start();
    void stop();

    std::size_t advertising_tlv_encode();

private:
    advertising_data_t advertising_data;

    /**
     * @todo For now all implementations are nordic nRF based.
     * Perhaps some other day, make it generic.
     */
    nordic::ble_advertising_params_t advertising_params;
};

} // namespace ble



extern "C" {

// This will be static and private to the base advertising class.
// void ble_advertising_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_adv);
// void ble_advertising_on_sys_evt(uint32_t sys_evt, void * p_adv);

}
