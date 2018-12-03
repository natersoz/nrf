/**
 * @file ltv_encode.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * BLE advertising Length/Type/Value (LTV) encoding.
 */

#pragma once

#include "ble/gap_advertising_data.h"
#include "ble/gap_types.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <boost/endian/conversion.hpp>

namespace ble
{
namespace gap
{

/**
 * The number of bytes required to carry the type and length values.
 * This plus the size of the payload will be the resulting encoded length.
 */
static constexpr size_t const ltv_header_length = 2u * sizeof(uint8_t);

void ltv_encode_push_back(gap::advertising_data&    encoded,
                          void const*               data,
                          size_t                    length);

size_t ltv_encode(gap::advertising_data&    encoded,
                  gap::type                 type,
                  std::string const&        string);

size_t ltv_encode(gap::advertising_data&    encoded,
                  gap::type                 type,
                  char const*               char_string);

size_t ltv_encode_address(gap::advertising_data&    encoded,
                          bool                      address_is_random,
                          void const*               address_pointer);

template <typename data_type>
std::size_t ltv_encode(gap::advertising_data&       encoded,
                       gap::type                    type,
                       data_type const*             data,
                       std::size_t                  data_length)
{
    std::size_t const length = data_length * sizeof(data_type);
    if (encoded.capacity() - encoded.size() < length + ltv_header_length)
    {
        return 0u;
    }

    auto const begin_pointer = encoded.end_pointer();
    encoded.push_back(static_cast<uint8_t>(length + sizeof(gap::type)));
    encoded.push_back(static_cast<uint8_t>(type));

    for (std::size_t index = 0; index < data_length; ++index)
    {
        auto data_little = boost::endian::native_to_little(data[index]);
        ltv_encode_push_back(encoded, &data_little, sizeof(data_little));
    }

    return std::distance(begin_pointer, encoded.end_pointer());
}

template <typename data_type>
std::size_t ltv_encode(gap::advertising_data&       encoded,
                       gap::type                    type,
                       data_type const&             data)
{
    std::size_t const length = sizeof(data_type);
    if (encoded.capacity() - encoded.size() < length + ltv_header_length)
    {
        return 0u;
    }

    auto const begin_pointer = encoded.end_pointer();
    encoded.push_back(static_cast<uint8_t>(length + sizeof(gap::type)));
    encoded.push_back(static_cast<uint8_t>(type));

    auto data_little = boost::endian::native_to_little(data);
    ltv_encode_push_back(encoded, &data_little, sizeof(data_little));

    return std::distance(begin_pointer, encoded.end_pointer());
}

#if 0
template <typename data_type>
size_t ltv_encode(ble::advertising_data_t &encoded, gap::typetype, data_type const &container)
{
    size_t const length = container.size() * sizeof(data_type);
    if (encoded.capacity() - encoded.size() < length + ltv_header_length)
    {
        return 0u;
    }

    auto const begin = encoded.end();
    encoded.push_back(static_cast<uint8_t>(type));
    encoded.push_back(static_cast<uint8_t>(length));

    for (auto const &data : container)
    {
        auto data_little = boost::endian::native_to_little(data);
        ltv_encode_push_back(encoded, &data_little, sizeof(data_little));
    }

    return std::distance(begin, encoded.end());
}
#endif

} // namespace gap
} // namespace ble
