/**
 * @file uuid.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include <array>
#include <cstdint>
#include <cstddef>

#include "int_to_string.h"
#include "charconv.h"

namespace utility
{

/// Determine the number of char's required to represent a byte array
template<size_t count, char separator=':'>
static constexpr size_t array_uint8_conversion_size =
    (count * (2u + (separator != 0))) + 1u;

/**
 * Into the user supplied buffer [begin, end) output the ascii string
 * represetation of the byte array. If the template parameter separator is
 * zero (i.e. '\0' or 0x00) then no separator is placed in between bytes.
 * If the length of the user supplied buffer is sufficiently large a null
 * terminator will be appended.
 *
 * @tparam count The number of elements in the std::array container.
 * @param data   The instance of the std::array to represent.
 * @param begin  The user supplied char buffer pointer.
 * @param end    One past the end of the user supplied char buffer.
 */
template<size_t count, char separator=':'>
std::to_chars_result to_chars(std::array<uint8_t, count> const&  data,
                              char*                              begin,
                              char*                              end)
{
    using signed_size_t = typename std::make_signed<size_t>::type;
    constexpr signed_size_t const conv_size = array_uint8_conversion_size<count, separator>;

    ptrdiff_t const buf_len = end - begin;

    if (buf_len < conv_size)
    {
        return std::to_chars_result(begin, std::errc::value_too_large);
    }

    char *out_iter = begin;
    for (uint8_t const* i_array = data.begin(); i_array < data.end(); ++i_array)
    {
        *out_iter++ = nybble_to_char(*i_array >> 4u);
        *out_iter++ = nybble_to_char(*i_array >> 0u);
        if (separator != 0)
        {
            *out_iter++ = separator;
        }
    }

    --out_iter;                 // The last character was a separator.
    *out_iter++ = 0;            // Replace it with a null-terminator.

    return std::to_chars_result(out_iter);
}

} // namespace utility
