/**
 * @file float_to_string.h
 */

#pragma once

#include <cstdint>
#include <iostream>
#include <type_traits>

#include "int_to_string.h"
#include "bit_manip.h"

/**
 * IEEE-754 floating point format:
 * half-float:      sign[ 16] exponent[ 15: 10] mantissa[  9:0]
 * float:           sign[ 32] exponent[ 30: 23] mantissa[ 22:0]
 * double:          sign[ 63] exponent[ 62: 52] mantissa[ 51:0]
 * long double:     sign[127] exponent[126:112] mantissa[111:0]
 *
 * Only doubles and long doubles need apply.
 * Actually, for now, we're just doing double.
 */

/**
 * To convert a double to hex:
 * bytes    reason
 * -----    --------------------------
 * 1        for leading signage.
 * 7 * 2    mantissa hex conversion digits.
 * 1        'p' separator.
 * 9        signed 16-bit conversion.
 * 1        zero terminator.
 * -----
 * 29       bytes total
 */
static constexpr size_t double_hex_conversion_size =
    1u + 7u * 2u + 2u + dec_conversion_size<int16_t> + 1u;

struct double_parts
{
    uint64_t mantissa;
    uint16_t exponent;
    uint16_t sign;
};

inline double_parts double_extract_parts(double value)
{
    uint64_t const u64_float = *reinterpret_cast<uint64_t*>(&value);

    uint64_t mantissa_1 = 1u;
    mantissa_1 <<= 51u;

    double_parts parts = {
        .mantissa = mantissa_1 +          bit_manip::value_get(u64_float, 51u,  0u),
        .exponent = static_cast<uint16_t>(bit_manip::value_get(u64_float, 62u, 52u)),
        .sign     = static_cast<uint16_t>(bit_manip::value_get(u64_float, 63u, 63u)),
    };

    return parts;
}

inline size_t double_to_hex(char        *buffer,
                            size_t      length,
                            double      double_value,
                            size_t      conv_length = 0u,
                            size_t      precision   = 6u,
                            char        fill_value  = '0',
                            char        prefix_plus = 0)
{
    double_parts const parts = double_extract_parts(double_value);

    bool conv_overflow           = false;
    char *buffer_iter            = buffer;
    char const* const buffer_end = &buffer[length];

    if (parts.sign  == 0)               // Sign indicates positive mantissa.
    {
        if (prefix_plus != 0)
        {
            if (buffer_iter < buffer_end)
            {
                *buffer_iter++ = prefix_plus;
            }
            else
            {
                 conv_overflow = true;
            }
        }
    }
    else                                // Sign indicates negative mantissa.
    {
        if (buffer_iter < buffer_end)
        {
            *buffer_iter++ = '-';
        }
        else
        {
             conv_overflow = true;
        }
    }

    if (buffer_iter < buffer_end)
    {
        *buffer_iter++ = '0';
    }
    else
    {
         conv_overflow = true;
    }

    if (buffer_iter < buffer_end)
    {
        *buffer_iter++ = 'x';
    }
    else
    {
         conv_overflow = true;
    }

    if (buffer_iter < buffer_end)
    {
        size_t const buffer_remain = length - (buffer_iter - buffer);
        size_t const conv_count    = int_to_hex(buffer_iter,
                                                buffer_remain,
                                                parts.mantissa);
        buffer_iter += conv_count;
    }
    else
    {
         conv_overflow = true;
    }

    if (buffer_iter < buffer_end)
    {
        *buffer_iter++ = 'p';
    }
    else
    {
         conv_overflow = true;
    }

    if (buffer_iter < buffer_end)
    {
        int16_t exponent = parts.exponent - 1023u;
        size_t const buffer_remain = length - (buffer_iter - buffer);
        size_t const conv_count    = int_to_dec(buffer_iter,
                                                buffer_remain,
                                                exponent,
                                                1u,
                                                ' ',
                                                '+');
        buffer_iter += conv_count;
    }
    else
    {
         conv_overflow = true;
    }

    if (buffer_iter < buffer_end)
    {
        *buffer_iter++ = 0;
    }
    else
    {
         conv_overflow = true;
    }

    if (conv_overflow)
    {
        memset(buffer, overflow_fill, length);
    }

    return buffer_iter - buffer;
}

