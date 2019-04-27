/**
 * @file ble/gatt_descriptors.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "ble/uuid.h"
#include "ble/att.h"
#include "ble/gatt_attribute.h"
#include "ble/gatt_declaration.h"
#include "ble/gatt_enum_types.h"

#include <cstdint>
#include <boost/intrusive/list.hpp>

namespace ble
{
namespace gatt
{

struct characteristic;

/**
 * @struct descriptor_base
 * All decriptors should inherit from this base object.
 *
 * @details
 * In the BLE specification descriptors are merely a kind of characteristic.
 * Here there is a distinction in that descriptors are associated with a
 * specific characteristic, and most importantly act upon a characterisitc.
 * Therefore, in this implementation, descriptors maintain a pointer to the
 * characteristic to which they are associated.
 */
struct descriptor_base: public ble::gatt::attribute
{
    virtual ~descriptor_base() override                   = default;

    descriptor_base(descriptor_base const&)               = delete;
    descriptor_base(descriptor_base &&)                   = delete;
    descriptor_base& operator=(descriptor_base const&)    = delete;
    descriptor_base& operator=(descriptor_base&&)         = delete;

    /**
     * The default ctor creates a descriptor with undefined context and usage.
     * These can be defined later and is useful in the GATT client scenarios.
     */
    descriptor_base():
        attribute(attribute_type::undefined, 0u),
        characteristic_ptr(nullptr)
    {
    }

    descriptor_base(ble::gatt::characteristic&  characteristic,
                    ble::gatt::attribute_type   attr_type,
                    uint16_t                    property_bits)
    :
        attribute(attr_type, property_bits),
        characteristic_ptr(&characteristic)
    {
    }

    characteristic* characteristic_ptr;
};

struct cud: public descriptor_base
{
    virtual ~cud() override     = default;

    cud()                       = delete;
    cud(cud const&)             = delete;
    cud(cud &&)                 = delete;
    cud& operator=(cud const&)  = delete;
    cud& operator=(cud&&)       = delete;

    cud(ble::gatt::characteristic&  characteristic,
        char const*                 string_ptr,
        att::length_t               string_length)
    :
        descriptor_base(
            characteristic,
            ble::gatt::attribute_type::characteristic_user_description,
            ble::gatt::properties::read),
        user_string_ptr(string_ptr),
        user_string_length(string_length)
    {
    }

    virtual void const* data_pointer() const override {
        return reinterpret_cast<void const*>(user_string_ptr);
    }

    virtual att::length_t data_length() const override {
        return user_string_length;
    }

    char const*   const user_string_ptr;
    att::length_t const user_string_length;
};

using characteristic_user_descriptor = cud;

/**
 * @class cpfd, characteristic presentation format descriptor
 */
struct cpfd: public descriptor_base
{
    virtual ~cpfd() override      = default;

    cpfd()                        = delete;
    cpfd(cpfd const&)             = delete;
    cpfd(cpfd &&)                 = delete;
    cpfd& operator=(cpfd const&)  = delete;
    cpfd& operator=(cpfd&&)       = delete;

    cpfd(ble::gatt::characteristic& characteristic,
         ble::gatt::format          presentation_format,
         int8_t                     presentation_exponent,
         ble::gatt::units_type      presentation_units,
         uint8_t                    presentation_name_space = 0u,
         uint16_t                   presentation_description = 0u)
    :
         descriptor_base(
            characteristic,
            ble::gatt::attribute_type::characteristic_presentation_format,
            ble::gatt::properties::read),
        format(presentation_format),
        exponent(presentation_exponent),
        units(presentation_units),
        name_space(presentation_name_space),
        description(presentation_description)
    {
    }

    cpfd(ble::gatt::characteristic& characteristic,
         ble::gatt::format          presentation_format,
         ble::gatt::units_type      presentation_units)
    :
         descriptor_base(
            characteristic,
            ble::gatt::attribute_type::characteristic_presentation_format,
            ble::gatt::properties::read),
        format(presentation_format),
        exponent(0),
        units(presentation_units),
        name_space(0u),
        description(0u)
    {
    }

    ble::gatt::format       const format;
    int8_t                  const exponent;
    ble::gatt::units_type   const units;
    uint8_t                 const name_space;   ///< 1: Bluetooth SIG Assigned Numbers
    uint16_t                const description;  ///< No one seems to know what this does.
};

using characteristic_presentation_format_descriptor = cpfd;

/**
 * @class cccd, client characteristic configuration descriptor
 */
struct cccd: public descriptor_base
{
    static constexpr uint16_t const notification_enable = 0x01;
    static constexpr uint16_t const indication_enable   = 0x02;

    virtual ~cccd() override        = default;

    cccd()                          = delete;
    cccd(cccd const&)               = delete;
    cccd(cccd &&)                   = delete;
    cccd& operator=(cccd const&)    = delete;
    cccd& operator=(cccd&&)         = delete;

    cccd(ble::gatt::characteristic &characteristic)
    :
        descriptor_base(
            characteristic,
            ble::gatt::attribute_type::client_characteristic_configuration,
            ble::gatt::properties::read_write),
        configuration_bits(0u)
    {
    }

    bool notifications_enabled() const {
        return bool(configuration_bits & notification_enable);
    }

    bool indications_enabled() const {
        return bool(configuration_bits & indication_enable);
    }

    virtual void const* data_pointer() const override {
        return reinterpret_cast<void const*>(&configuration_bits);
    }

    virtual att::length_t data_length() const override {
        return sizeof(configuration_bits);
    }

    /**
     * @todo @bug There needs to be a set of configuration bits for each
     * client connection made to the server.
     * @todo @bug For each bonded client connection the configuration bits
     * need to be persisted via non-volatile memory.
     */
    uint16_t configuration_bits;
};

using client_characteristic_configuration_descriptor = cccd;

/**
 * @class sccd, server characteristic configuration descriptor
 */
struct sccd: public descriptor_base
{
    static constexpr uint16_t const broadcasts_enable = 0x01;

    virtual ~sccd() override        = default;

    sccd()                          = delete;
    sccd(sccd const&)               = delete;
    sccd(sccd &&)                   = delete;
    sccd& operator=(sccd const&)    = delete;
    sccd& operator=(sccd&&)         = delete;

    sccd(characteristic &characteristic)
    :
        descriptor_base(
            characteristic,
            attribute_type::server_characteristic_configuration,
            properties::read_write),
        configuration_bits(0u)
    {
    }

    bool broadcasts_enabled() const {
        return bool(configuration_bits & broadcasts_enable);
    }

    virtual void const* data_pointer() const override {
        return reinterpret_cast<void const*>(&configuration_bits);
    }

    virtual att::length_t data_length() const override {
        return sizeof(configuration_bits);
    }

    uint16_t configuration_bits;
};

using server_characteristic_configuration_descriptor = sccd;

} // namespace gatt
} // namespace ble
