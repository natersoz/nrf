/**
 * @file uuid.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "ble/uuid.h"
#include "int_to_string.h"
#include <array>
#include <type_traits>

namespace ble
{
namespace att
{

// Provide the instance for the ble::uuid::base.
constexpr boost::uuids::uuid const uuid::base;

uuid::uuid():
    boost::uuids::uuid {{
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    }}
{
}

uuid::uuid(boost::uuids::uuid const &other):
    boost::uuids::uuid(other)
{
}

uuid::uuid(uint32_t uuid_32):
    boost::uuids::uuid {{
        static_cast<uint8_t>(uuid_32 >> 24u),
        static_cast<uint8_t>(uuid_32 >> 16u),
        static_cast<uint8_t>(uuid_32 >>  8u),
        static_cast<uint8_t>(uuid_32 >>  0u),
        base.data[base_offset +  0u],
        base.data[base_offset +  1u],
        base.data[base_offset +  2u],
        base.data[base_offset +  3u],
        base.data[base_offset +  4u],
        base.data[base_offset +  5u],
        base.data[base_offset +  6u],
        base.data[base_offset +  7u],
        base.data[base_offset +  8u],
        base.data[base_offset +  9u],
        base.data[base_offset + 10u],
        base.data[base_offset + 11u],
    }}
{
}

bool uuid::is_ble() const
{
    auto this_iter = this->begin() + uuid::base_offset;
    auto base_iter = base.begin()  + uuid::base_offset;

    while (this_iter < this->end())
    {
        bool const check = (*this_iter++ == *base_iter++);
        if (not check)
        {
            return false;
        }
    }

    return true;
}

uint16_t uuid::get_u16() const
{
    uint16_t value = this->data[2];
    value <<= 8u;
    value |= this->data[3];

    return value;
}

uint32_t uuid::get_u32() const
{
    uint16_t value = this->data[0];
    value <<= 8u;
    value |= this->data[1];
    value <<= 8u;
    value |= this->data[2];
    value <<= 8u;
    value |= this->data[3];

    return value;
}

uuid uuid::reverse() const
{
    uuid uuid_rev;

    size_t const index_end = sizeof(this->data);
    size_t       index_rev = index_end - 1u;

    for (size_t index_fwd = 0u; index_fwd < index_end; ++index_fwd, --index_rev)
    {
        uuid_rev.data[index_rev] = this->data[index_fwd];
    }

    return uuid_rev;
}

static void byte_conv(char *out_ptr, uint8_t const *data_ptr, size_t length)
{
    uint8_t const *data_end = data_ptr + length;
    for (; data_ptr < data_end; ++data_ptr)
    {
        *out_ptr++ = nybble_to_char(*data_ptr >> 4u);
        *out_ptr++ = nybble_to_char(*data_ptr >> 0u);
    }
}

std::to_chars_result uuid::to_chars(char *begin, char *end) const
{
    using signed_size_t = std::make_signed<size_t>::type;
    std::to_chars_result result;

    ptrdiff_t buffer_length = end - begin;
    if (buffer_length < static_cast<signed_size_t>(conversion_length))
    {
        result.ec = std::errc::value_too_large;
        result.ptr = begin;
        return result;
    }

    // The form is: "00000000-0000-1000-8000-00805F9B34FB"
    uint8_t const *data_ptr = this->data;
    char          *out_ptr  = begin;
    std::array<uint8_t, 5u> const conv_len_segments = {{ 4u, 2u, 2u, 2u, 6u }};

    for (auto &conv_length : conv_len_segments)
    {
        byte_conv(out_ptr, data_ptr, conv_length);
        data_ptr  += conv_length;
        out_ptr   += conv_length * 2u;
        *out_ptr++ = '-';
    }

    *(--out_ptr) = 0u;
    result.ptr = out_ptr;
    return result;
}

} // namespace att
} // namespace ble
