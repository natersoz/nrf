/**
 * @file ble/gatt_characteristic.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "ble/gatt_attribute.h"
#include "ble/gatt_descriptors.h"
#include "ble/gatt_enum_types.h"
#include "ble/gatt_format.h"

#include <cstdint>
#include <utility>

namespace ble
{
namespace gatt
{

class service;

/**
 * @struct characteristic
 * A container of the Bluetooth LE characteristic:
 * - Inherits from ble::gatt::attribute, so it contains the declaration:
 *   properties and declaration handle.
 * - The uuid:         which may be 16 or 128-bit.
 * - The value handle: when accessed points to the attribute data accessors.
 *   The value handle is associated with the member uuid, not the declaration.
 * - A descriptor list: Which contains a list of BLE descriptors (if any).
 * - A pointer to the containing service.
 */
struct characteristic: public attribute
{
    virtual ~characteristic() override                  = default;

    characteristic(characteristic const&)               = delete;
    characteristic(characteristic &&)                   = delete;
    characteristic& operator=(characteristic const&)    = delete;
    characteristic& operator=(characteristic&&)         = delete;

    /** A characteristic which is uninitialized. The uuid is zero filled */
    characteristic() :
        attribute(attribute_type::characteristic, 0u),
        uuid(),
        value_handle(ble::att::handle_invalid),
        service_(nullptr)
    {
    }

    /** A characteristic created by standard uuid. */
    characteristic(att::uuid const& att_uuid, uint16_t prop_bits) :
        attribute(attribute_type::characteristic, prop_bits),
        uuid(att_uuid),
        value_handle(ble::att::handle_invalid),
        service_(nullptr)
    {
    }

    /** A characteristic create by a 16-bit or 32-bit value. */
    characteristic(uint32_t uuid_32, uint16_t prop_bits) :
        attribute(attribute_type::characteristic, prop_bits),
        uuid(uuid_32),
        value_handle(ble::att::handle_invalid),
        service_(nullptr)
    {
    }

    /** A standard Bluetooth SIG characteristic. */
    characteristic(characteristic_type uuid_ble_sig, uint16_t prop_bits) :
        attribute(attribute_type::characteristic, prop_bits),
        uuid(static_cast<uint16_t>(uuid_ble_sig)),
        value_handle(ble::att::handle_invalid),
        service_(nullptr)
    {
    }

    /** @return The service which contains the characteristic. */
    ble::gatt::service const* service() const { return this->service_; }
    ble::gatt::service*       service()       { return this->service_; }

    void descriptor_add(descriptor_base &descriptor) {
        this->descriptor_list.push_back(descriptor);
    }

    /**
     * Get the attribute contained within the characteristic based on the
     * attribute handle.
     *
     * @note In the GATTS case Nordic's softdevice handles the case of the
     *       declaration handle. In the case of other stacks? TBD ...
     *
     * @param handle The attribute handle of interest.
     *
     * @return attribute const* The attribute associated with the handle;
     *                          The characteristic value handle included.
     */
    attribute const* find_attribute(uint16_t handle) const
    {
        if (this->value_handle == handle)
        {
            return this;
        }

        for (attribute const &descriptor : this->descriptor_list)
        {
            if (descriptor.decl.handle == handle)
            {
                return &descriptor;
            }
        }

        return nullptr;
    }

    attribute* find_attribute(uint16_t handle)
    {
        return const_cast<ble::gatt::attribute*>(
            std::as_const(*this).find_attribute(handle)
            );
    }

    ble::att::handle_range hande_range() const
    {
        if (this->descriptor_list.empty())
        {
            return ble::att::handle_range(this->decl.handle,
                                          this->value_handle);
        }
        else
        {
            auto descriptor_iter = this->descriptor_list.end();
            --descriptor_iter;
            return ble::att::handle_range(this->decl.handle,
                                          (*descriptor_iter).decl.handle);
        }
    }

    att::uuid uuid;
    uint16_t  value_handle;
    list_type descriptor_list;

private:
    /**
     * When adding a characteristic to a service allow the service to set
     * itself as the service which contains the characteristic.
     */
    ble::gatt::service* service_;
    friend class ble::gatt::service;
};

} // namespace gatt
} // namespace ble
