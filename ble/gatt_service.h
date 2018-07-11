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
#include "ble/gatt_uuids.h"
#include "ble/gatt_format.h"
#include "project_assert.h"

#include <algorithm>
#include <cstdint>
#include <boost/intrusive/list.hpp>

namespace ble
{
namespace gatt
{

class service_container;

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

    service()                           = delete;
    service(service const&)             = delete;
    service(service &&)                 = delete;
    service& operator=(service const&)  = delete;
    service& operator=(service&&)       = delete;

    /**
     * Instantiate a BLE GATT Service.
     *
     * @param service_uuid The service UUID
     * @param attr_type must be one of:
     *        - ble::gatt::declarations::primary_service
     *        - ble::gatt::declarations::secondary_service
     */
    service(att::uuid service_uuid, attribute_type attr_type) :
        decl(attr_type, properties::read),
        uuid(service_uuid)
    {
    }

    /**
     * Instantiate a BLE GATT Service.
     *
     * @param service_uuid The service UUID shortened to 16 or 32 bits.
     * @param attr_type must be one of:
     *        - ble::gatt::declarations::primary_service
     *        - ble::gatt::declarations::secondary_service
     */
    service(uint32_t service_uuid, attribute_type attr_type) :
        decl(attr_type, properties::read),
        uuid(service_uuid)
    {
    }

    /**
     * Instantiate a BLE GATT Service.
     *
     * @param service_uuid The 128-bit service UUID.
     * @param attr_type must be one of:
     *        - ble::gatt::declarations::primary_service
     *        - ble::gatt::declarations::secondary_service
     */
    service(services service_uuid, attribute_type attr_type) :
        decl(attr_type, properties::read),
        uuid(static_cast<uint16_t>(service_uuid))
    {
    }

    void characteristic_add(characteristic &char_to_add)
    {
        this->characteristic_list.push_back(char_to_add);
    }

    declaration     decl;
    att::uuid const uuid;

    bool operator==(service const& other) const {
        // Since the uuid has to be unique it can determine equivalence.
        return (this->uuid == other.uuid);
    }

    // The list of characteristics contained in the service.
    using characteristic_list_type =
        boost::intrusive::list<
            characteristic,
            boost::intrusive::member_hook<characteristic,
                                          boost::intrusive::list_member_hook<>,
                                          &characteristic::hook_> >;

    characteristic_list_type characteristic_list;

private:
    // So that services can be added to the service_container.
    boost::intrusive::list_member_hook<> hook_;

    friend class service_container;
};

/**
 * @class service_include
 * Include characteristics from one service within another.
 */
class service_include
{
public:
    virtual ~service_include()                          = default;

    service_include(service_include const&)             = delete;
    service_include(service_include &&)                 = delete;
    service_include& operator=(service_include const&)  = delete;
    service_include& operator=(service_include&&)       = delete;

    service_include():
        decl(attribute_type::include, properties::read),
        service_ptr(nullptr),
        characteristic_begin_ptr(nullptr),
        characteristic_last_ptr(nullptr)
    {
    }

    /**
     * Instantiate a BLE GATT Service Inclusion.
     *
     * @param service A reference to the service whose characteristics
     *                will be included.
     * @param characteristic_begin The first characteristic to include.
     * @param characteristic_last  The last characteristic to include.
     */
    service_include(service             &service,
                    characteristic      &characteristic_begin,
                    characteristic      &characteristic_last) :

        decl(attribute_type::include,   properties::read),
        service_ptr(&service),
        characteristic_begin_ptr(&characteristic_begin),
        characteristic_last_ptr(&characteristic_last)
    {
    }

    declaration     decl;
    service         *service_ptr;
    characteristic  *characteristic_begin_ptr;
    characteristic  *characteristic_last_ptr;
};

class service_container
{
private:
    // The list of descriptors associated with this characteristic.
    using service_list_type =
        boost::intrusive::list<
            service,
            boost::intrusive::member_hook<service,
                                          boost::intrusive::list_member_hook<>,
                                          &service::hook_>>;

public:
    ~service_container()                                    = default;

    service_container()                                     = default;
    service_container(service_container const&)             = delete;
    service_container(service_container &&)                 = delete;
    service_container& operator=(service_container const&)  = delete;
    service_container& operator=(service_container&&)       = delete;

    void add(service &service)
    {
        service_list_type::const_iterator iter =
            std::find(this->service_list_.begin(), this->service_list_.end(), service);

        // For now be paranoid and makes sure no duplicates are added.
        if (iter == this->service_list_.end())
        {
            this->service_list_.push_back(service);
        }
        else
        {
            ASSERT(0);
        }
    }

    void remove(service &service)
    {
        this->service_list_.remove(service);
    }

    service_list_type::iterator find(ble::att::uuid const &uuid)
    {
        service const to_find(uuid, attribute_type::primary_service);
        return std::find(this->service_list_.begin(), this->service_list_.end(), to_find);
    }

    service_list_type::const_iterator find(ble::att::uuid const &uuid) const
    {
        service const to_find(uuid, attribute_type::primary_service);
        return std::find(this->service_list_.begin(), this->service_list_.end(), to_find);
    }

    service_list_type::iterator find(ble::gatt::services uuid)
    {
        service const to_find(uuid, attribute_type::primary_service);
        return std::find(this->service_list_.begin(), this->service_list_.end(), to_find);
    }

    service_list_type::const_iterator find(ble::gatt::services uuid) const
    {
        service const to_find(uuid, attribute_type::primary_service);
        return std::find(this->service_list_.begin(), this->service_list_.end(), to_find);
    }

private:
    service_list_type service_list_;
};

} // namespace gatt
} // namespace ble
