/**
 * @file ble/gatt_characteristic.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "ble/gatt_attribute.h"
#include "ble/gatt_descriptors.h"
#include "ble/gatt_uuids.h"
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
 */
struct characteristic: public attribute
{
    virtual ~characteristic() override                  = default;

    characteristic()                                    = delete;
    characteristic(characteristic const&)               = delete;
    characteristic(characteristic &&)                   = delete;
    characteristic& operator=(characteristic const&)    = delete;
    characteristic& operator=(characteristic&&)         = delete;

    characteristic(att::uuid att_uuid, uint16_t prop_bits) :
        attribute(attribute_type::characteristic, prop_bits),
        uuid(att_uuid),
        service_(nullptr)
    {
    }

    characteristic(uint32_t uuid_32, uint16_t prop_bits) :
        attribute(attribute_type::characteristic, prop_bits),
        uuid(uuid_32),
        service_(nullptr)
    {
    }

    characteristic(characteristics uuid_char, uint16_t prop_bits) :
        attribute(attribute_type::characteristic, prop_bits),
        uuid(static_cast<uint16_t>(uuid_char)),
        service_(nullptr)
    {
    }

    ble::gatt::service const* service() const { return this->service_; }
    ble::gatt::service*       service()       { return this->service_; }

    void descriptor_add(characteristic_base_descriptor &descriptor) {
        this->descriptor_list.push_back(descriptor);
    }

    attribute const* find_attribute(uint16_t handle) const;
    attribute*       find_attribute(uint16_t handle);

    att::uuid               uuid;
    ble::gatt::service*     service_;   ///< The service owning this characteristic.

    list_type               descriptor_list;
};

} // namespace gatt
} // namespace ble
