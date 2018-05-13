/**
 * @file tlv_encode.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Type/Length/Value encoding
 */

#include "tlv_encode.h"

namespace ble
{

void tlv_encode_push_back(advertising_data_t    &encoded,
                          void const            *data,
                          size_t                length)
{
    uint8_t const *data_ptr_8 = reinterpret_cast<uint8_t const*>(data);
    for ( ; length > 0; --length)
    {
        encoded.push_back(*data_ptr_8++);
    }
}

size_t tlv_encode(advertising_data_t    &encoded,
                  gap_type              type,
                  std::string const     &string)
{
    size_t const length = string.size();
    if (encoded.capacity() - encoded.size() < length + tlv_header_length)
    {
        return 0u;
    }

    auto const end_start = encoded.end();
    encoded.push_back(static_cast<uint8_t>(length + sizeof(gap_type)));
    encoded.push_back(static_cast<uint8_t>(type));
    tlv_encode_push_back(encoded, string.c_str(), length);

    return std::distance(end_start, encoded.end());
}

size_t tlv_encode(advertising_data_t    &encoded,
                  gap_type              type,
                  char const            *char_string)
{
    if (encoded.capacity() - encoded.size() < tlv_header_length)
    {
        return 0u;
    }

    auto const begin = encoded.end();
    encoded.push_back(0u);
    encoded.push_back(static_cast<uint8_t>(type));

    uint8_t const *data_ptr_8 = reinterpret_cast<uint8_t const*>(char_string);
    uint8_t const *const data_ptr_begin = data_ptr_8;
    while ((encoded.size() < encoded.capacity()) && (*data_ptr_8 != 0u))
    {
        encoded.push_back(*data_ptr_8++);
    }

    // The length is now known and can be set.
    *begin = (data_ptr_8 - data_ptr_begin) + sizeof(gap_type);

    return std::distance(begin, encoded.end());
}

size_t tlv_encode_address(advertising_data_t    &encoded,
                          bool                  address_is_random,
                          void const            *address_pointer)
{
    size_t const length = 7u;
    if (encoded.capacity() - encoded.size() < length + tlv_header_length)
    {
        return 0u;
    }

    auto const begin = encoded.end();
    encoded.push_back(static_cast<uint8_t>(length + sizeof(gap_type)));
    encoded.push_back(static_cast<uint8_t>(gap_type::device_dddress));
    encoded.push_back(static_cast<uint8_t>(address_is_random));
    tlv_encode_push_back(encoded, address_pointer, length - 1u);

    return std::distance(begin, encoded.end());
}

} // namespace ble
