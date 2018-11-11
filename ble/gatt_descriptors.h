/**
 * @file ble/gatt_descriptors.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "ble/uuid.h"
#include "ble/att.h"
#include "ble/gatt_attribute.h"
#include "ble/gatt_declaration.h"
#include "ble/gatt_uuids.h"
#include "ble/gatt_format.h"

#include <cstdint>
#include <boost/intrusive/list.hpp>

namespace ble
{
namespace gatt
{

struct characteristic;

struct characteristic_base_descriptor: public attribute
{
    virtual ~characteristic_base_descriptor() override                                  = default;

    characteristic_base_descriptor()                                                    = delete;
    characteristic_base_descriptor(characteristic_base_descriptor const&)               = delete;
    characteristic_base_descriptor(characteristic_base_descriptor &&)                   = delete;
    characteristic_base_descriptor& operator=(characteristic_base_descriptor const&)    = delete;
    characteristic_base_descriptor& operator=(characteristic_base_descriptor&&)         = delete;

    characteristic_base_descriptor(characteristic&  characteristic,
                                   attribute_type   attr_type,
                                   uint16_t         property_bits):
        attribute(attr_type, property_bits),
        characteristic_ptr(&characteristic)
    {
    }

    characteristic* const characteristic_ptr;
};

struct characteristic_user_descriptor: public characteristic_base_descriptor
{
    virtual ~characteristic_user_descriptor() override                                  = default;

    characteristic_user_descriptor()                                                    = delete;
    characteristic_user_descriptor(characteristic_user_descriptor const&)               = delete;
    characteristic_user_descriptor(characteristic_user_descriptor &&)                   = delete;
    characteristic_user_descriptor& operator=(characteristic_user_descriptor const&)    = delete;
    characteristic_user_descriptor& operator=(characteristic_user_descriptor&&)         = delete;

    characteristic_user_descriptor(characteristic &characteristic,
                                   char const     *string_ptr,
                                   att::length_t   string_length) :
        characteristic_base_descriptor(
            characteristic,
            attribute_type::characteristic_user_description,
            properties::read),
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

/**
 * @class cpfd, characteristic presentation format descriptor
 */
struct cpfd: public characteristic_base_descriptor
{
    virtual ~cpfd() override      = default;

    cpfd()                        = delete;
    cpfd(cpfd const&)             = delete;
    cpfd(cpfd &&)                 = delete;
    cpfd& operator=(cpfd const&)  = delete;
    cpfd& operator=(cpfd&&)       = delete;

    cpfd(characteristic &characteristic,
         enum format    presentation_format,
         int8_t         presentation_exponent,
         enum units     presentation_units,
         uint8_t        presentation_name_space = 0u,
         uint16_t       presentation_description = 0u) :
         characteristic_base_descriptor(
            characteristic,
            attribute_type::characteristic_presentation_format,
            properties::read),
        format(presentation_format),
        exponent(presentation_exponent),
        units(presentation_units),
        name_space(presentation_name_space),
        description(presentation_description)
    {
    }

    cpfd(characteristic &characteristic,
         enum format    presentation_format,
         enum units     presentation_units):
         characteristic_base_descriptor(
            characteristic,
            attribute_type::characteristic_presentation_format,
            properties::read),
        format(presentation_format),
        exponent(0),
        units(presentation_units),
        name_space(0u),
        description(0u)
    {
    }

    enum format const format;       ///< @see enum class ble::gatt::format.
    int8_t      const exponent;
    enum units  const units;        ///< @see enum class ble::gatt::units.
    uint8_t     const name_space;   ///< 1: Bluetooth SIG Assigned Numbers
    uint16_t    const description;  ///< No one seems to know what this does.
};

using characteristic_presentation_format_descriptor = cpfd;

/**
 * @class cccd, client characteristic configuration descriptor
 */
struct cccd: public characteristic_base_descriptor
{
    static constexpr uint16_t const notification_enable = 0x01;
    static constexpr uint16_t const indication_enable   = 0x02;

    virtual ~cccd() override                                                  = default;

    cccd()                                                                    = delete;
    cccd(cccd const&)               = delete;
    cccd(cccd &&)                   = delete;
    cccd& operator=(cccd const&)    = delete;
    cccd& operator=(cccd&&)         = delete;

    cccd(characteristic &characteristic):
        characteristic_base_descriptor(
            characteristic,
            attribute_type::client_characteristic_configuration,
            properties::read_write),
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
struct sccd: public characteristic_base_descriptor
{
    static constexpr uint16_t const broadcasts_enable = 0x01;

    virtual ~sccd() override        = default;

    sccd()                          = delete;
    sccd(sccd const&)               = delete;
    sccd(sccd &&)                   = delete;
    sccd& operator=(sccd const&)    = delete;
    sccd& operator=(sccd&&)         = delete;

    sccd(characteristic &characteristic):
        characteristic_base_descriptor(
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
