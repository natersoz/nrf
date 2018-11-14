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
    static uint16_t const interval_unlimited   = 0x0000u;

    ~gap_advertising_params_t()                                          = default;

    gap_advertising_params_t(gap_advertising_params_t const&)            = default;
    gap_advertising_params_t(gap_advertising_params_t &&)                = default;
    gap_advertising_params_t& operator=(gap_advertising_params_t const&) = default;
    gap_advertising_params_t& operator=(gap_advertising_params_t&&)      = default;

    gap_advertising_params_t(uint16_t interval)
    {
        memset(this, 0, sizeof(*this));

        this->properties.type               = BLE_GAP_ADV_TYPE_CONNECTABLE_SCANNABLE_UNDIRECTED;
        this->properties.anonymous          = false;
        this->properties.include_tx_power   = false;

        this->p_peer_addr                   = nullptr;

        this->interval                      = interval;
        this->duration                      = interval_unlimited;
        this->max_adv_evts                  = interval_unlimited;

        this->filter_policy                 = BLE_GAP_ADV_FP_ANY;
        this->primary_phy                   = BLE_GAP_PHY_AUTO;
        this->secondary_phy                 = BLE_GAP_PHY_AUTO;
        this->set_id                        = 0u;
        this->scan_req_notification         = 0u;
    }
};

} // namespace nordic

