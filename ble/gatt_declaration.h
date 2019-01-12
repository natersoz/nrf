/**
 * @file ble/gatt_declaration.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "ble/uuid.h"
#include "ble/att.h"
#include "ble/gatt_uuids.h"

#include <cstdint>

namespace ble
{
namespace gatt
{

/**
 * @struct properties
 * The properties determine how a service, a characteristic or a desctiptor
 * (also a characteristic) are handled. Typically this is one byte of fields
 * but may be 2 bytes if the extended flag is set.
 *
 * @see Bluetooth Core Specification 5.0, Volume 3, Part G
 * 3.3.1.1 Characteristic Properties,
 * Table 3.5: Characteristic Properties bit field
 */
struct properties
{
    ~properties()                               = default;

    properties(properties const&)               = default;
    properties(properties &&)                   = default;
    properties& operator=(properties const&)    = default;
    properties& operator=(properties&&)         = default;

    properties(): bits(0u), bits_ext(0u) {}

    properties(uint16_t props): bits(0u), bits_ext(0u)
    {
        this->set(props);
    }

    enum : uint16_t
    {
        broadcast               = 0x0001,
        read                    = 0x0002,
        write_without_response  = 0x0004,
        write                   = 0x0008,
        read_write              = (read | write),   // 0x000a
        notify                  = 0x0010,
        indicate                = 0x0020,
        write_with_signature    = 0x0040,
        extended                = 0x0080,
        write_reliable          = 0x0100,
        write_aux               = 0x0200
    };

    void set(uint16_t properties)
    {
        this->bits      = static_cast<uint8_t>(properties >> 0u);
        this->bits_ext  = static_cast<uint8_t>(properties >> 8u);
        this->bits     |= (this->bits_ext == 0u) ? 0u : extended;
    }

    uint16_t get() const
    {
        uint16_t prop_bits = this->bits_ext;
        prop_bits <<= 8u;
        prop_bits |= (prop_bits == 0u)? 0u : extended;
        prop_bits |= this->bits;
        return prop_bits;
    }

    bool is_writable()   const { return this->get() & write; }
    bool is_readable()   const { return this->get() &  read; }
    bool is_read_write() const { return this->is_readable() && this->is_writable(); }

    uint8_t     bits;
    uint8_t     bits_ext;
};

/**
 * @struct declaration
 * Declarations contain the following:
 * - A set of properties.
 * - A handle which allows the declaration to be addressed.
 * - A ble::gatt::attribute_type 16-bit UUID representing the
 *   declaration type.
 */
struct declaration
{
    ~declaration()                              = default;

    declaration()                               = delete;
    declaration(declaration const&)             = default;
    declaration(declaration &&)                 = default;
    declaration& operator=(declaration const&)  = default;
    declaration& operator=(declaration&&)       = default;

    declaration(enum attribute_type attr_type, uint16_t props):
        properties(props),
        handle(ble::att::handle_invalid),
        attribute_type(attr_type)
    {}

    struct properties           properties;
    uint16_t                    handle;
    enum attribute_type const   attribute_type;
};

} // namespace gatt
} // namespace ble
