/**
 * @file battery_service.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "ble/service/battery_service.h"
#include "bit_manip.h"

namespace ble
{
namespace service
{

void battery_level::write_battery_percentage(uint8_t battery_percentage)
{
    this->battery_percentage_ = battery_percentage;

    /// @todo Add notification code ...
}

uint8_t battery_level::read_battery_percentage() const
{
    return this->battery_percentage_;
}

void battery_power_state::write_battery_power_state(presence     presence_state,
                                                    discharging  discharging_state,
                                                    charging     charging_state,
                                                    level        level_state)
{
    uint8_t state = 0u;
    state = bit_manip::value_set(state, static_cast<uint8_t>(presence_state),    1u, 0u);
    state = bit_manip::value_set(state, static_cast<uint8_t>(discharging_state), 3u, 2u);
    state = bit_manip::value_set(state, static_cast<uint8_t>(charging_state),    5u, 4u);
    state = bit_manip::value_set(state, static_cast<uint8_t>(level_state),       7u, 6u);

    this->battery_power_state_ = state;

}

battery_power_state::presence battery_power_state::read_battery_power_presence() const
{
    uint8_t const value = bit_manip::value_get(this->battery_power_state_, 1u, 0u);
    return static_cast<presence>(value);
}

battery_power_state::discharging battery_power_state::read_battery_power_discharging() const
{
    uint8_t const value = bit_manip::value_get(this->battery_power_state_, 3u, 2u);
    return static_cast<discharging>(value);
}

battery_power_state::charging battery_power_state::read_battery_power_charging() const
{
    uint8_t const value = bit_manip::value_get(this->battery_power_state_, 5u, 4u);
    return static_cast<charging>(value);
}

battery_power_state::level battery_power_state::read_battery_power_level() const
{
    uint8_t const value = bit_manip::value_get(this->battery_power_state_, 7u, 6u);
    return static_cast<level>(value);
}

} // namespace service
} // namespace ble

