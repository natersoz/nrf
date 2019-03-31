/**
 * @file format_conversion.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * A format conversion object for use in parsing printf format strings.
 */

#pragma once

#include <cstddef>
#include <array>

/**
 * Printf conversion specifiers.
 * Taken from http://en.cppreference.com/w/cpp/io/c/fprintf
 *
 * Modifiers:
 * '-'      The result of the conversion is left-justified within the field.
 *          Conversions are right-justified by default.
 * '+'      The sign of signed conversions is always prepended to the conversion.
 * ' '      Signed conversion without a sign character get a space prepended.
 * '#'      Alternative conversion performed. @todo TBD
 * '0'      For integer and float conversions zeros '0' are leading padding chars.
 * <int>    Specifies the minimum field width.
 * '*'      An integer variable is used to specify the field width.
 * '.'      Used to specify floating point field precision.
 *
 * Format specifiers:
 * '%'      Print the '%' character.
 * 'c'      Write a single character.
 *          If an 'l' modifier is used the conversion is type wchar_t[2].
 * 's'      Writes a character string.
 *          If an 'l' modifier is used the conversion is type wchar_t.
 * 'd' 'i'  Signed integer conversion
 *          If an 'l' modifier is used the conversion is type long int.
 * 'o'      Octal conversion
 * 'x'      Base 16 'hex' conversion, lower case
 * 'X'      Base 16 'hex' conversion, upper case
 * 'u'      Unsigned integer conversion
 *          If an 'l' modifier is used the conversion is type long unsigned int.
 * 'f' 'F'  Floating point conversion
 * 'e' 'E'  Floating point conversion using exponent notation [-]d.ddde±dd
 * 'a' 'A'  Floating point conversion using hex exponent notation [-]0xh.hhhp±d
 * 'g' 'G'  Converts floating point, notation E or F depending on value, precision.
 * 'n'      The number of characters written by the printf call.
 * 'p'      Prints the pointer value.
 */
struct format_conversion
{
    /// The printf character which indicates that a conversion is
    /// to be performed.
    static constexpr char const format_char = '%';

    static constexpr std::array<char, 19> const known_conversion_specifiers =
    {{
        '%',                                        // percent print
        'c',                                        // char    conversion
        's',                                        // string  conversion
        'd', 'i', 'o', 'x', 'X', 'u',               // integer conversions
        'f', 'F', 'e', 'E', 'a', 'A', 'g', 'G',     // float   conversions
        'p',                                        // pointer conversions
        'n',
    }};

    static bool is_integer_conversion_specifier(char conversion_specifier);
    static bool is_float_conversion_specifier(char conversion_specifier);

    ~format_conversion()                                    = default;
    format_conversion(format_conversion const&)             = delete;
    format_conversion& operator=(format_conversion const&)  = delete;

    format_conversion();

    /**
     * Create the format conversion struct based on the format converssion
     * string starting with and including the format_char.
     *
     * @param format_spec The format specifier string.
     * @param format_length The maximum length allowed for parsing the string.
     */
    format_conversion(char const *format_spec);

    enum class justification
    {
        left,
        right
    };

    enum class length_modifier
    {
        /// Signed and unsigned int conversion.
        /// char* string conversion.
        none = 0,

        /// Signed and unsigned char conversion.
        hh,

        /// Signed and unsigned short int conversion.
        h,

        /// Signed and unsigned long int conversion.
        /// wchar_t* string conversion.
        /// wint_t wide char conversion.
        /// double floating point conversion
        l,

        /// Signed and unsigned long long int conversion.
        ll,

        /// Signed and unsigned greatest width integer conversions.
        /// Uses types intmax_t and uintmax_t in stdint.h.
        j,

        /// Signed and unsigned size_t type conversions.
        z,

        /// Signed and unsigned ptrdiff_t type conversions.
        t,

        /// Long double floating point conversion.
        L
    };

    enum class modifier_state
    {
        use_default = 0,
        use_asterisk,
        is_specified
    };

    enum class parse_error
    {
        /// Indicates the format specifier was parsed successfully.
        none = 0,

        /// The format character '%' was not found as the format_specifier[0].
        no_format_char,

        /// The parsing failed. Specific reasons why are not provided.
        bad_parse
    };

    /// The char value of the conversion specifier. i.e. 'd', 'i', 'x', 'u', etc.
    char conversion_specifier;

    /// Set to zero for the default minimal conversion width.
    /// Set to non-zero to specify a minimal conversion width.
    short int width;

    modifier_state width_state;

    /// By default the precision for integer types is 1.
    /// Integer precision specifies the minimum number of conversion digits.
    /// By default the precision for floating points is 6.
    /// Floating precision specifies the minimum number of conversion digits
    /// after the decimal point.
    short int precision;

    modifier_state precision_state;

    /// @see enum class length_modifier
    length_modifier length_modifier;

    /// The default is padding with the space ' ' character padding to lenth.
    /// Set to '0' for zero padding to the conversion length.
    char pad_value;

    /// How the conversion is justified.
    /// Conversions are right justified by default.
    justification justification;

    /// '+', prepend a plus sign for positive values.
    /// ' ', prepend a space for positive values.
    char prepend_value;

    /**
     * Octal: precision is increased if necessary, to write one leading zero.
     * Hex:   0x or 0X is prefixed to results if the converted value is nonzero.
     * Float: decimal point character is written even if no digits follow it.
     */
    bool alternative_conversion;

    /// The number of characters comprising the format conversion sequenece.
    /// This is the number of characters following the format_char delimiter
    /// and does not include the format_char delimiter
    std::size_t format_length;

    parse_error parse_error;

    bool operator==(format_conversion const& other) const;
    bool operator!=(format_conversion const& other) const;

private:
    static constexpr std::size_t const conv_index_int_begin   =  3u;
    static constexpr std::size_t const conv_index_int_end     =  9u;

    static constexpr std::size_t const conv_index_float_begin =  9u;
    static constexpr std::size_t const conv_index_float_end   = 17u;

    /// Set data members to default initial conditions.
    void init();

    /**
     * Parse the format string to initialize the struct format_conversion
     * data fields.
     */
    void parse(char const *format_spec);

    char const* parse_flags(char const *format_iter);
    char const* parse_field_width(char const *format_iter);
    char const* parse_precision(char const *format_iter);
    char const* parse_length_modifiers(char const *format_iter);
    char const* parse_short_int(short int& value, char const *format_iter);
};

