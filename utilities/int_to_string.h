/**
 * @file int_to_string.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 * Integer to string conversions.
 */

#pragma once

#include <cstdint>
#include <cstddef>
#include <type_traits>
#include <limits>
#include <algorithm>
#include <cstring>

/**
 * Convert a 4-bit value to a char hexadecimal representation.
 *
 * @param nybble The value to convert from a 4-bit unsigned value.
 * @param upper_case Sets the range of the output char:
 *        true:  [0-9, A-F].
 *        false: [0-9, a-f].
 * @return char The ASCII representation of the nybble.
 */
char nybble_to_char(uint8_t nybble, bool upper_case = false);

/// Each hex digit occupies 4 bits of the integer value.
static constexpr size_t const hex_bits_per_digit = 4u;

/**
 * When a conversion overflow occurs fill the buffer with this char value.
 * The conversion is useless and should be flagged for easy identification.
 */
static constexpr char const overflow_fill = '-';

/**
 * The number of bytes to allocate for a hex conversion buffer.
 * Add one for the null terminator.
 */
template <typename int_type>
static constexpr size_t hex_conversion_size = (sizeof(int_type) * 2u) + 1u;

/**
 * The number of bytes to allocate for a decimal (base 10) conversion buffer.
 * Use the hex_conversion_size * 16 / 10 since this is the ratio by which
 * base 10 is less efficient in representation.
 * Add 1 for the leading +/- sign
 * Add 1 for the null terminator.
 */
template <typename int_type>
static constexpr size_t dec_conversion_size =
    (((hex_conversion_size<int_type> - 1u) * 16u) / 10u) + 2u;

/**
 * Determine the number of hex digits required to represent an unsigned
 * integer value.
 *
 * @param uint_value The unsigned integer value to convert to an array of hex chars.
 *
 * @return size_t The number of digits required to represent the unsgined value.
 * @example 0x012345 requires 5 digits. The leading zero is ignored.
 */
template<typename uint_type>
inline size_t hex_digits_required(uint_type uint_value)
{
    // Forbid this operation on signed types.
    static_assert(std::numeric_limits<decltype(uint_value)>::is_signed == false);

    // The number of digits required is always >= 1.
    size_t hex_digit_count = 1u;

    // hex_value_limit will be 1 greater than the max value that can fit within
    // the allocated hex digits.
    decltype(uint_value) value_limit = 0x1u << hex_bits_per_digit;

    // Note: in the case of all 'ff' the value_limit value will become zero when
    // the hex value fits within the hex digit count. This is the expected result.
    for ( ; value_limit != 0u; value_limit <<= hex_bits_per_digit, ++hex_digit_count)
    {
        // Since value_limit is max(uint_value) + 1, compare as > (not >=).
        if (value_limit > uint_value)
        {
            break;
        }
    }

    return hex_digit_count;
}

/**
 * Convert an integer value to a string of base 16 character values.
 * The string is null terminated; occupying one byte of the user supplied buffer.
 *
 * @tparam int_type The integer type being passed in for conversion.
 *
 * @param buffer The user supplied output buffer of char values.
 * @param length The length of the output buffer.
 * @param int_value The integer value to convert to chars.
 * @param conv_length The requested length, in chars, of the conversion.
 * @param fill_value The padding value insert for leading zero in the result.
 *
 * @return size_t The number of bytes placed onto the conversion buffer,
 * not including the zero terminator (like strlen).
 * This value will always be <= length.
 * The user supplied buffer is never overflowed.
 * No padding is applied when the number of chars converted < buffer length.
 */
