/**
 * @file ble/gatt_characteristic.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "ble/uuid.h"
#include "ble/att.h"
#include "ble/gatt_declaration.h"
#include "ble/gatt_descriptors.h"
#include "ble/gatt_uuids.h"
#include "ble/gatt_format.h"

#include <cstdint>
#include <boost/intrusive/list.hpp>

namespace ble
{
namespace gatt
{

/**
 * @struct characteristic
 * Always consists of:
 * - A declaration, which contains:
 *   - A set of properties.
 *   - A handle (of the declaration).
 *     This handle allows the server to modify the properties if necessary.
 *   - The UUID 0x2803, attribute_types::characteristic.
 * - A handle.
 * - A uuid.
 * - Data which can be manipulated according to the properties.
 */
struct characteristic
{
    virtual ~characteristic()                           = default;

    characteristic()                                    = delete;
    characteristic(characteristic const&)               = delete;
    characteristic(characteristic &&)                   = delete;
    characteristic& operator=(characteristic const&)    = delete;
    characteristic& operator=(characteristic&&)         = delete;

    characteristic(att::uuid characterstic_uuid, uint16_t prop_bits) :
        decl(attribute_type::characteristic, prop_bits),
        handle(0u),
        uuid(characterstic_uuid)
    {
    }

    characteristic(uint32_t characterstic_uuid, uint16_t prop_bits) :
        decl(attribute_type::characteristic, prop_bits),
        handle(0u),
        uuid(characterstic_uuid)
    {
    }

    characteristic(characteristics characterstic_uuid, uint16_t prop_bits) :
        decl(attribute_type::characteristic, prop_bits),
        handle(0u),
        uuid(static_cast<uint16_t>(characterstic_uuid))
    {
    }

    declaration     decl;
    uint16_t        handle;
    att::uuid const uuid;

    /// @todo Provide a const version of this?
    virtual void *data_pointer() { return nullptr; }

    virtual att::length_t data_length()     const { return 0u; }
    virtual att::length_t data_length_max() const { return this->data_length(); }

    bool data_length_is_variable() const {
        return this->data_length() != this->data_length_max();
    }

    void descriptor_add(characteristic_base_descriptor &descriptor)
    {
        this->descriptor_list.push_back(descriptor);
    }

    // The list of descriptors associated with this characteristic.
    using descriptor_list_type =
        boost::intrusive::list<
            characteristic_base_descriptor,
            boost::intrusive::member_hook<characteristic_base_descriptor,
                                          boost::intrusive::list_member_hook<>,
                                          &characteristic_base_descriptor::hook_> >;

    descriptor_list_type descriptor_list;

    // Contain characteristics within Services.
    boost::intrusive::list_member_hook<> hook_;
};

} // namespace gatt
} // namespace ble
