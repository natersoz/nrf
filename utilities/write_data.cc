/**
 * @file write_data.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "write_data.h"
#include "int_to_string.h"

#include <cctype>
#include <type_traits>
#include <algorithm>

using signed_size_t = typename std::make_signed<size_t>::type;

static constexpr char const new_line = '\n';

static size_t write_byte(output_stream &os, uint8_t byte_value)
{
    char buffer[2u];
    buffer[0u] = nybble_to_char(byte_value >> 4u);
    buffer[1u] = nybble_to_char(byte_value >> 0u);
    return os.write(buffer, sizeof(buffer));
}

namespace write_data
{

/*
 * 00000000 00000000 00000000 00000000  .@#^Dccc--xoit.
 * 00000000 00000000 00000000 00000000  ........This is
 */
static size_t write_data_line(output_stream&  os,
                              uint8_t const   *data,
                              size_t          length,
                              size_t          bytes_per_line,
                              bool            fill_line)
{
    char const space = ' ';

    size_t n_written = 0u;
    for (size_t iter = 0u; iter < length; ++iter, ++data)
    {
        if ((iter % 4u == 0u) && (iter > 0u))
        {
            n_written += os.write(&space, sizeof(space));
        }
        n_written += write_byte(os, *data);
    }

    if (fill_line)
    {
        for (size_t iter = length; iter < bytes_per_line; ++iter)
        {
            if ((iter % 4u == 0u) && (iter > 0u))
            {
                n_written += os.write(&space, sizeof(space));
            }

            char const spaces[] = {space, space};
            n_written += os.write(&spaces, sizeof(spaces));
        }
    }

    return n_written;
}

static size_t write_char_data_line(output_stream&   os,
                                   uint8_t const    *data,
                                   size_t           length)
{
    size_t n_written = 0u;
    for (size_t iter = 0u; iter < length; ++iter, ++data)
    {
        if (std::isprint(*data))
        {
            n_written += os.write(data, sizeof(*data));
        }
        else
        {
            char const non_printable = '.';
            n_written += os.write(&non_printable, sizeof(non_printable));
        }
    }

    return n_written;
}

size_t write_data(output_stream &os,
                  void const    *data,
                  size_t        length,
                  bool          char_data,
                  data_prefix   prefix)
{
    size_t n_write = 0u;
    uint8_t const *data_ptr = static_cast<uint8_t const *>(data);

    // The number of bytes to write on a line.
    size_t const bytes_per_line = 16u;

    for (size_t iter = 0u; iter < length; iter     += bytes_per_line,
                                          data_ptr += bytes_per_line)
    {
        size_t const bytes_remaining = length - iter;
        size_t const bytes_to_write = std::min(bytes_remaining, bytes_per_line);

        switch (prefix)
        {
        case data_prefix::none:
            break;
        case data_prefix::address:
            {
                uintptr_t const address = reinterpret_cast<uintptr_t>(&data_ptr[iter]);
                char buffer[hex_conversion_size<uintptr_t>];
                size_t const digits = sizeof(address) * 2u;
                size_t hex_len = int_to_hex(buffer, digits, address, digits);
                n_write += os.write(buffer, hex_len);

                char const colon[] = ": ";
                n_write += os.write(&colon, sizeof(colon) - 1u);
            }
            break;
        case data_prefix::index:
            if (true)
            {
                using size_type_t = decltype(iter);
                char buffer[hex_conversion_size<size_type_t>];
                size_t const digits = sizeof(iter) * 2u;
                size_t hex_len = int_to_hex(buffer, digits, iter, digits);
                n_write += os.write(buffer, hex_len);

                char const colon[] = ": ";
                n_write += os.write(&colon, sizeof(colon) - 1u);
            }
            break;
        }

        n_write += write_data_line(os,
                                   data_ptr,
                                   bytes_to_write,
                                   bytes_per_line,
                                   char_data);

        if (char_data)
        {
            char const space = ' ';
            n_write += os.write(&space, sizeof(space));
            n_write += write_char_data_line(os, data_ptr, bytes_to_write);
        }

        n_write += os.write(&new_line, sizeof(new_line));
    }

    return n_write;
}

}   // namespace write_data
