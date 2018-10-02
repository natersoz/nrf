/**
 * @file ble/gatt_descriptors.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "ble/uuid.h"
#include "ble/att.h"
#include "ble/gatt_declaration.h"
#include "ble/gatt_uuids.h"
#include "ble/gatt_format.h"

#include <cstdint>
#include <boost/intrusive/list.hpp>

namespace ble
{
namespace gatt
{

// Forward declare ble::gatt::characteristic.
// It has dependencies on the descriptors and
// the descriptors are dependent on characteristic.
struct characteristic;

struct characteristic_base_descriptor
{
    virtual ~characteristic_base_descriptor()                                           = default;

    characteristic_base_descriptor()                                                    = delete;
    characteristic_base_descriptor(characteristic_base_descriptor const&)               = delete;
    characteristic_base_descriptor(characteristic_base_descriptor &&)                   = delete;
    characteristic_base_descriptor& operator=(characteristic_base_descriptor const&)    = delete;
    characteristic_base_descriptor& operator=(characteristic_base_descriptor&&)         = delete;

    characteristic_base_descriptor(characteristic &characteristic,
                                   attribute_type attr_type,
                                   uint16_t       property_bits):
        decl(attr_type, property_bits),
        characteristic_ptr(&characteristic)
    {
    }

    declaration           decl;
    characteristic* const characteristic_ptr;

private:
    // Contain descriptors within Characteristics.
    using list_hook_type = boost::intrusive::list_member_hook<
        boost::intrusive::link_mode<boost::intrusive::auto_unlink>
        >;
    list_hook_type hook_;

    friend struct characteristic;
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
                                   size_t         string_length):
        characteristic_base_descriptor(
            characteristic,
            attribute_type::characteristic_user_description,
            properties::read),
        user_string_ptr(string_ptr),
        user_string_length(string_length)
    {
    }

    /// @todo Implement the case where the user descriptor can be written.
    char const* const   user_string_ptr;
    uint16_t            user_string_length;
};

struct characteristic_presentation_format_descriptor: public characteristic_base_descriptor
{
    virtual ~characteristic_presentation_format_descriptor() override                                               = default;

    characteristic_presentation_format_descriptor()                                                                 = delete;
    characteristic_presentation_format_descriptor(characteristic_presentation_format_descriptor const&)             = delete;
    characteristic_presentation_format_descriptor(characteristic_presentation_format_descriptor &&)                 = delete;
    characteristic_presentation_format_descriptor& operator=(characteristic_presentation_format_descriptor const&)  = delete;
    characteristic_presentation_format_descriptor& operator=(characteristic_presentation_format_descriptor&&)       = delete;

    characteristic_presentation_format_descriptor(characteristic &characteristic,
                                                  enum format    presentation_format,
                                                  int8_t         presentation_exponent,
                                                  enum units     presentation_units,
                                                  uint8_t        presentation_name_space = 0u,
                                                  uint16_t       presentation_description = 0u
                                                 ):
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

    characteristic_presentation_format_descriptor(characteristic &characteristic,
                                                  enum format    presentation_format,
                                                  enum units     presentation_units
                                                 ):
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

    enum format           const format;         ///< @see enum class ble::gatt::format.
    int8_t                const exponent;
    enum units            const units;          ///< @see enum class ble::gatt::units.
    uint8_t               const name_space;     ///< 1: Bluetooth SIG Assigned Numbers
    uint16_t              const description;    ///< No one seems to know what this does.
};

using cpfd = characteristic_presentation_format_descriptor;

struct client_characteristic_configuration_descriptor: public characteristic_base_descriptor
{
    static constexpr uint16_t const notification_enable = 0x01;
    static constexpr uint16_t const indication_enable   = 0x02;

    virtual ~client_characteristic_configuration_descriptor() override                                                  = default;

    client_characteristic_configuration_descriptor()                                                                    = delete;
    client_characteristic_configuration_descriptor(client_characteristic_configuration_descriptor const&)               = delete;
    client_characteristic_configuration_descriptor(client_characteristic_configuration_descriptor &&)                   = delete;
    client_characteristic_configuration_descriptor& operator=(client_characteristic_configuration_descriptor const&)    = delete;
    client_characteristic_configuration_descriptor& operator=(client_characteristic_configuration_descriptor&&)         = delete;

    client_characteristic_configuration_descriptor(characteristic &characteristic):
        characteristic_base_descriptor(
            characteristic,
            attribute_type::client_characteristic_configuration,
            properties::read_write),
        configuration_bits(0u)
    {
    }

    bool notifications_enabled() const { return bool(configuration_bits & notification_enable); }
    bool indications_enabled() const { return bool(configuration_bits & indication_enable); }

    /**
     * @todo @bug There needs to be a set of configuration bits for each
     * client connection made to the server.
     * @todo @bug For each bonded client connection the configuration bits
     * need to be persisted via non-volatile memory.
     */
    uint16_t                    configuration_bits;
};

using cccd = client_characteristic_configuration_descriptor;

struct server_characteristic_configuration_descriptor: public characteristic_base_descriptor
{
    static constexpr uint16_t const broadcasts_enable = 0x01;

    virtual ~server_characteristic_configuration_descriptor() override                                                  = default;

    server_characteristic_configuration_descriptor()                                                                    = delete;
    server_characteristic_configuration_descriptor(server_characteristic_configuration_descriptor const&)               = delete;
    server_characteristic_configuration_descriptor(server_characteristic_configuration_descriptor &&)                   = delete;
    server_characteristic_configuration_descriptor& operator=(server_characteristic_configuration_descriptor const&)    = delete;
    server_characteristic_configuration_descriptor& operator=(server_characteristic_configuration_descriptor&&)         = delete;

    server_characteristic_configuration_descriptor(characteristic &characteristic):
        characteristic_base_descriptor(
            characteristic,
            attribute_type::server_characteristic_configuration,
            properties::read_write),
        configuration_bits(0u)
    {
    }

    bool broadcasts_enabled() const { return bool(configuration_bits & broadcasts_enable); }

    uint16_t                    configuration_bits;
};

using sccd = server_characteristic_configuration_descriptor;

} // namespace gatt
} // namespace ble
