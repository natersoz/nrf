/**
 * @file ble/gatt_attribute.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "ble/gatt_attribute.h"
#include "logger.h"
#include "project_assert.h"

#include <algorithm>
#include <utility>
#include <cstring>

namespace ble
{
namespace gatt
{

void* attribute::data_pointer()
{
    void *ptr = const_cast<void*>(std::as_const(*this).data_pointer());
    if (ptr == nullptr)
    {
        logger::instance().warn(
            "attribute type: 0x%04x, ptr: 0x%p, data_pointer() returns null",
            static_cast<uint16_t>(this->decl.attribute_type), this);
    }
    return ptr;
}

ble::att::length_t attribute::write(ble::att::op_code   write_type,
                                    ble::att::length_t  offset,
                                    ble::att::length_t  length,
                                    void const*         data)
{
    (void) write_type;

    uint8_t *data_dst = reinterpret_cast<uint8_t*>(this->data_pointer());
    ASSERT(data_dst);

    if (data_dst)
    {
        data_dst += offset;

        uint8_t* data_end  = data_dst + length;
        uint8_t* data_max  = data_dst + this->data_length_max();

        data_end = std::min(data_end, data_max);

        std::ptrdiff_t const dest_length = data_end - data_dst;
        if (dest_length > 0)
        {
            memcpy(data_dst, data, dest_length);
            return static_cast<att::length_t>(dest_length);
        }
        else
        {
            logger::instance().warn("attribute::write: %d", dest_length);
            return 0u;
        }
    }

    logger::instance().warn("attribute::write to null");
    return 0u;
}

} // namespace gatt
} // namespace ble

