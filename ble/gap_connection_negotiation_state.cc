/**
 * @file ble/gap_connection_negotiation_state.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * ble::gap::connection::negotiation_state implementation.
 */

#include "ble/gap_connection.h"

namespace ble
{
namespace gap
{

connection::negotiation_state::negotiation_state() :
    gatt_mtu_exchange_pending_(false),
    gap_connection_parameters_update_pending_(false),
    link_layer_update_pending_(false),
    phy_layer_update_pending_(false)
{
}

bool connection::negotiation_state::is_any_update_pending() const
{
    return
        this->gatt_mtu_exchange_pending_                ||
        this->gap_connection_parameters_update_pending_ ||
        this->link_layer_update_pending_                ||
        this->phy_layer_update_pending_                 ;
}

void connection::negotiation_state::clear_all_pending()
{
    this->gatt_mtu_exchange_pending_                = false;
    this->gap_connection_parameters_update_pending_ = false;
    this->link_layer_update_pending_                = false;
    this->phy_layer_update_pending_                 = false;
}

bool connection::negotiation_state::is_gatt_mtu_exchange_pending() const
{
    return this->gatt_mtu_exchange_pending_;
}

bool connection::negotiation_state::is_gap_connection_parameters_pending() const
{
    return this->gap_connection_parameters_update_pending_;
}

bool connection::negotiation_state::is_link_layer_update_pending() const
{
    return this->link_layer_update_pending_;
}

bool connection::negotiation_state::is_phy_layer_update_pending() const
{
    return this->phy_layer_update_pending_;
}

void connection::negotiation_state::set_gatt_mtu_exchange_pending(bool is_pending)
{
    this->gatt_mtu_exchange_pending_ = is_pending;
    this->set_pending_state_update(is_pending);
}

void connection::negotiation_state::set_gap_connection_parameters_pending(bool is_pending)
{
    this->gap_connection_parameters_update_pending_ = is_pending;
    this->set_pending_state_update(is_pending);
}

void connection::negotiation_state::set_link_layer_update_pending(bool is_pending)
{
    this->link_layer_update_pending_ = is_pending;
    this->set_pending_state_update(is_pending);
}

void connection::negotiation_state::set_phy_layer_update_pending(bool is_pending)
{
    this->phy_layer_update_pending_ = is_pending;
    this->set_pending_state_update(is_pending);
}

void connection::negotiation_state::set_pending_state_update(bool is_pending)
{
    if (is_pending)
    {
        timer_start();
    }
    else
    {
        if (this->is_any_update_pending())
        {
            timer_stop();
        }
    }
}

} // namespace gap
} // namespace ble
