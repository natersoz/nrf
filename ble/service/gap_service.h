/**
 * @file gap_service.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Provide the generic service 0x1800
 * @see https://www.bluetooth.com/specifications/gatt
 */

#pragma once

#include "ble/gatt_service.h"
#include "ble/gap_types.h"
#include "ble/gatt_uuids.h"
#include "ble/gatt_appearance.h"

namespace ble
{
namespace service
{

class device_name: public gatt::characteristic
{
public:
    virtual ~device_name() override             = default;

    device_name(device_name const&)             = delete;
    device_name(device_name &&)                 = delete;
    device_name& operator=(device_name const&)  = delete;
    device_name& operator=(device_name&&)       = delete;

    device_name() :
        gatt::characteristic(gatt::characteristics::device_name,
                             gatt::properties::read),
        device_name_(nullptr),
        device_name_length_(0u)
    {
    }

    device_name(char const *device_name, att::length_t device_name_length):
        gatt::characteristic(gatt::characteristics::device_name,
                             gatt::properties::read),
        device_name_(device_name),
        device_name_length_(device_name_length)
    {
    }

    virtual void *data_pointer() override {
        return const_cast<char *>(this->device_name_);
    }

    virtual att::length_t data_length() const override {
        return this->device_name_length_;
    }

    void set_device_name(char const *device_name, att::length_t device_name_length)
    {
        this->device_name_ = device_name;
        this->device_name_length_ = device_name_length;
    }

private:
    char const    *device_name_;
    att::length_t device_name_length_;
};

/**
 * @class appearance
 * @see enum class appearance
 */
class appearance: public gatt::characteristic
{
public:
    virtual ~appearance() override            = default;

    appearance(appearance const&)             = delete;
    appearance(appearance &&)                 = delete;
    appearance& operator=(appearance const&)  = delete;
    appearance& operator=(appearance&&)       = delete;

    appearance(ble::gatt::appearance device_appearance) :
        gatt::characteristic(gatt::characteristics::appearance,
                             gatt::properties::read),
        appearance_(device_appearance)
    {
    }

    virtual void *data_pointer() override {
        return const_cast<ble::gatt::appearance*>(&this->appearance_);
    }

    virtual att::length_t data_length() const override {
        return sizeof(this->appearance_);
    }

private:
    ble::gatt::appearance const appearance_;
};

/**
 * @class ppcp
 * Peripheral Preferred Connection Parameters
 */
class ppcp: public gatt::characteristic
{
public:
    virtual ~ppcp() override      = default;

    ppcp(ppcp const&)             = delete;
    ppcp(ppcp &&)                 = delete;
    ppcp& operator=(ppcp const&)  = delete;
    ppcp& operator=(ppcp&&)       = delete;

    ppcp(gap::connection_parameters connection_parameters) :
        gatt::characteristic(gatt::characteristics::ppcp, gatt::properties::read),
        connection_parameters_(connection_parameters)
    {
    }

    virtual void *data_pointer() override {
        return const_cast<ble::gap::connection_parameters*>(&this->connection_parameters_);
    }

    virtual att::length_t data_length() const override {
        return sizeof(this->connection_parameters_);
    }

private:
    ble::gap::connection_parameters const connection_parameters_;
};

class gap_service: public gatt::service
{
public:
    virtual ~gap_service() override             = default;

    gap_service(gap_service const&)             = delete;
    gap_service(gap_service &&)                 = delete;
    gap_service& operator=(gap_service const&)  = delete;
    gap_service& operator=(gap_service&&)       = delete;

    /// Construct a Generic Access (GAP) Service as a primary service.
    gap_service():
        service(gatt::services::generic_access,
                gatt::attribute_type::primary_service)
    {
    }
};

} // namespace service
} // namespace ble

