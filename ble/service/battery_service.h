/**
 * @file battery_service.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * @see
 * https://www.bluetooth.com/specifications/gatt
 * https://www.bluetooth.org/docman/handlers/downloaddoc.ashx?doc_id=245138
 * https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.service.battery_service.xml
 */

#pragma once

#include "ble/gatt_service.h"
#include "ble/gatt_descriptors.h"
#include "ble/gatt_uuids.h"

namespace ble
{
namespace service
{

/**
 * @todo Add the Characteristic Presentation Format:
 * https://www.bluetooth.org/docman/handlers/downloaddoc.ashx?doc_id=245138
 * 3.1.2.1 Characteristic Presentation Format
 * When a device has more than one instance of the Battery service,
 * each Battery Level characteristic shall include a
 * Characteristic Presentation Format descriptor that has a
 * namespace/description value that is unique for that instance of
 * the Battery service.
 */
class battery_level: public gatt::characteristic
{
public:
    virtual ~battery_level() override                 = default;

    battery_level(battery_level const&)               = delete;
    battery_level(battery_level &&)                   = delete;
    battery_level& operator=(battery_level const&)    = delete;
    battery_level& operator=(battery_level&&)         = delete;

    battery_level() :
        gatt::characteristic(gatt::characteristics::battery_level,
                             gatt::properties::read  |
                             gatt::properties::notify),
        cccd(*this),
        battery_percentage_(0u)
    {
        this->descriptor_add(this->cccd);
    }

    virtual void const* data_pointer() const override {
        return &this->battery_percentage_;
    }

    virtual att::length_t data_length() const override {
        return sizeof(this->battery_percentage_);
    }

    gatt::client_characteristic_configuration_descriptor cccd;

    void write_battery_percentage(uint8_t battery_percentage);
    uint8_t read_battery_percentage() const;

private:
    uint8_t battery_percentage_;
};

/**
 * @see https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.battery_power_state.xml
 *
 * @todo Consider adding the Characteristic Presentation Format 8-bit
 * to this characteristic.
 */
class battery_power_state: public gatt::characteristic
{
public:
    enum class presence
    {
        unknown         = 0,
        not_supported   = 1,
        not_present     = 2,
        present         = 3
    };

    enum class discharging
    {
        unknown         = 0,
        not_supported   = 1,
        not_discharging = 2,
        discharging     = 3
    };

    enum class charging
    {
        unknown         = 0,
        not_chargeable  = 1,
        not_charging    = 2,
        charging        = 3
    };

    enum class level
    {
        unknown         = 0,
        not_supported   = 1,
        good            = 2,
        critically_low  = 3
    };

    virtual ~battery_power_state() override                     = default;

    battery_power_state(battery_power_state const&)             = delete;
    battery_power_state(battery_power_state &&)                 = delete;
    battery_power_state& operator=(battery_power_state const&)  = delete;
    battery_power_state& operator=(battery_power_state&&)       = delete;

    battery_power_state() :
        gatt::characteristic(gatt::characteristics::battery_power_state,
                             gatt::properties::read  |
                             gatt::properties::notify),
        cccd(*this),
        battery_power_state_(0u)
    {
        this->descriptor_add(this->cccd);
    }

    virtual void const* data_pointer() const override {
        return &this->battery_power_state_;
    }

    virtual att::length_t data_length() const override {
        return sizeof(this->battery_power_state_);
    }

    void write_battery_power_state(presence     present_state,
                                   discharging  discharging_state,
                                   charging     charging_state,
                                   level        level_state);

    gatt::client_characteristic_configuration_descriptor cccd;

    presence    read_battery_power_presence() const;
    discharging read_battery_power_discharging() const;
    charging    read_battery_power_charging() const;
    level       read_battery_power_level() const;

private:
    uint8_t battery_power_state_;
};

class battery_service: public gatt::service
{
public:
    virtual ~battery_service() override                 = default;

    battery_service(battery_service const&)             = delete;
    battery_service(battery_service &&)                 = delete;
    battery_service& operator=(battery_service const&)  = delete;
    battery_service& operator=(battery_service&&)       = delete;

    /**
     * Construct a Battery Service as a primary service.
     *
     * @param attr_type Specify whether the battery service
     * is a primary or secondary service.
     */
    battery_service():
        service(gatt::services::battery_service,
                gatt::attribute_type::primary_service)
    {
    }

    /**
     * Construct a Battery Service object.
     *
     * @param attr_type Specify whether the battery service
     * is a primary or secondary service.
     */
    battery_service(gatt::attribute_type attr_type):
        service(gatt::services::battery_service, attr_type)
    {
    }
};

} // namespace service
} // namespace ble

