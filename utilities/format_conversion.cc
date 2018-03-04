/**
 * @file format_conversion.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "format_conversion.h"
#include <algorithm>

constexpr char const format_conversion::format_char;
constexpr std::array<char, 19> const format_conversion::known_conversion_specifiers;

bool format_conversion::is_integer_conversion_specifier(char conversion_specifier)
{
    return std::find(
        &known_conversion_specifiers[conv_index_int_begin],
        &known_conversion_specifiers[conv_index_int_end],
        conversion_specifier
        ) < &known_conversion_specifiers[conv_index_int_end];
}

bool format_conversion::is_float_conversion_specifier(char conversion_specifier)
{
    return std::find(
        &known_conversion_specifiers[conv_index_float_begin],
        &known_conversion_specifiers[conv_index_float_end],
        conversion_specifier
        ) < &known_conversion_specifiers[conv_index_float_end];
}

static bool is_digit(char value)
{
    return (value >= '0') && (value <= '9');
}

format_conversion::format_conversion()
{
    this->init();
}

format_conversion::format_conversion(char const *format_spec)
{
    this->parse(format_spec);
}

bool format_conversion::operator==(format_conversion const& other) const
{
    return
        (this->conversion_specifier     == other.conversion_specifier)   &&
        (this->width                    == other.width)                  &&
        (this->width_state              == other.width_state)            &&
        (this->precision                == other.precision)              &&
        (this->precision_state          == other.precision_state)        &&
        (this->length_modifier          == other.length_modifier)        &&
        (this->pad_value                == other.pad_value)              &&
        (this->justification            == other.justification)          &&
        (this->prepend_value            == other.prepend_value)          &&
        (this->alternative_conversion   == other.alternative_conversion) &&
        (this->format_length            == other.format_length)          &&
        (this->parse_error              == other.parse_error)            ;
}

bool format_conversion::operator!=(format_conversion const& other) const
{
    return not (*this == other);
};

void format_conversion::init()
{
    this->conversion_specifier      = 0;
    this->width                     = 0;
    this->width_state               = modifier_state::use_default;
    this->precision                 = 0;
    this->precision_state           = modifier_state::use_default;
    this->length_modifier           = length_modifier::none;
    this->pad_value                 = ' ';
    this->justification             = justification::right;
    this->prepend_value             = 0;
    this->alternative_conversion    = false;
    this->format_length             = 0u;
    this->parse_error               = parse_error::none;
}

void format_conversion::parse(char const *format_spec)
{
    this->init();

    if (*format_spec == format_char)
    {
        char const* format_iter = format_spec + 1u;
        format_iter = this->parse_flags(format_iter);
        format_iter = this->parse_field_width(format_iter);
        format_iter = this->parse_precision(format_iter);
        format_iter = this->parse_length_modifiers(format_iter);

        this->conversion_specifier = *format_iter++;

        if (this->precision_state == format_conversion::modifier_state::use_default)
        {
            if (is_integer_conversion_specifier(this->conversion_specifier))
            {
                this->precision = 1;
            }
            else if (is_float_conversion_specifier(this->conversion_specifier))
            {
                this->precision = 6;
            }
        }

        // Special handling for pointer conversions.
        if (this->conversion_specifier == 'p')
        {
            this->width        = sizeof(uintptr_t) * 2u;
            this->width_state  = format_conversion::modifier_state::is_specified;
            this->pad_value    = '0';
        }

        this->format_length = format_iter - format_spec;

        auto iter_found = std::find(std::begin(known_conversion_specifiers),
                                    std::end(known_conversion_specifiers),
                                    this->conversion_specifier);

        this->parse_error = (iter_found == std::end(known_conversion_specifiers))
                            ? parse_error::bad_parse
                            : parse_error::none;
    }
    else
    {
        this->parse_error = parse_error::no_format_char;
        this->format_length = 0u;
    }
}

char const* format_conversion::parse_flags(char const *format_iter)
{
    bool flag_found = false;

    do
    {
        switch (*format_iter)
        {
        case '-':
            this->justification = justification::left;
            flag_found = true;
            format_iter += 1u;
            break;

        case '+':
        case ' ':
            this->prepend_value = *format_iter;
            flag_found = true;
            format_iter += 1u;
            break;

        case '#':
            this->alternative_conversion = true;
            flag_found = true;
            format_iter += 1u;
            break;

        case '0':
            this->pad_value = '0';
            flag_found = true;
            format_iter += 1u;
            break;

        default:
            flag_found = false;
            break;
        }
    } while(flag_found);

    return format_iter;
}

char const* format_conversion::parse_field_width(char const *format_iter)
{
    this->width = 0;
    this->width_state = modifier_state::use_default;

    if (*format_iter == '*')
    {
        this->width_state = modifier_state::use_asterisk;
    }
    else
    {
        if (is_digit(*format_iter))
        {
            this->width_state = modifier_state::is_specified;
            format_iter = this->parse_short_int(this->width, format_iter);
        }
    }

    return format_iter;
}

char const* format_conversion::parse_precision(char const *format_iter)
{
    this->precision = 0;
    this->precision_state = modifier_state::use_default;

    if (*format_iter == '.')
    {
        format_iter += 1u;
        if (*format_iter == '*')
        {
            this->precision_state = modifier_state::use_asterisk;
        }
        else
        {
            this->precision_state = modifier_state::is_specified;
            format_iter = this->parse_short_int(this->precision, format_iter);
        }
    }

    return format_iter;
}

char const* format_conversion::parse_length_modifiers(char const *format_iter)
{
    switch (*format_iter)
    {
    case 'h':
        this->length_modifier = length_modifier::h;
        format_iter += 1u;
        if (*format_iter == 'h')
        {
            this->length_modifier = length_modifier::hh;
            format_iter += 1u;
        }
        break;

    case 'l':
        this->length_modifier = length_modifier::l;
        format_iter += 1u;
        if (*format_iter == 'l')
        {
            this->length_modifier = length_modifier::ll;
            format_iter += 1u;
        }
        break;

    case 'j':
        this->length_modifier = length_modifier::j;
        format_iter += 1u;
        break;

    case 'z':
        this->length_modifier = length_modifier::z;
        format_iter += 1u;
        break;

    case 't':
        this->length_modifier = length_modifier::t;
        format_iter += 1u;
        break;

    case 'L':
        this->length_modifier = length_modifier::L;
        format_iter += 1u;
        break;

    default:
        // No length modifier was found.
        // Parsing continues without incrementing the format iterator.
        this->length_modifier = length_modifier::none;
        break;
    }

    return format_iter;
}

char const* format_conversion::parse_short_int(short int& value,
                                               char const *format_iter)
{
    value = 0;
    while (is_digit(*format_iter))
    {
        value *= 10;
        value += *format_iter - '0';
        format_iter += 1u;
    }

    return format_iter;
}


