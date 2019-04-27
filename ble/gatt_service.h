/**
 * @file ble/gatt_service.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "ble/uuid.h"
#include "ble/att.h"
#include "ble/gatt_declaration.h"
#include "ble/gatt_descriptors.h"
#include "ble/gatt_characteristic.h"
#include "ble/gatt_enum_types.h"

#include <algorithm>
#include <cstdint>
#include <boost/intrusive/list.hpp>

namespace ble
{
namespace profile { class connectable; }
namespace gatt
{

/**
 * @class ble::gatt::service
 *
 * @see Townsend, Kevin; Cufí, Carles; Akiba; Davidson, Robert.
 * Getting Started with Bluetooth Low Energy.
 * Table 4-2. Service Declaration attribute
 *
 * Each service contains:
 * - A declaration which contains:
 *   - A set of properties which is set to read-only.
 *   - A handle.
 *   - A service type UUID which is one of:
 *     - 0x2800, attribute_types::primary_service.
 *     - 0x2801, attribute_types::secondary_service.
 * - A service UUID, either BLE defined type of 16 or 32 bits or
 *   a user specified type of 128 bits.
 * - A container of characteristics.
 */
class service
{
public:
    virtual ~service()                  = default;

    service(service const&)             = delete;
    service(service &&)                 = delete;
    service& operator=(service const&)  = delete;
    service& operator=(service&&)       = delete;

    /**
     * Create an uninitialized service:
     * A service whose intended purpose has not yet been determined.
     * Init the service type is primary; it can be changed later.
     */
    service():
        decl(ble::gatt::attribute_type::primary_service, 0u),
        uuid(),
        connectable_(nullptr)
    {
    }

    /**
     * Instantiate a BLE GATT Service.
     *
     * @param service_uuid The service UUID
     * @param attr_type must be one of:
     *        - ble::gatt::attribute_type::primary_service
     *        - ble::gatt::attribute_type::secondary_service
     */
    service(att::uuid service_uuid, attribute_type attr_type) :
        decl(attr_type, properties::read),
        uuid(service_uuid),
        connectable_(nullptr)
    {
    }

    /**
     * Instantiate a BLE GATT Service.
     *
     * @param service_uuid The service UUID shortened to 16 or 32 bits.
     * @param attr_type must be one of:
     *        - ble::gatt::attribute_type::primary_service
     *        - ble::gatt::attribute_type::secondary_service
     */
    service(uint32_t service_uuid, attribute_type attr_type) :
        decl(attr_type, properties::read),
        uuid(service_uuid),
        connectable_(nullptr)
    {
    }

    /**
     * Instantiate a BLE GATT Service.
     *
     * @param service_uuid The 128-bit service UUID.
     * @param attr_type must be one of:
     *        - ble::gatt::attribute_type::primary_service
     *        - ble::gatt::attribute_type::secondary_service
     */
    service(service_type service_uuid, attribute_type attr_type) :
        decl(attr_type, properties::read),
        uuid(static_cast<uint16_t>(service_uuid)),
        connectable_(nullptr)
    {
    }

    ble::profile::connectable const* connectable() const {
        return this->connectable_;
    }

    ble::profile::connectable* connectable() {
        return this->connectable_;
    }

    void characteristic_add(characteristic &characteristic_to_add);

    attribute const* find_attribute(uint16_t handle) const;
    attribute*       find_attribute(uint16_t handle);

    characteristic const* find_characteristic(ble::att::uuid const& chr_uuid) const;
    characteristic*       find_characteristic(ble::att::uuid const& chr_uuid);

    declaration decl;
    att::uuid   uuid;

    attribute::list_type characteristic_list;

    using list_hook_type = boost::intrusive::list_member_hook<
        boost::intrusive::link_mode<boost::intrusive::auto_unlink>
        >;

    list_hook_type hook;

private:
    ble::profile::connectable *connectable_;

    friend class ble::profile::connectable;
};

} // namespace gatt
} // namespace ble
