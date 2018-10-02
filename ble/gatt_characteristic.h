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
#include <utility>
#include <boost/intrusive/list.hpp>

namespace ble
{
namespace gatt
{

class service;

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

    /**
     * Provide non-const access to the characteristic data.
     * Per Scott Meyers Effictive C++ Item 3, call the const version from the
     * non-const version.
     */
    void* data_pointer() {
        return const_cast<void*>(std::as_const(*this).data_pointer());
    }

    /**
     * Provide const access to the characteristic data.
     * @note This method is virtual and should be overridden by the data type
     * of the characteristic being exposed. The non-const does not and should not
     * need to be overridden by class which inherit from characteristic.
     */
    virtual void const* data_pointer() const { return nullptr; }

    virtual att::length_t data_length()     const { return 0u; }
    virtual att::length_t data_length_max() const { return this->data_length(); }

    bool data_length_is_variable() const {
        return this->data_length() != this->data_length_max();
    }

    void descriptor_add(characteristic_base_descriptor &descriptor)
    {
        this->descriptor_list.push_back(descriptor);
    }

private:
    // Contain characteristics within Services.
    using list_hook_type = boost::intrusive::list_member_hook<
        boost::intrusive::link_mode<boost::intrusive::auto_unlink>
        >;

    // The list of descriptors associated with this characteristic.
    using descriptor_list_type =
        boost::intrusive::list<
            characteristic_base_descriptor,
            boost::intrusive::constant_time_size<false>,
            boost::intrusive::member_hook<characteristic_base_descriptor,
                                          characteristic_base_descriptor::list_hook_type,
                                          &characteristic_base_descriptor::hook_>
        >;

    list_hook_type hook_;

    friend class service;

public:
    descriptor_list_type descriptor_list;
};

} // namespace gatt
} // namespace ble
