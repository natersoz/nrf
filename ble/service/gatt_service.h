/**
 * @file gatt_service.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Provide the generic_attribute 0x1801
 * @see https://www.bluetooth.com/specifications/gatt
 */

#pragma once

#include "ble/gatt_service.h"
#include "ble/gatt_descriptors.h"
#include "ble/gatt_uuids.h"

namespace ble
{
namespace service
{

class service_changed: public gatt::characteristic
{
public:
    virtual ~service_changed() override                 = default;

    service_changed(service_changed const&)             = delete;
    service_changed(service_changed &&)                 = delete;
    service_changed& operator=(service_changed const&)  = delete;
    service_changed& operator=(service_changed&&)       = delete;

    service_changed() :
        gatt::characteristic(gatt::characteristics::service_changed,
                             gatt::properties::indicate),
        cccd_(*this)
    {
        this->descriptor_add(cccd_);
    }

    virtual void *data_pointer() override {
        return const_cast<handles_t *>(&this->service_changed_);
    }

    virtual att::length_t data_length() const override {
        return sizeof(this->service_changed_);
    }

    void services_changed(uint16_t service_handle_first,
                          uint16_t service_handle_last)
    {
        this->service_changed_.handle_first = service_handle_first;
        this->service_changed_.handle_last  = service_handle_last;

        /// @todo Implement notification code.
    }

private:
    struct handles_t
    {
        ~handles_t() = default;
        handles_t(): handle_first(0u), handle_last(0u) {};

        uint16_t handle_first;
        uint16_t handle_last;
    };

    handles_t service_changed_;

    ble::gatt::client_characteristic_configuration_descriptor cccd_;
};

class gatt_service: public gatt::service
{
public:
    virtual ~gatt_service() override             = default;

    gatt_service(gatt_service const&)             = delete;
    gatt_service(gatt_service &&)                 = delete;
    gatt_service& operator=(gatt_service const&)  = delete;
    gatt_service& operator=(gatt_service&&)       = delete;

    /// Construct a Generic Attribute (GATT) Service as a primary service.
    gatt_service():
        service(gatt::services::generic_attribute,
                gatt::attribute_type::primary_service)
    {
        this->characteristic_add(this->service_changed);
    }

    class service_changed service_changed;
};

} // namespace service
} // namespace ble

