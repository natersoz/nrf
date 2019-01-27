/**
 * @file test_bit_manip.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "gtest/gtest.h"
#include "bit_manip.h"

#include <cstdint>
#include <iostream>
#include <iomanip>

static constexpr bool debug_print = false;

inline std::ostream &operator<<(std::ostream &os, int8_t value)
{
    return os << static_cast<int>(value);
}

inline std::ostream &operator<<(std::ostream &os, uint8_t value)
{
    return os << static_cast<unsigned int>(value);
}

template <typename int_type, typename int_set_type>
void test_bit_manip_set(int_type                int_value,
                        int_set_type            value_set,
                        bit_manip::bit_pos_t    bit_pos_hi,
                        bit_manip::bit_pos_t    bit_pos_lo)
{
    int_type const result_value = bit_manip::value_set(int_value,
                                                       value_set,
                                                       bit_pos_hi,
                                                       bit_pos_lo);

    int_set_type const value_get = bit_manip::value_get(result_value,
                                                        bit_pos_hi,
                                                        bit_pos_lo);

    int_set_type const comp_mask =
        bit_manip::bit_mask<int_type>(bit_pos_hi, bit_pos_lo) >> bit_pos_lo;

    if (debug_print)
    {
        std::cout << "-------------------------------" << std::endl;
        std::cout << "val: 0x" << std::hex << std::setfill('0') << std::setw(8u)
                  << int_value << std::endl;
        std::cout << "set: 0x" << std::hex << std::setfill('0') << std::setw(8u)
                  << value_set;
        std::cout << " (" << std::dec << bit_pos_hi;
        std::cout << ", " << std::dec << bit_pos_lo << ")" << std::endl;

        std::cout << "res: 0x" << std::hex << std::setfill('0') << std::setw(8u)
                  << result_value << std::endl;
        std::cout << "get: 0x" << std::hex << std::setfill('0') << std::setw(8u)
                  << value_get << std::endl;

        std::cout << std::endl;
    }

    ASSERT_EQ(value_get, (value_set & comp_mask));
}

TEST(BitManipSet_32, Width_8)
{
    bit_manip::bit_pos_t bit_width = 8u;
    for (bit_manip::bit_pos_t bit_lsb = 0u; bit_lsb + bit_width < 32u; ++bit_lsb)
    {
        uint32_t const test_value = 0x12345678;
        uint8_t  const test_set   = 0x12;
        test_bit_manip_set(test_value, test_set, bit_lsb + bit_width, bit_lsb);
    }
}

TEST(BitManipSet_32, Width_13)
{
    bit_manip::bit_pos_t bit_width = 13u;
    for (bit_manip::bit_pos_t bit_lsb = 0u; bit_lsb + bit_width < 32u; ++bit_lsb)
    {
        uint32_t const test_value = 0x12345678;
        uint8_t  const test_set   = 0x12;
        test_bit_manip_set(test_value, test_set, bit_lsb + bit_width, bit_lsb);
    }
}

TEST(BitManipSet_32, Width_27)
{
    bit_manip::bit_pos_t bit_width = 27u;
    for (bit_manip::bit_pos_t bit_lsb = 0u; bit_lsb + bit_width < 32u; ++bit_lsb)
    {
        uint32_t const test_value = 0x12345678;
        uint8_t  const test_set   = 0x12;
        test_bit_manip_set(test_value, test_set, bit_lsb + bit_width, bit_lsb);
    }
}

TEST(BitManipSet_64, Width_57)
{
    bit_manip::bit_pos_t bit_width = 57u;
    for (bit_manip::bit_pos_t bit_lsb = 0u; bit_lsb + bit_width < 64u; ++bit_lsb)
    {
        uint64_t const test_value = 0x1234567812345678ULL;
        uint64_t const test_set   = 0xabcdef12345abcdeUL;
        test_bit_manip_set(test_value, test_set, bit_lsb + bit_width, bit_lsb);
    }
}

TEST(BitManipSet_64, Width_17)
{
    bit_manip::bit_pos_t bit_width = 17u;
    for (bit_manip::bit_pos_t bit_lsb = 0u; bit_lsb + bit_width < 64u; ++bit_lsb)
    {
        uint64_t const test_value = 0xabcdef012345678ULL;
        uint64_t const test_set   = 0xabcdef12345abcdeUL;
        test_bit_manip_set(test_value, test_set, bit_lsb + bit_width, bit_lsb);
    }
}

TEST(BitManipSignExtend_32, Value_0x0000)
{
    int32_t test_value = 0x80000000;
    int32_t test_sign_ext = bit_manip::sign_extend(test_value, 31u);

    ASSERT_EQ(test_value, test_sign_ext);
}

TEST(BitManipSignExtend_32, Value_0xFFFF0000)
{
    int32_t test_value = 0x00008000;
    int32_t expected_value = 0xFFFF8000;
    int32_t test_sign_ext = bit_manip::sign_extend(test_value, 15u);

    ASSERT_EQ(expected_value, test_sign_ext);
}

TEST(BitManipSignExtend_32, Value_0x000040000)
{
    int32_t test_value = 0x00004000;
    int32_t expected_value = 0xFFFFC000;
    int32_t test_sign_ext = bit_manip::sign_extend(test_value, 14u);

    ASSERT_EQ(expected_value, test_sign_ext);
}

TEST(BitManipSignExtend_32, Value_0x00007FFF)
{
    int32_t test_value = 0x00007FFF;
    int32_t expected_value = -1;
    int32_t test_sign_ext = bit_manip::sign_extend(test_value, 14u);

    ASSERT_EQ(expected_value, test_sign_ext);
}

TEST(BitManipSignExtend_64, Value_0x000000000)
{
    int64_t test_value = 0x80000000ULL;
    int64_t test_sign_ext = bit_manip::sign_extend(test_value, 63u);

    ASSERT_EQ(test_value, test_sign_ext);
}
