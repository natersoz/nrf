/**
 * @file ble/gatt_characteristic.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "ble/gatt_characteristic.h"

namespace ble
{
namespace gatt
{

attribute const* characteristic::find_attribute(uint16_t handle) const
{
    if (this->decl.handle == handle)
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

attribute* characteristic::find_attribute(uint16_t handle)
{
    if (this->decl.handle == handle)
    {
        return this;
    }

    for (attribute &descriptor : this->descriptor_list)
    {
        if (descriptor.decl.handle == handle)
        {
            return &descriptor;
        }
    }

    return nullptr;
}

} // namespace gatt
} // namespace ble
