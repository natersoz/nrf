/**
 * @file vwritef.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * A variable argument printf like writer interface.
 *
 * @todo not implemented:
 * + Width based on '*' modifier. format_conversion::width_state
 * + Precision
 * + String padding
 * + Justification to right
 * + No octal conversion.
 * + No floating point.
 * + Small case only for hex conversions.
 * + Padding longer than the conversion buffer is truncated.
 */

#include "vwritef.h"
#include "format_conversion.h"
#include "int_to_string.h"

#include <type_traits>
#include <cstring>

using signed_size_t = typename std::make_signed<size_t>::type;

static size_t convert_char(output_stream& os,
                           format_conversion const& conversion,
                           va_list &args)
{
    int const int_value = va_arg(args, int);
    char const value = static_cast<char>(int_value);
    return os.write(&value, sizeof(value));
}

static size_t write_padding(output_stream& os, size_t length, char pad_value)
{
    size_t n_written = 0u;
    for (; length > 0u; --length)
    {
        char const space = ' ';
        n_written += os.write(&space, sizeof(space));
    }

    return n_written;
}

static size_t convert_string(output_stream& os,
                             format_conversion const& conversion,
                             va_list &args)
{
    size_t n_write = 0u;

    // Set string_ptr to the pointer obtained from va_arg;
    // this is the pointer to the beginning of the zero terminated string.
    char const *string_ptr = va_arg(args, char *);

    if ((conversion.width_state == format_conversion::modifier_state::is_specified) &&
        (conversion.justification == format_conversion::justification::right))
    {
        size_t const string_length = strlen(string_ptr);
        if (conversion.width > static_cast<signed_size_t>(string_length))
        {
            size_t const pad_length = conversion.width - string_length;
            n_write += write_padding(os, pad_length, conversion.pad_value);
        }
    }

    for (; *string_ptr != 0; ++string_ptr)
    {
        n_write += os.write(string_ptr, sizeof(*string_ptr));
    }

    if ((conversion.width_state == format_conversion::modifier_state::is_specified) &&
        (conversion.justification == format_conversion::justification::left))
    {
        if (conversion.width > static_cast<signed_size_t>(n_write))
        {
            size_t const pad_length = conversion.width - n_write;
            n_write += write_padding(os, pad_length, conversion.pad_value);
        }
    }

    return n_write;
}

template <typename int_type>
static size_t convert_int_to_dec(output_stream& os,
                                 format_conversion const& conversion,
                                 va_list &args)
{
    int_type const int_value = va_arg(args, int_type);
    char buffer[dec_conversion_size<decltype(int_value)>];
    size_t const n_conv = int_to_dec(buffer,
                                     sizeof(buffer),
                                     int_value,
                                     conversion.width,
                                     conversion.pad_value,
                                     conversion.prepend_value);

    return os.write(buffer, n_conv);
}

static size_t convert_int_to_dec(output_stream& os,
                                 format_conversion const& conversion,
                                 va_list &args)
{
    switch (conversion.length_modifier)
    {
    case format_conversion::length_modifier::ll:
        return convert_int_to_dec<long long int>(os, conversion, args);
    case format_conversion::length_modifier::l:
        return convert_int_to_dec<long int>(os, conversion, args);
    default:
        return convert_int_to_dec<int>(os, conversion, args);
    }
}

template <typename int_type>
static size_t convert_uint_to_dec(output_stream& os,
                                 format_conversion const& conversion,
                                 va_list &args)
{
    int_type const int_value = va_arg(args, int_type);
    char buffer[dec_conversion_size<decltype(int_value)>];
    size_t const n_conv = int_to_dec(buffer,
                                     sizeof(buffer),
                                     int_value,
                                     conversion.width,
                                     conversion.pad_value);

    return os.write(buffer, n_conv);
}

static size_t convert_uint_to_dec(output_stream& os,
                                 format_conversion const& conversion,
                                 va_list &args)
{
    switch (conversion.length_modifier)
    {
    case format_conversion::length_modifier::ll:
        return convert_uint_to_dec<unsigned long long int>(os, conversion, args);
    case format_conversion::length_modifier::l:
        return convert_uint_to_dec<unsigned long int>(os, conversion, args);
    default:
        return convert_uint_to_dec<unsigned int>(os, conversion, args);
    }
}

template <typename int_type>
static size_t convert_int_to_hex(output_stream& os,
                                 format_conversion const& conversion,
                                 va_list &args)
{
    int_type const int_value = va_arg(args, int_type);
    char buffer[hex_conversion_size<decltype(int_value)>];
    size_t const n_conv = int_to_hex(buffer,
                                     sizeof(buffer),
                                     int_value,
                                     conversion.width,
                                     conversion.pad_value);

    return os.write(buffer, n_conv);
}

static size_t convert_int_to_hex(output_stream& os,
                                 format_conversion const& conversion,
                                 va_list &args)
{
    switch (conversion.length_modifier)
    {
    case format_conversion::length_modifier::ll:
        return convert_int_to_hex<long long int>(os, conversion, args);
    case format_conversion::length_modifier::l:
        return convert_int_to_hex<long int>(os, conversion, args);
    default:
        return convert_int_to_hex<int>(os, conversion, args);
    }
}

static size_t convert_pointer(output_stream& os,
                              format_conversion const& conversion,
                              va_list &args)
{
    return convert_int_to_hex<uintptr_t>(os, conversion, args);
}

size_t writef(output_stream& os, char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    size_t const n_written = vwritef(os, fmt, args);

    va_end(args);
    return n_written;
}

size_t vwritef(output_stream& os, char const *fmt, va_list &args)
{
    size_t n_written = 0u;
    char const *fmt_iter = fmt;

    while (*fmt_iter != 0)
    {
        if (*fmt_iter == format_conversion::format_char)
        {
            size_t n_write = 0u;
            format_conversion const conversion(fmt_iter);
            fmt_iter += conversion.format_length;
            switch (conversion.conversion_specifier)
            {
            case format_conversion::format_char:
                n_write = os.write(&format_conversion::format_char,
                                 sizeof(format_conversion::format_char));
                break;

            case 'c':
                n_write = convert_char(os, conversion, args);
                break;

            case 's':
                n_write = convert_string(os, conversion, args);
                break;

            case 'd':
            case 'i':
                n_write = convert_int_to_dec(os, conversion, args);
                break;

            case 'o':       // For now octal will fall throug to hex
            case 'x':
            case 'X':
                n_write = convert_int_to_hex(os, conversion, args);
                break;

            case 'u':
                n_write = convert_uint_to_dec(os, conversion, args);
                break;

            case 'f':
            case 'F':
            case 'e':
            case 'E':
            case 'a':
            case 'A':
            case 'g':
            case 'G':
                break;

            case 'p':
                n_write = convert_pointer(os, conversion, args);
                break;

            case 'n':
                break;

            default:
                break;
            }
            n_written += n_write;
        }
        else
        {
            n_written += os.write(fmt_iter, sizeof(*fmt_iter));
            fmt_iter += 1u;
        }
    }

    return n_written;
}

