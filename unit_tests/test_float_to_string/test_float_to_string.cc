/**
 * @file test_float_to_string.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "float_to_string.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <cassert>

// static constexpr bool debug_print = true;

inline std::ostream &operator<<(std::ostream &os, int8_t value)
{
    return os << static_cast<int>(value);
}

inline std::ostream &operator<<(std::ostream &os, uint8_t value)
{
    return os << static_cast<unsigned int>(value);
}

static void print_double(double value)
{
    char hex_conv_buffer[double_hex_conversion_size];
    double_to_hex(hex_conv_buffer, sizeof(hex_conv_buffer), value);

    uint64_t const u64_float = *reinterpret_cast<uint64_t*>(&value);
    double_parts const parts = double_extract_parts(value);

    std::cout << "-----------------------------" << std::endl;
    std::cout << "doub: " << value << std::endl;
    std::cout << "conv: " << hex_conv_buffer << std::endl;
    std::cout << "strl: " << std::dec << strnlen(hex_conv_buffer,
                                                 sizeof(hex_conv_buffer))
              << std::endl;

    std::cout << "sign: " << parts.sign;
    std::cout << ", exp: " <<  std::hex << std::setfill('0') << std::setw(4u)
              << parts.exponent;
    std::cout << ", mant: " <<  std::hex << std::setfill('0') << std::setw(4u)
              << parts.mantissa;
    std::cout << std::endl;

    std::cout << "raw : 0x" << std::hex << std::setfill('0') << std::setw(16u)
              << u64_float << std::endl;
}

int main()
{
    int result = 0;

    std::cout << "size: " << sizeof(double) << std::endl;

    print_double(0.0);
    print_double(0.1);
    print_double(0.2);
    print_double(1.0);
    print_double(-1.0);

#if 0
    for (double value = 0.0; value < 11.0; value += 0.1)
    {
        print_double(value);
    }
#endif

    std::cout << "--- Tests Pass" << std::endl;
    return result;
}