template<typename int_type>
inline size_t int_to_hex(char      *buffer,
                         size_t    length,
                         int_type  int_value,
                         size_t    conv_length = 0u,
                         char      fill_value  = '0')
{
    using uint_type = typename std::make_unsigned<int_type>::type;
    auto uint_value = static_cast<uint_type>(int_value);

    // Since shifting to test for the hex value length is less expensive
    // than memmove(), use the hex_digits_required() to determine the number
    // of bytes required for the hex conversion.

    size_t const digits_required = hex_digits_required(uint_value);
    bool   const conv_overflow   = (digits_required > length);

    conv_length = std::max<size_t>(conv_length, digits_required);
    length      = std::min<size_t>(conv_length + 1u, length);
    char *ptr   = &buffer[length - 1u];

    // Insert the null terminator.
    *ptr-- = 0;

    while (ptr >= buffer)
    {
        *ptr-- = nybble_to_char(static_cast<uint8_t>(uint_value));
        uint_value >>= hex_bits_per_digit;
    }

    if (conv_overflow)
    {
        memset(buffer, overflow_fill, length);
    }

    // Length is the conversion length including the zero terminator.
    // Don't include the zero terminator as part of the count returned.
    return length - 1u;
}

/**
 * Convert an integer value to a string of base 10 character values.
 * The string is null terminated; occupying one byte of the user supplied buffer.
 *
 * @tparam int_type The integer type being passed in for conversion.
 *
 * @param buffer The user supplied output buffer of char values.
 * @param length The length of the output buffer.
 * @param int_value The integer value to convert to chars.
 * @param conv_length The requested length, in chars, of the conversion.
 * @param fill_value The padding value insert for leading zero in the result.
 * @param prefix_plus When a value is negative a prefix_plus character is
 *        inserted prior to the conversion output if this value is non-zero.
 *        printf() format conversions will use '+' or ' ' to prefix positive
 *        values.
 *
 * If true then prefix positive conversion values with
 *        a leading '+' sign.
 *
 * @return size_t The number of bytes placed onto the conversion buffer,
 * not including the zero terminator (like strlen).
 * This value will always be <= length.
 * The user supplied buffer is never overflowed.
 * No padding is applied when the number of chars converted < buffer length.
 */
template<typename int_type>
inline size_t int_to_dec(char      *buffer,
                         size_t    length,
                         int_type  int_value,
                         size_t    conv_length = 0u,
                         char      fill_value  = ' ',
                         char      prefix_plus = 0)
{
    bool const is_negative  = (int_value < 0);
    bool const has_prefix   = is_negative || (prefix_plus != 0);
    char const prefix_value = is_negative? '-' : prefix_plus;

    int_value *= is_negative? -1: 1;

    // Since finding the number of digits required for a base 10 conversion
    // is expensive (requires division) we will convert to digits in place
    // within the user supplied buffer and remove leading zeros by moving
    // the data once converted.

    bool   conv_overflow = false;
    size_t conv_count    = 0u;
    char   *ptr          = &buffer[length - 1u];

    // Insert the null terminator. Not counted as part of the conversion length.
    *ptr-- = 0;

    if (int_value == 0)     // Special case when the value == 0:
    {                       // Insert a '0'; otherwise only a blank gets printed.
        *ptr-- = '0';
        conv_count +=  1u;
    }

    while ((ptr >= buffer) && (int_value != 0))
    {
        auto const digit_value = static_cast<unsigned short int>(int_value % 10u);
        int_value  /= 10u;
        conv_count +=  1u;
        *ptr-- = static_cast<char>(digit_value + '0');
    }

    // There were more digits to convert than space in the buffer.
    conv_overflow = (int_value != 0);

    if (has_prefix)
    {
        if (ptr >= buffer)
        {
            *ptr-- = prefix_value;
            conv_count += 1u;
        }
        else
        {
            conv_overflow = true;
            buffer[0] = prefix_value;
        }
    }

    // The conversion length cannot exceed the buffer length.
    conv_length = std::min<size_t>(conv_length, length);
    long int fill_length = conv_length - conv_count;
    for ( ; fill_length > 0; --fill_length)
    {
        *ptr-- = fill_value;
        conv_count += 1u;
    }

    // ptr is pointing to the next insertion.
    // Point it to the last inserted.
    ptr += 1u;

    if (ptr > buffer)
    {
        memmove(buffer, ptr, conv_count);
    }

    if (conv_overflow)
    {
        memset(buffer, overflow_fill, length);
    }

    return conv_count;
}


