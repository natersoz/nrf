/**
 * @file bit_manip.h
 */

#pragma once

#include <type_traits>

namespace bit_manip
{

using bit_pos_t = unsigned short int;

/**
 * Create a bit mask given the most and least significant bit positions.
 * The mask is create with the msbit, lsbit included: [msbit:lsbit].
 *
 * @tparam int_type The integer type of the mask to return.
 * @param bit_pos_hi The most significant bit of the mask.
 * @param bit_pos_lo The least significant bit of the mask.
 *
 * @return uint_mask The unsigned integer mask [msbit:lsbit] (inclusive).
 */
template <typename uint_type>
inline auto bit_mask(bit_pos_t bit_pos_hi, bit_pos_t bit_pos_lo) -> uint_type
{
    // bit masks only should be made for unsgined types.
    static_assert(std::is_unsigned<uint_type>::value);

    // Assign first mask the value '1' prior to shifting.
    // This insures '1' has uint_type and will fit the shift operation.
    uint_type mask = 1u;
    mask <<= (bit_pos_hi - bit_pos_lo) + 1u;
    mask -= 1;
    mask <<= bit_pos_lo;
    return mask;
}

/**
 * Sign extend a signed or unsigned integer value.
 *
 * @tparam int_type The integer type of the integer value being passed in.
 * @param int_value The integer value to sign extend.
 * @param sign_pos  The bit position of the sign bit.
 *
 * @return auto The sign extended version of the int_value passed in.
 */
template <typename int_type>
auto sign_extend(int_type int_value, bit_pos_t sign_pos) -> int_type
{
    using uint_type = typename std::make_unsigned<int_type>::type;
    uint_type uint_value = static_cast<uint_type>(int_value);

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
 * Set the bits with the value parameter with the value_set parameter
 * within the bit positions specified by [bit_pos_hi:bit_pos_lo].
 *
 * @tparam int_type
 * @tparam int_set_type
 *
 * @param int_type     value
 * @param int_set_type value_set
 *
 * @param bit_pos_hi
 * @param bit_pos_lo
 *
 * @return int_type The integer value after the bits [bit_pos_hi:bit_pos_lo]
 * have been set.
 */
template <typename int_type, typename int_set_type>
inline auto value_set(int_type      int_value,
                      int_set_type  value_set,
                      bit_pos_t     bit_pos_hi,
                      bit_pos_t     bit_pos_lo) -> int_type
{
    using uint_type = typename std::make_unsigned<int_type>::type;

    // It is intentional that u_value_set is of type uint_type.
    // It is going to be shifted into position within the bounds of
    // u_value_origin so it needs to be the same.
    uint_type uint_value     = static_cast<uint_type>(int_value);
    uint_type uint_value_set = static_cast<uint_type>(value_set);

    uint_type const mask = bit_mask<uint_type>(bit_pos_hi, bit_pos_lo);
    uint_value &= ~mask;

    uint_value_set <<= bit_pos_lo;
    uint_value_set &= mask;

    uint_value |= uint_value_set;
    return static_cast<int_type>(uint_value);
}

/**
 * Based on the bit range [bit_pos_hi:bit_pos_lo] extract the integer value
 * from within the integer passed in.
 *
 * @note If the integer passed in has a signed type then the value returned
 * will also be signed (and appropriately sign extended if negative value).
 *
 * @tparam int_type
 * @param int_value  The value from which to extract bits
 * @param bit_pos_hi
 * @param bit_pos_lo
 *
 * @return int_type
 */
template <typename int_type>
inline auto value_get(int_type  int_value,
                      bit_pos_t bit_pos_hi,
                      bit_pos_t bit_pos_lo) -> int_type
{
    using uint_type = typename std::make_unsigned<int_type>::type;

    uint_type uint_value = static_cast<uint_type>(int_value);
    uint_type const mask = bit_mask<uint_type>(bit_pos_hi, bit_pos_lo);

    uint_value &= mask;
    uint_value >>= bit_pos_lo;

    if (std::is_signed<int_type>::value)
    {
        bit_pos_t const sign_pos = bit_pos_hi - bit_pos_lo;
        uint_value = sign_extend(uint_value, sign_pos);
    }

    return static_cast<int_type>(uint_value);
}

};
