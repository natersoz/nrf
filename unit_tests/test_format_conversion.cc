/**
 * @file test_int_to_string.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "gtest/gtest.h"
#include "format_conversion.h"

#include <iostream>
#include <sstream>
#include <iomanip>
#include <cassert>

static constexpr bool debug_print = false;

std::ostream& operator << (std::ostream &os, format_conversion const& fmt_conv)
{
    os << "conv spec : '" << fmt_conv.conversion_specifier << "'" << std::endl;

    os << "conv width: ";
    if (fmt_conv.width_state == format_conversion::modifier_state::use_asterisk)
    {
        os << "'" << '*' << "'";
    }
    else
    {
        os << fmt_conv.width;
    }
    os << std::endl;

    os << "precision : ";
    if (fmt_conv.precision_state == format_conversion::modifier_state::use_asterisk)
    {
        os << "'" << '*' << "'";
    }
    else
    {
        os << fmt_conv.precision;
    }
    os << std::endl;

    os << "length mod: ";
    switch (fmt_conv.length_modifier) {
    case format_conversion::length_modifier::none:  os << "-none-";     break;
    case format_conversion::length_modifier::hh:    os << "hh";         break;
    case format_conversion::length_modifier::h:     os << "h";          break;
    case format_conversion::length_modifier::l:     os << "l";          break;
    case format_conversion::length_modifier::ll:    os << "ll";         break;
    case format_conversion::length_modifier::j:     os << "j";          break;
    case format_conversion::length_modifier::z:     os << "z";          break;
    case format_conversion::length_modifier::t:     os << "t";          break;
    case format_conversion::length_modifier::L:     os << "L";          break;
    default:                                        os << "-unknown-";  break;
    }
    os << std::endl;

    os << "pad value : ";
    if (fmt_conv.pad_value == 0)
    {
        os << "-none-";
    }
    else
    {
        os << "'" << fmt_conv.pad_value << "'";
    }
    os << std::endl;

    os << "justify   : ";
    switch (fmt_conv.justification)
    {
    case format_conversion::justification::left:    os << "left";       break;
    case format_conversion::justification::right:   os << "right";      break;
    default:                                        os << "-unknown-";  break;
    }
    os << std::endl;

    os << "prepend   : ";
    if (fmt_conv.prepend_value == 0)
    {
        os << "-none-";
    }
    else
    {
        os << "'" << fmt_conv.prepend_value << "'";
    }
    os << std::endl;

    os << "alt conv  : " << fmt_conv.alternative_conversion << std::endl;
    os << "fmt length: " << fmt_conv.format_length << std::endl;
    os << "error     : ";
    switch (fmt_conv.parse_error)
    {
    case format_conversion::parse_error::none:              os << "none";       break;
    case format_conversion::parse_error::no_format_char:    os << "no %";       break;
    case format_conversion::parse_error::bad_parse:         os << "fail";       break;
    default:                                                os << "-unknown-";  break;
    }

    return os;
}

static void test_format_conversion(char const *format_spec,
                                   format_conversion const& format_converted,
                                   format_conversion const& format_expected)
{
    if (debug_print)
    {
        if (format_converted != format_expected)
        {
            std::cerr << "============================" << std::endl;
            std::cerr << "format spec: '" << format_spec << "'" << std::endl;
            std::cerr << "----------------------------" << std::endl;
            std::cerr << "converted:" << std::endl;
            std::cerr << format_converted << std::endl;

            std::cerr << "----------------------------" << std::endl;
            std::cerr << "expected:" << std::endl;
            std::cerr << format_expected << std::endl;
            std::cerr << "============================" << std::endl;
        }
        else
        {
            std::cerr << "----------------------------" << std::endl;
            std::cerr << "converted:" << std::endl;
            std::cerr << format_converted << std::endl;
        }
    }

    ASSERT_TRUE(format_converted == format_expected);
}

TEST(FormatConversion, Integer_d)
{
    char const* format_spec = "%d";
    format_conversion const format_converted(format_spec);
    format_conversion format_expected;
    format_expected.conversion_specifier    = format_spec[1];
    format_expected.precision               = 1;
    format_expected.format_length           = strlen(format_spec);
    test_format_conversion(format_spec, format_converted, format_expected);
}

TEST(FormatConversion, Integer_10d)
{
    char const* format_spec = "%10d";
    format_conversion const format_converted(format_spec);
    format_conversion format_expected;
    format_expected.conversion_specifier    = 'd';
    format_expected.width                   = 10;
    format_expected.width_state             = format_conversion::modifier_state::is_specified;
    format_expected.precision               = 1;
    format_expected.format_length           = strlen(format_spec);
    test_format_conversion(format_spec, format_converted, format_expected);
}

TEST(FormatConversion, Integer_10_20d)
{
    char const* format_spec = "%10.20d";
    format_conversion const format_converted(format_spec);
    format_conversion format_expected;
    format_expected.conversion_specifier    = 'd';
    format_expected.width                   = 10;
    format_expected.width_state             = format_conversion::modifier_state::is_specified;
    format_expected.precision               = 20;
    format_expected.precision_state         = format_conversion::modifier_state::is_specified;
    format_expected.format_length           = strlen(format_spec);
    test_format_conversion(format_spec, format_converted, format_expected);
}

TEST(FormatConversion, Integer_10i)
{
    char const* format_spec = "%10li";
    format_conversion const format_converted(format_spec);
    format_conversion format_expected;
    format_expected.conversion_specifier    = 'i';
    format_expected.width                   = 10;
    format_expected.width_state             = format_conversion::modifier_state::is_specified;
    format_expected.precision               = 1;
    format_expected.length_modifier         = format_conversion::length_modifier::l;
    format_expected.format_length           = strlen(format_spec);
    test_format_conversion(format_spec, format_converted, format_expected);
}

TEST(FormatConversion, Unsigned_plus_10_0L)
{
    char const* format_spec = "%+10.0lu";
    format_conversion const format_converted(format_spec);
    format_conversion format_expected;
    format_expected.conversion_specifier    = 'u';
    format_expected.width                   = 10;
    format_expected.width_state             = format_conversion::modifier_state::is_specified;
    format_expected.precision               = 0;
    format_expected.precision_state         = format_conversion::modifier_state::is_specified;
    format_expected.length_modifier         = format_conversion::length_modifier::l;
    format_expected.prepend_value           = '+';
    format_expected.format_length           = strlen(format_spec);
    test_format_conversion(format_spec, format_converted, format_expected);
}

TEST(FormatConversion, Unsigned_space_10_0Lu)
{
    char const* format_spec = "% 10.0lu";
    format_conversion const format_converted(format_spec);
    format_conversion format_expected;
    format_expected.conversion_specifier    = 'u';
    format_expected.width                   = 10;
    format_expected.width_state             = format_conversion::modifier_state::is_specified;
    format_expected.precision               = 0;
    format_expected.precision_state         = format_conversion::modifier_state::is_specified;
    format_expected.length_modifier         = format_conversion::length_modifier::l;
    format_expected.prepend_value           = ' ';
    format_expected.format_length           = strlen(format_spec);
    test_format_conversion(format_spec, format_converted, format_expected);
}

TEST(FormatConversion, Unsigned_010_0Lu)
{
    char const* format_spec = "%010.0lu";
    format_conversion const format_converted(format_spec);
    format_conversion format_expected;
    format_expected.conversion_specifier    = 'u';
    format_expected.width                   = 10;
    format_expected.width_state             = format_conversion::modifier_state::is_specified;
    format_expected.precision               = 0;
    format_expected.precision_state         = format_conversion::modifier_state::is_specified;
    format_expected.length_modifier         = format_conversion::length_modifier::l;
    format_expected.pad_value               = '0';
    format_expected.format_length           = strlen(format_spec);
    test_format_conversion(format_spec, format_converted, format_expected);
}

TEST(FormatConversion, Unsigned_space_010_0Lu)
{
    char const* format_spec = "% 010.0lu";
    format_conversion const format_converted(format_spec);
    format_conversion format_expected;
    format_expected.conversion_specifier    = 'u';
    format_expected.width                   = 10;
    format_expected.width_state             = format_conversion::modifier_state::is_specified;
    format_expected.precision               = 0;
    format_expected.precision_state         = format_conversion::modifier_state::is_specified;
    format_expected.length_modifier         = format_conversion::length_modifier::l;
    format_expected.pad_value               = '0';
    format_expected.prepend_value           = ' ';
    format_expected.format_length           = strlen(format_spec);
    test_format_conversion(format_spec, format_converted, format_expected);
}

TEST(FormatConversion, Unsigned_space_010_0hhu)
{
    char const* format_spec = "% 010.0hhu";
    format_conversion const format_converted(format_spec);
    format_conversion format_expected;
    format_expected.conversion_specifier    = 'u';
    format_expected.width                   = 10;
    format_expected.width_state             = format_conversion::modifier_state::is_specified;
    format_expected.precision               = 0;
    format_expected.precision_state         = format_conversion::modifier_state::is_specified;
    format_expected.length_modifier         = format_conversion::length_modifier::hh;
    format_expected.pad_value               = '0';
    format_expected.prepend_value           = ' ';
    format_expected.format_length           = strlen(format_spec);
    test_format_conversion(format_spec, format_converted, format_expected);
}

TEST(FormatConversion, Unsigned_10LLx)
{
    char const* format_spec = "%10llx";
    format_conversion const format_converted(format_spec);
    format_conversion format_expected;
    format_expected.conversion_specifier    = 'x';
    format_expected.width                   = 10;
    format_expected.width_state             = format_conversion::modifier_state::is_specified;
    format_expected.precision               = 1;
    format_expected.length_modifier         = format_conversion::length_modifier::ll;
    format_expected.format_length           = strlen(format_spec);
    test_format_conversion(format_spec, format_converted, format_expected);
}

TEST(FormatConversion, Unsigned_o)
{
    char const* format_spec = "%o";
    format_conversion const format_converted(format_spec);
    format_conversion format_expected;
    format_expected.conversion_specifier    = format_spec[1];
    format_expected.precision               = 1;
    format_expected.format_length           = strlen(format_spec);
    test_format_conversion(format_spec, format_converted, format_expected);
}

TEST(FormatConversion, Unsigned_08lx)
{
    char const* format_spec = "%08lx";
    format_conversion const format_converted(format_spec);
    format_conversion format_expected;
    format_expected.conversion_specifier    = 'x';
    format_expected.width                   = 8;
    format_expected.width_state             = format_conversion::modifier_state::is_specified;
    format_expected.precision               = 1;
    format_expected.length_modifier         = format_conversion::length_modifier::l;
    format_expected.pad_value               = '0';
    format_expected.format_length           = strlen(format_spec);
    test_format_conversion(format_spec, format_converted, format_expected);
}

TEST(FormatConversion, Unsigned_X)
{
    char const* format_spec = "%X";
    format_conversion const format_converted(format_spec);
    format_conversion format_expected;
    format_expected.conversion_specifier    = format_spec[1];
    format_expected.precision               = 1;
    format_expected.format_length           = strlen(format_spec);
    test_format_conversion(format_spec, format_converted, format_expected);
}

TEST(FormatConversion, Unsigned_u)
{
    char const* format_spec = "%u";
    format_conversion const format_converted(format_spec);
    format_conversion format_expected;
    format_expected.conversion_specifier    = format_spec[1];
    format_expected.precision               = 1;
    format_expected.format_length           = strlen(format_spec);
    test_format_conversion(format_spec, format_converted, format_expected);
}

TEST(FormatConversion, PercentSign)
{
    char const* format_spec = "%%";
    format_conversion const format_converted(format_spec);
    format_conversion format_expected;
    format_expected.conversion_specifier    = '%';
    format_expected.format_length           = strlen(format_spec);
    test_format_conversion(format_spec, format_converted, format_expected);
}

TEST(FormatConversion, Char)
{
    char const* format_spec = "%c";
    format_conversion const format_converted(format_spec);
    format_conversion format_expected;
    format_expected.conversion_specifier    = format_spec[1];
    format_expected.format_length           = strlen(format_spec);
    test_format_conversion(format_spec, format_converted, format_expected);
}

TEST(FormatConversion, String)
{
    char const* format_spec = "%s";
    format_conversion const format_converted(format_spec);
    format_conversion format_expected;
    format_expected.conversion_specifier    = format_spec[1];
    format_expected.format_length           = strlen(format_spec);
    test_format_conversion(format_spec, format_converted, format_expected);
}

TEST(FormatConversion, Float_f)
{
    char const* format_spec = "%f";
    format_conversion const format_converted(format_spec);
    format_conversion format_expected;
    format_expected.conversion_specifier    = format_spec[1];
    format_expected.precision               = 6;
    format_expected.format_length           = strlen(format_spec);
    test_format_conversion(format_spec, format_converted, format_expected);
}

TEST(FormatConversion, Float_F)
{
    char const* format_spec = "%F";
    format_conversion const format_converted(format_spec);
    format_conversion format_expected;
    format_expected.conversion_specifier    = format_spec[1];
    format_expected.precision               = 6;
    format_expected.format_length           = strlen(format_spec);
    test_format_conversion(format_spec, format_converted, format_expected);
}

TEST(FormatConversion, Float_e)
{
    char const* format_spec = "%e";
    format_conversion const format_converted(format_spec);
    format_conversion format_expected;
    format_expected.conversion_specifier    = format_spec[1];
    format_expected.precision               = 6;
    format_expected.format_length           = strlen(format_spec);
    test_format_conversion(format_spec, format_converted, format_expected);
}

TEST(FormatConversion, Float_E)
{
    char const* format_spec = "%E";
    format_conversion const format_converted(format_spec);
    format_conversion format_expected;
    format_expected.conversion_specifier    = format_spec[1];
    format_expected.precision               = 6;
    format_expected.format_length           = strlen(format_spec);
    test_format_conversion(format_spec, format_converted, format_expected);
}

TEST(FormatConversion, Float_a)
{
    char const* format_spec = "%a";
    format_conversion const format_converted(format_spec);
    format_conversion format_expected;
    format_expected.conversion_specifier    = format_spec[1];
    format_expected.precision               = 6;
    format_expected.format_length           = strlen(format_spec);
    test_format_conversion(format_spec, format_converted, format_expected);
}

TEST(FormatConversion, Float_A)
{
    char const* format_spec = "%A";
    format_conversion const format_converted(format_spec);
    format_conversion format_expected;
    format_expected.conversion_specifier    = format_spec[1];
    format_expected.precision               = 6;
    format_expected.format_length           = strlen(format_spec);
    test_format_conversion(format_spec, format_converted, format_expected);
}

TEST(FormatConversion, Float_g)
{
    char const* format_spec = "%g";
    format_conversion const format_converted(format_spec);
    format_conversion format_expected;
    format_expected.conversion_specifier    = format_spec[1];
    format_expected.precision               = 6;
    format_expected.format_length           = strlen(format_spec);
    test_format_conversion(format_spec, format_converted, format_expected);
}

TEST(FormatConversion, Float_G)
{
    char const* format_spec = "%G";
    format_conversion const format_converted(format_spec);
    format_conversion format_expected;
    format_expected.conversion_specifier    = format_spec[1];
    format_expected.precision               = 6;
    format_expected.format_length           = strlen(format_spec);
    test_format_conversion(format_spec, format_converted, format_expected);
}

TEST(FormatConversion, StringLength)
{
    char const* format_spec = "%n";
    format_conversion const format_converted(format_spec);
    format_conversion format_expected;
    format_expected.conversion_specifier    = format_spec[1];
    format_expected.format_length           = strlen(format_spec);
    test_format_conversion(format_spec, format_converted, format_expected);
}

TEST(FormatConversion, Pointer)
{
    char const* format_spec = "%p";
    format_conversion const format_converted(format_spec);
    format_conversion format_expected;
    format_expected.conversion_specifier    = format_spec[1];
    format_expected.width                   = sizeof(uintptr_t) * 2u;
    format_expected.width_state             = format_conversion::modifier_state::is_specified;
    format_expected.pad_value               = '0';
    format_expected.format_length           = strlen(format_spec);
    test_format_conversion(format_spec, format_converted, format_expected);
}
