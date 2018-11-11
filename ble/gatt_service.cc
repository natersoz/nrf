/**
 * @file ble/gatt_service.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "ble/gatt_service.h"
#include "project_assert.h"
#include "logger.h"

#include <algorithm>
#include <type_traits>

/**
 * @note Having difficulty implementing a single find function that performs
 * both const and non-const operations.
 *
 * When casting service::characteristic_list_type::const_iterator to
 * its non-const counterpart, error: no conversion available.
 */

namespace ble
{
namespace gatt
{

void service::characteristic_add(characteristic &characteristic_to_add)
{
    characteristic_to_add.service_ = this;
    this->characteristic_list.push_back(characteristic_to_add);
}

attribute const* service::find_attribute(uint16_t handle) const
{
    for (attribute const& attr : this->characteristic_list)
    {
        characteristic const& chr = reinterpret_cast<characteristic const&>(attr);
        attribute const* attr_found = chr.find_attribute(handle);
        if (attr_found) { return attr_found; }
    }

    return nullptr;
}

attribute* service::find_attribute(uint16_t handle)
{
    for (attribute& attr : this->characteristic_list)
    {
        characteristic& chr = reinterpret_cast<characteristic&>(attr);
        attribute* attr_found = chr.find_attribute(handle);
        if (attr_found) { return attr_found; }
    }

    return nullptr;
}

characteristic const* service::find_characteristic(ble::att::uuid const& uuid) const
{
    for (attribute const& attr : this->characteristic_list)
    {
        characteristic const& chr = reinterpret_cast<characteristic const&>(attr);
        if (chr.uuid == uuid) { return &chr; }
    }

    return nullptr;
}

characteristic* service::find_characteristic(ble::att::uuid const& uuid)
{
    for (attribute& attr : this->characteristic_list)
    {
        characteristic& chr = reinterpret_cast<characteristic&>(attr);
        if (chr.uuid == uuid) { return &chr; }
    }

    return nullptr;
}

} // namespace gatt
} // namespace ble

