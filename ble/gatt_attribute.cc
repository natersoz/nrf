/**
 * @file ble/gatt_attribute.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "ble/gatt_attribute.h"
#include "logger.h"
#include "project_assert.h"

#include <algorithm>
#include <utility>

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
    uint8_t *dst_beg = reinterpret_cast<uint8_t*>(this->data_pointer());

    if (dst_beg)
    {
        // The attribute data buffer end pointer.
        // Writing to this location or beyond is an overflow.
        uint8_t* const dst_end = dst_beg + this->data_length_max();

        // The attribute data iter locations which data will be written based
        // on the caller request.
        dst_beg += offset;
        uint8_t* const dst_last = std::min(dst_beg + length, dst_end);

        // Limit copy length to what is available.
        std::ptrdiff_t const copy_length = dst_last - dst_beg;
        if (copy_length > 0)
        {
            uint8_t const* data_in = reinterpret_cast<uint8_t const*>(data);
            std::copy(data_in, data_in + copy_length, dst_beg);
            return static_cast<att::length_t>(copy_length);
        }
        else
        {
            logger::instance().warn("attribute::write: %d <= 0", copy_length);
            return 0u;
        }
    }

    logger::instance().warn("attribute::write(0x%04x) to null",
                            this->decl.attribute_type);
    return 0u;
}

} // namespace gatt
} // namespace ble

