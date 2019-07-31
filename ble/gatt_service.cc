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
        characteristic const& chr = static_cast<characteristic const&>(attr);
        attribute const* attr_found = chr.find_attribute(handle);
        if (attr_found) { return attr_found; }
    }

    return nullptr;
}

attribute* service::find_attribute(uint16_t handle)
{
    return const_cast<ble::gatt::attribute*>(
        std::as_const(*this).find_attribute(handle)
        );
}

characteristic const* service::find_characteristic(ble::att::uuid const& chr_uuid) const
{
    for (attribute const& attr : this->characteristic_list)
    {
        characteristic const& chr = static_cast<characteristic const&>(attr);
        if (chr.uuid == chr_uuid) { return &chr; }
    }

    return nullptr;
}

characteristic* service::find_characteristic(ble::att::uuid const& chr_uuid)
{
    return const_cast<ble::gatt::characteristic*>(
        std::as_const(*this).find_characteristic(chr_uuid)
        );
}

} // namespace gatt
} // namespace ble

