/**
 * @file bit_manip.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * A bit manipulation set of functions.
 */

#pragma once

#include <climits>
#include <cstdlib>
#include <limits>
#include <type_traits>

#if defined __arm__
#include "cmsis_gcc.h"
#endif

namespace bit_manip
{
/// When specifying a bit position, width unsigned short is sufficient.
using bit_pos_t   = unsigned short int;
using bit_width_t = unsigned short int;

/**
 * Create a bit mask given the least significant bit position and its width.
 *
 * @tparam int_type The integer type of the mask to return.
 * @param bit_width The mask bit width.
 * @param bit_pos_lo The least significant bit of the mask.
 *
 * @return uint_mask The unsigned integer mask [msbit:lsbit] (inclusive).
 */
template <typename uint_type> constexpr inline
auto bit_mask(bit_width_t bit_width, bit_pos_t bit_pos_lo) -> uint_type
{
    // bit masks only should be made for unsigned types.
    static_assert(std::is_unsigned<uint_type>::value);

    uint_type mask = 1u;
    if (bit_width < sizeof(uint_type) * CHAR_BIT)
    {
        mask <<= bit_width;
        mask -= 1u;
    }
    else
    {
        mask = std::numeric_limits<uint_type>::max();
    }

    if (bit_pos_lo < sizeof(uint_type) * CHAR_BIT)
    {
        mask <<= bit_pos_lo;
    }
    else
    {
        mask = 0u;
    }

    return mask;
}

/**
 * Sign extend a signed or unsigned integer value.
 *
 * @tparam uint_type The integer type of the integer value being passed in.
 * @note This must be an unsigned integer type.
 *
 * @param int_value The integer value to sign extend.
 * @param sign_pos  The bit position of the sign bit.
 *                  This will be one less than the bit-width of an integer
 *                  value.
 * @example sign_pos for uint32_t is 31u.
 * @example sign_pos for a 24-bit number is 23u.
 *
 * @return auto The sign extended version of the int_value passed in.
 */
template <typename uint_type> constexpr inline
auto sign_extend(uint_type uint_value, bit_pos_t sign_pos) ->
    typename std::make_signed<uint_type>::type
{
    static_assert(std::is_unsigned<uint_type>::value);
    using int_type = typename std::make_signed<uint_type>::type;

    // If the sign bit is located outside of the possible range then the
    // sign cannot be applied. Return the original value.
    if (sign_pos >= sizeof(uint_value) * CHAR_BIT)
    {
        return uint_value;
    }

    uint_type sign_bit_mask = 1u;
    sign_bit_mask <<= sign_pos;

    bool const is_negative = bool(sign_bit_mask & uint_value);
    if (is_negative)
    {
        // The sign bit value is set to '1' since it has tested as negative.
        // Subtract 1 from the sign_bit_mask sets all bits below the sign
        // bit to '1'. Invert the mask and OR with the original value to
        // set all bits above the sign bit to '1'.
        uint_type negative_mask = sign_bit_mask - 1u;
        uint_value |= ~negative_mask;
    }

    return static_cast<int_type>(uint_value);
}

/**
 * Set the bits with the value_set parameter within the bit positions
 * specified by bit_width, bit_pos_lo.
 *
 * @param int_type     int_value The initial value.
 * @param int_set_type value_set The integer value which is emplaced within
 *                               the parameter int_value to form the resulting
 *                               return value.
 * @param bit_width  Specifies the bit range width.
 * @param bit_pos_lo Specifies the bit range LSbit position.
 *
 * @return int_type The integer value after the value_set parameter has been
 *                  set within the int_value.
 */
template <typename int_type, typename int_set_type> constexpr inline
auto value_set(int_type     int_value,
               int_set_type value_set,
               bit_width_t  bit_width,
               bit_pos_t    bit_pos_lo) -> int_type
{
    using uint_type = typename std::make_unsigned<int_type>::type;

    // It is intentional that u_value_set is of type uint_type.
    // It is going to be shifted into position within the bounds of
    // u_value_origin so it needs to be the same.
    uint_type uint_value     = static_cast<uint_type>(int_value);
    uint_type uint_value_set = static_cast<uint_type>(value_set);

    uint_type const mask = bit_mask<uint_type>(bit_width, bit_pos_lo);
    uint_value &= ~mask;

    if (bit_pos_lo < sizeof(uint_type) * CHAR_BIT)
    {
        uint_value_set <<= bit_pos_lo;
    }
    else
    {
        uint_value_set = 0u;
    }

    uint_value_set &= mask;
    uint_value |= uint_value_set;
    return static_cast<int_type>(uint_value);
}

/**
 * Based on the bit (width, position) extract the integer value
 * from within the integer passed in.
 *
 * @note If the integer passed in has a signed type then the value returned
 * will also be signed (and appropriately sign extended if negative value).
 *
 * @param int_value  The value from which to extract bits
 * @param bit_width  Specifies the bit range width.
 * @param bit_pos_lo Specifies the bit range LSbit position.
 *
 * @return int_type The integer value extracted from the parameter int_value
 *                  from the bit range specified.
 */
template <typename int_type> constexpr inline
auto value_get(int_type    int_value,
               bit_width_t bit_width,
               bit_pos_t   bit_pos_lo) -> int_type
{
    using uint_type = typename std::make_unsigned<int_type>::type;

    uint_type       uint_value = static_cast<uint_type>(int_value);
    uint_type const mask       = bit_mask<uint_type>(bit_width, bit_pos_lo);

    uint_value &= mask;
    if (bit_pos_lo < sizeof(uint_type) * CHAR_BIT)
    {
        uint_value >>= bit_pos_lo;
    }
    else
    {
        uint_value = 0u;
    }

    if (std::is_signed<int_type>::value)
    {
        bit_pos_t const sign_pos = bit_width - 1u;
        uint_value               = sign_extend(uint_value, sign_pos);
    }

    return static_cast<int_type>(uint_value);
}

#if defined __arm__

inline uint16_t endian_swap_16(uint16_t value)
{
    return __REV16(value);
}

inline uint16_t endian_swap_32(uint32_t value)
{
    return __REV(value);
}

#else   // __arm__

constexpr inline uint16_t endian_swap_16(uint16_t value)
{
    return (value >> 8u) | (value << 8u);
}

constexpr inline uint16_t endian_swap_32(uint32_t value)
{
    return ((value >> 24) & 0x000000ff) | ((value << 8) & 0x00ff0000) |
           ((value << 24) & 0xff000000) | ((value >> 8) & 0x0000ff00);
}

#endif  // __arm__

}  // namespace bit_manip
