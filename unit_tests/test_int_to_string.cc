/**
 * @file test_int_to_string.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "int_to_string.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <cassert>

static constexpr bool debug_print = false;

template<typename int_value>
static bool test_hex_conversion(int_value  test_value,
                                char const *conv_buffer,
                                size_t     conv_width,
                                char       fill_value)
{
    bool result = true;

    std::ostringstream os_str;
    os_str << std::hex << std::setfill(fill_value) << std::setw(conv_width) << test_value;

    std::string conv_str(conv_buffer);

    if (conv_str != os_str.str())
    {
        std::cout << "'" << os_str.str() << "'" << " != " << "'" << conv_str << "'" << std::endl;
        assert(os_str.str() == conv_str);
        result = false;
    }

    return result;
}

template<typename int_value>
static bool test_dec_conversion(int_value  test_value,
                                char const *conv_buffer,
                                size_t     conv_width,
                                char       fill_value,
                                bool       prefix_plus)
{
    bool result = true;

    std::ostringstream os_str;
    os_str << std::dec << std::setfill(fill_value) << std::setw(conv_width) << test_value;

    std::string conv_str(conv_buffer);

    if (conv_str != os_str.str())
    {
        std::cout << "'" << os_str.str() << "'" << " != " << "'" << conv_str << "'" << std::endl;
        assert(os_str.str() == conv_str);;
        result = false;
    }

    return result;
}

int main()
{
    int result = 0;

    int test_int_values[] =
    {
        0x01234567,
        0x19abcdef,
        0x13579bdf,
        0x0db97521,

         1234567890,
        -1234567890,
        -1
    };

    unsigned int test_uint_values[] =
    {
        0x01234567u,
        0x89abcdefu,
        0x13579bdfu,
        0xfdb97521u,
    };

    // --- Test Hex conversions:

    for (auto test_value : test_int_values)
    {
        size_t const conv_width = hex_conversion_size<decltype(test_value)> - 1u;
        char   const fill_value = '0';

        char conv_buffer[hex_conversion_size<decltype(test_value)>];
        size_t const conv_buffer_length = sizeof(conv_buffer);
        size_t const conv_length = int_to_hex(conv_buffer, conv_buffer_length, test_value, conv_width);
        (void) conv_length;

        if (debug_print)
        {
            std::cout << "0x" << std::hex << std::setfill('0') << std::setw(conv_width) << test_value << ", "
                      << "0x" << conv_buffer << std::endl;
        }

        test_hex_conversion(test_value, conv_buffer, conv_width, fill_value);
    }

    for (auto test_value : test_uint_values)
    {
        size_t const conv_width = hex_conversion_size<decltype(test_value)> - 1u;
        char   const fill_value = '0';

        char conv_buffer[hex_conversion_size<decltype(test_value)>];
        size_t const conv_buffer_length = sizeof(conv_buffer);
        size_t const conv_length = int_to_hex(conv_buffer, conv_buffer_length, test_value, conv_width);
        (void) conv_length;

        if (debug_print)
        {
            std::cout << "0x" << std::hex << std::setfill('0') << std::setw(conv_width) << test_value << ", "
                      << "0x" << conv_buffer << std::endl;
        }

        test_hex_conversion(test_value, conv_buffer, conv_width, fill_value);
    }

    // --- Test Decimal conversions:

    for (auto test_value : test_int_values)
    {
        size_t const conv_width  = dec_conversion_size<decltype(test_value)> - 1u;
        char   const fill_value  = ' ';
        bool   const prefix_plus = false;

        char conv_buffer[dec_conversion_size<decltype(test_value)>];
        size_t const conv_buffer_length = sizeof(conv_buffer);
        size_t const conv_length = int_to_dec(conv_buffer, conv_buffer_length, test_value,
                                              conv_width, fill_value, prefix_plus);
        (void) conv_length;

        if (debug_print)
        {
            std::cout << std::dec << std::setfill(fill_value) << std::setw(conv_width)
                      << test_value << ", "
                      << "'" << conv_buffer << "'" << std::endl;
        }

        test_dec_conversion(test_value, conv_buffer, conv_width, fill_value, prefix_plus);
    }

    for (auto test_value : test_uint_values)
    {
        size_t const conv_width  = dec_conversion_size<decltype(test_value)> - 1u;
        char   const fill_value  = ' ';
        bool   const prefix_plus = false;

        char conv_buffer[dec_conversion_size<decltype(test_value)>];
        size_t const conv_buffer_length = sizeof(conv_buffer);
        size_t const conv_length = int_to_dec(conv_buffer, conv_buffer_length, test_value,
                                              conv_width, fill_value, prefix_plus);
        (void) conv_length;

        if (debug_print)
        {
            std::cout << std::dec << std::setfill(fill_value) << std::setw(conv_width)
                      << test_value << ", "
                      << "'" << conv_buffer << "'" << std::endl;
        }

        test_dec_conversion(test_value, conv_buffer, conv_width, fill_value, prefix_plus);
    }

    std::cout << "--- Tests Pass" << std::endl;
    return result;
}
