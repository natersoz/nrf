/**
 * @file charconv.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * A stand-in for "charconv" until GCC suppports it and we upgrade to it.
 */

#pragma once
#include <system_error>

namespace std
{

// floating-point format for numeric conversion
enum class chars_format
{
    scientific  = 0x01,
    fixed       = 0x02,
    hex         = 0x03,
    general     = fixed | scientific
};

// numeric output conversion
struct to_chars_result
{
    char *ptr;
    errc ec;
};

// numeric input conversion
struct from_chars_result
{
    const char *ptr;
    errc ec;
};

template <typename int_type>
to_chars_result to_chars(char *first, char *last, int_type   value, int base = 10);

template <typename float_type>
to_chars_result to_chars(char *first, char *last, float_type value);

template <typename float_type>
to_chars_result to_chars(char *first, char *last, float_type value, chars_format fmt);

template <typename float_type>
to_chars_result to_chars(char *first, char *last, float_type value, chars_format fmt, int precision);

template <typename int_type>
from_chars_result from_chars(const char *first, const char *last, int_type   &value, int base = 10);

template <typename float_type>
from_chars_result from_chars(const char *first, const char *last, float_type &value, chars_format fmt = chars_format::general);

} // namespace std
