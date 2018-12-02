/**
 * @file tlv_encode.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Type/Length/Value encoding
 */

#include "tlv_encode.h"

namespace ble
{

void tlv_encode_push_back(gap::advertising_data&  encoded,
                          void const*               data,
                          size_t                    length)
{
    uint8_t const *data_ptr_8 = reinterpret_cast<uint8_t const*>(data);
    for ( ; length > 0; --length)
    {
        encoded.push_back(*data_ptr_8++);
    }
}

size_t tlv_encode(gap::advertising_data&    encoded,
                  gap::type                 type,
                  std::string const&        string)
{
    size_t const length = string.size();
    if (encoded.capacity() - encoded.size() < length + tlv_header_length)
    {
        return 0u;
    }

    auto const end_start = encoded.end();
    encoded.push_back(static_cast<uint8_t>(length + sizeof(gap::type)));
    encoded.push_back(static_cast<uint8_t>(type));
    tlv_encode_push_back(encoded, string.c_str(), length);

    return std::distance(end_start, encoded.end());
}

size_t tlv_encode(gap::advertising_data&    encoded,
                  gap::type                 type,
                  char const*               char_string)
{
    if (encoded.capacity() - encoded.size() < tlv_header_length)
    {
        return 0u;
    }

    uint8_t* length_pointer = encoded.end_pointer();
    encoded.push_back(0u);
    encoded.push_back(static_cast<uint8_t>(type));

    uint8_t const *data_ptr_8 = reinterpret_cast<uint8_t const*>(char_string);
    uint8_t const *const data_ptr_begin = data_ptr_8;
    while ((encoded.size() < encoded.capacity()) && (*data_ptr_8 != 0u))
    {
        encoded.push_back(*data_ptr_8++);
    }

    // The length is now known and can be set.
    *length_pointer = (data_ptr_8 - data_ptr_begin) + sizeof(gap::type);

    return std::distance(length_pointer, encoded.end_pointer());
}

size_t tlv_encode_address(gap::advertising_data&    encoded,
                          bool                      address_is_random,
                          void const*               address_pointer)
{
    size_t const length = 7u;
    if (encoded.capacity() - encoded.size() < length + tlv_header_length)
    {
        return 0u;
    }

    auto const begin = encoded.end();
    encoded.push_back(static_cast<uint8_t>(length + sizeof(gap::type)));
    encoded.push_back(static_cast<uint8_t>(gap::type::device_dddress));
    encoded.push_back(static_cast<uint8_t>(address_is_random));
    tlv_encode_push_back(encoded, address_pointer, length - 1u);

    return std::distance(begin, encoded.end());
}

} // namespace ble
