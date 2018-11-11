/**
 * @file ble/gatt_service_container.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "gatt_service_container.h"
#include "project_assert.h"

#include <algorithm>

namespace ble
{
namespace gatt
{

service const* service_container::find_service(ble::att::uuid const &uuid) const
{
    service_list_type::const_iterator iter =
        std::find_if(this->begin(), this->end(),
                     [&uuid] (service const& service_in_list) {
                         return uuid == service_in_list.uuid; });

    return (iter == this->end())? nullptr : &*iter;
}

/// @todo this should be implemented as:
/// return static_cast<service_container *>(this)->find_service(uuid);
service* service_container::find_service(ble::att::uuid const &uuid)
{
    service_list_type::iterator iter =
        std::find_if(this->begin(), this->end(),
                     [&uuid] (service& service_in_list) {
                         return uuid == service_in_list.uuid; });

    return (iter == this->end())? nullptr : &*iter;
}

service const* service_container::find_service(ble::gatt::services uuid_16) const
{
    ble::att::uuid const uuid(static_cast<uint16_t>(uuid_16));
    return this->find_service(uuid);
}

/// @todo this should be implemented as:
/// return static_cast<service_container *>(this)->find_service(uuid_16);
service* service_container::find_service(ble::gatt::services uuid_16)
{
    ble::att::uuid const uuid(static_cast<uint16_t>(uuid_16));
    return this->find_service(uuid);
}

characteristic const* service_container::find_characteristic(uint16_t handle) const
{
    for (service const& service : *this)
    {
        attribute const* attribute = service.find_attribute(handle);
        if (attribute)
        {
            return reinterpret_cast<characteristic const*>(attribute);
        }
    }

    return nullptr;
}

/// @todo this should be implemented as:
/// return static_cast<service_container *>(this)->find_characteristic(handle);
characteristic* service_container::find_characteristic(uint16_t handle)
{
    for (service& service : *this)
    {
        attribute* attribute = service.find_attribute(handle);
        if (attribute)
        {
            return reinterpret_cast<characteristic*>(attribute);
        }
    }

    return nullptr;
}

characteristic const* service_container::find_characteristic(ble::att::uuid const &uuid) const
{
    for (service const& service : *this)
    {
        characteristic const* charateristic = service.find_characteristic(uuid);
        if (charateristic)
        {
            return charateristic;
        }
    }

    return nullptr;
}

/// @todo this should be implemented as:
/// return static_cast<service_container *>(this)->find_characteristic(uuid);
characteristic* service_container::find_characteristic(ble::att::uuid const &uuid)
{
    for (service& service : *this)
    {
        characteristic* charateristic = service.find_characteristic(uuid);
        if (charateristic)
        {
            return charateristic;
        }
    }

    return nullptr;
}

characteristic const* service_container::find_characteristic(ble::gatt::characteristics const uuid_16) const
{
    ble::att::uuid const uuid(static_cast<uint16_t>(uuid_16));
    return this->find_characteristic(uuid);
}

/// @todo this should be implemented as:
/// return static_cast<service_container *>(this)->find_characteristic(uuid_16);
characteristic* service_container::find_characteristic(ble::gatt::characteristics const uuid_16)
{
    ble::att::uuid const uuid(static_cast<uint16_t>(uuid_16));
    return this->find_characteristic(uuid);
}

} // namespace gatt
} // namespace ble


