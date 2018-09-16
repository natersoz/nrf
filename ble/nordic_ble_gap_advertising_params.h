/**
 * @file ble/nordic_ble_gap_advertising_params.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include <cstdint>
#include "ble_gap.h"

namespace nordic
{

struct gap_advertising_params_t: public ::ble_gap_adv_params_t
{
public:
    static uint16_t const interval_unspecified = 0xFFFFu;

    ~gap_advertising_params_t()                                          = default;

    gap_advertising_params_t(gap_advertising_params_t const&)            = default;
    gap_advertising_params_t(gap_advertising_params_t &&)                = default;
    gap_advertising_params_t& operator=(gap_advertising_params_t const&) = default;
    gap_advertising_params_t& operator=(gap_advertising_params_t&&)      = default;

    gap_advertising_params_t(
        uint8_t               type          = BLE_GAP_ADV_TYPE_ADV_IND,
        ble_gap_addr_t const* peer_address  = nullptr,
        uint8_t               filter_policy = BLE_GAP_ADV_FP_ANY,
        uint16_t              interval      = interval_unspecified,
        uint16_t              timeout       = 0u)
    {
        this->type          = type;
        this->p_peer_addr   = peer_address;
        this->fp            = filter_policy;
        this->interval      = interval;
        this->timeout       = timeout;
        this->channel_mask  = {
            .ch_37_off  = false,
            .ch_38_off  = false,
            .ch_39_off  = false,
        };
    }
};

} // namespace nordic

