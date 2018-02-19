/**
 * @file test_int_to_string.cc
 */

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

bool test_format_conversion(char const *format_spec,
                            format_conversion const& format_converted,
                            format_conversion const& format_expected)
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

        assert(0);
        return false;
    }
    else
    {
        if (debug_print)
        {
            std::cerr << "----------------------------" << std::endl;
            std::cerr << "converted:" << std::endl;
            std::cerr << format_converted << std::endl;
        }
        return true;
    }
}

static bool test_integers()
{
    bool result = true;

    {
        char const* format_spec = "%d";
        format_conversion const format_converted(format_spec);
        format_conversion format_expected;
        format_expected.conversion_specifier    = format_spec[1];
        format_expected.precision               = 1;
        format_expected.format_length           = strlen(format_spec);
        test_format_conversion(format_spec, format_converted, format_expected);
    }

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

    {
        char const* format_spec = "%o";
        format_conversion const format_converted(format_spec);
        format_conversion format_expected;
        format_expected.conversion_specifier    = format_spec[1];
        format_expected.precision               = 1;
        format_expected.format_length           = strlen(format_spec);
        test_format_conversion(format_spec, format_converted, format_expected);
    }

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

    {
        char const* format_spec = "%X";
        format_conversion const format_converted(format_spec);
        format_conversion format_expected;
        format_expected.conversion_specifier    = format_spec[1];
        format_expected.precision               = 1;
        format_expected.format_length           = strlen(format_spec);
        test_format_conversion(format_spec, format_converted, format_expected);
    }

    {
        char const* format_spec = "%u";
        format_conversion const format_converted(format_spec);
        format_conversion format_expected;
        format_expected.conversion_specifier    = format_spec[1];
        format_expected.precision               = 1;
        format_expected.format_length           = strlen(format_spec);
        test_format_conversion(format_spec, format_converted, format_expected);
    }

    return result;
}

static bool test_single()
{
    bool result = true;

    {
        char const* format_spec = "%%";
        format_conversion const format_converted(format_spec);
        format_conversion format_expected;
        format_expected.conversion_specifier    = '%';
        format_expected.format_length           = strlen(format_spec);
        test_format_conversion(format_spec, format_converted, format_expected);
    }

    {
        char const* format_spec = "%c";
        format_conversion const format_converted(format_spec);
        format_conversion format_expected;
        format_expected.conversion_specifier    = format_spec[1];
        format_expected.format_length           = strlen(format_spec);
        test_format_conversion(format_spec, format_converted, format_expected);
    }

    {
        char const* format_spec = "%s";
        format_conversion const format_converted(format_spec);
        format_conversion format_expected;
        format_expected.conversion_specifier    = format_spec[1];
        format_expected.format_length           = strlen(format_spec);
        test_format_conversion(format_spec, format_converted, format_expected);
    }

    {
        char const* format_spec = "%f";
        format_conversion const format_converted(format_spec);
        format_conversion format_expected;
        format_expected.conversion_specifier    = format_spec[1];
        format_expected.precision               = 6;
        format_expected.format_length           = strlen(format_spec);
        test_format_conversion(format_spec, format_converted, format_expected);
    }

    {
        char const* format_spec = "%F";
        format_conversion const format_converted(format_spec);
        format_conversion format_expected;
        format_expected.conversion_specifier    = format_spec[1];
        format_expected.precision               = 6;
        format_expected.format_length           = strlen(format_spec);
        test_format_conversion(format_spec, format_converted, format_expected);
    }

    {
        char const* format_spec = "%e";
        format_conversion const format_converted(format_spec);
        format_conversion format_expected;
        format_expected.conversion_specifier    = format_spec[1];
        format_expected.precision               = 6;
        format_expected.format_length           = strlen(format_spec);
        test_format_conversion(format_spec, format_converted, format_expected);
    }

    {
        char const* format_spec = "%E";
        format_conversion const format_converted(format_spec);
        format_conversion format_expected;
        format_expected.conversion_specifier    = format_spec[1];
        format_expected.precision               = 6;
        format_expected.format_length           = strlen(format_spec);
        test_format_conversion(format_spec, format_converted, format_expected);
    }

    {
        char const* format_spec = "%a";
        format_conversion const format_converted(format_spec);
        format_conversion format_expected;
        format_expected.conversion_specifier    = format_spec[1];
        format_expected.precision               = 6;
        format_expected.format_length           = strlen(format_spec);
        test_format_conversion(format_spec, format_converted, format_expected);
    }

    {
        char const* format_spec = "%A";
        format_conversion const format_converted(format_spec);
        format_conversion format_expected;
        format_expected.conversion_specifier    = format_spec[1];
        format_expected.precision               = 6;
        format_expected.format_length           = strlen(format_spec);
        test_format_conversion(format_spec, format_converted, format_expected);
    }

    {
        char const* format_spec = "%g";
        format_conversion const format_converted(format_spec);
        format_conversion format_expected;
        format_expected.conversion_specifier    = format_spec[1];
        format_expected.precision               = 6;
        format_expected.format_length           = strlen(format_spec);
        test_format_conversion(format_spec, format_converted, format_expected);
    }

    {
        char const* format_spec = "%G";
        format_conversion const format_converted(format_spec);
        format_conversion format_expected;
        format_expected.conversion_specifier    = format_spec[1];
        format_expected.precision               = 6;
        format_expected.format_length           = strlen(format_spec);
        test_format_conversion(format_spec, format_converted, format_expected);
    }

    {
        char const* format_spec = "%n";
        format_conversion const format_converted(format_spec);
        format_conversion format_expected;
        format_expected.conversion_specifier    = format_spec[1];
        format_expected.format_length           = strlen(format_spec);
        test_format_conversion(format_spec, format_converted, format_expected);
    }

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

    return result;
}

int main(int argc, char **argv)
{
    int result = 0;

    // If there are arguments then do parsing with the user command line arguments.
    if (argc > 1)
    {
        for (int iter = 1; iter < argc; ++iter)
        {
            format_conversion const format_converted(argv[iter]);
            std::cout << format_converted << std::endl;
        }

        return 0;
    }

    test_single();
    test_integers();

    std::cout << "--- Tests Pass" << std::endl;
    return result;
}
