/**
 * @file test_bit_manip.cc
 */

#include "bit_manip.h"
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <cassert>

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
bool test_bit_manip_set(int_type                int_value,
                        int_set_type            value_set,
                        bit_manip::bit_pos_t    bit_pos_hi,
                        bit_manip::bit_pos_t    bit_pos_lo)
{
    bool result = true;
    int_type const result_value = bit_manip::value_set(int_value,
                                                       value_set,
                                                       bit_pos_hi,
                                                       bit_pos_lo);

    int_set_type const value_get = bit_manip::value_get(result_value,
                                                        bit_pos_hi,
                                                        bit_pos_lo);

    int_set_type const comp_mask = bit_manip::bit_mask<int_type>(bit_pos_hi, bit_pos_lo) >> bit_pos_lo;

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


    if (value_get != (value_set & comp_mask))
    {
        result = false;
        assert(0);
    }

    return result;
}

static bool test_bit_manip_set_u32()
{
    bool result = true;

    {
        bit_manip::bit_pos_t bit_width = 8u;
        for (bit_manip::bit_pos_t bit_lsb = 0u; bit_lsb + bit_width < 32u; ++bit_lsb)
        {
            uint32_t const test_value = 0x12345678;
            uint8_t  const test_set   = 0x12;
            result = test_bit_manip_set(test_value, test_set, bit_lsb + bit_width, bit_lsb);
        }
    }

    {
        bit_manip::bit_pos_t bit_width = 13u;
        for (bit_manip::bit_pos_t bit_lsb = 0u; bit_lsb + bit_width < 32u; ++bit_lsb)
        {
            uint32_t const test_value = 0x12345678;
            uint8_t  const test_set   = 0x12;
            result = test_bit_manip_set(test_value, test_set, bit_lsb + bit_width, bit_lsb);
        }
    }

    {
        bit_manip::bit_pos_t bit_width = 27u;
        for (bit_manip::bit_pos_t bit_lsb = 0u; bit_lsb + bit_width < 32u; ++bit_lsb)
        {
            uint32_t const test_value = 0x12345678;
            uint8_t  const test_set   = 0x12;
            result = test_bit_manip_set(test_value, test_set, bit_lsb + bit_width, bit_lsb);
        }
    }

    return result;
}

static bool test_bit_manip_set_u64()
{
    bool result = true;

    {
        bit_manip::bit_pos_t bit_width = 57u;
        for (bit_manip::bit_pos_t bit_lsb = 0u; bit_lsb + bit_width < 64u; ++bit_lsb)
        {
            uint64_t const test_value = 0x1234567812345678ull;
            uint64_t const test_set   = 0xabcdef12345abcdeul;
            test_bit_manip_set(test_value, test_set, bit_lsb + bit_width, bit_lsb);
        }
    }

    {
        bit_manip::bit_pos_t bit_width = 17u;
        for (bit_manip::bit_pos_t bit_lsb = 0u; bit_lsb + bit_width < 64u; ++bit_lsb)
        {
            uint64_t const test_value = 0xabcdef012345678ull;
            uint64_t const test_set   = 0xabcdef12345abcdeul;
            test_bit_manip_set(test_value, test_set, bit_lsb + bit_width, bit_lsb);
        }
    }

    return result;
}

static bool test_bit_manip_sign_extend_u32()
{
    bool result = true;

    {
        int32_t test_value = 0x80000000;
        int32_t test_sign_ext = bit_manip::sign_extend(test_value, 31u);

        if (test_value != test_sign_ext)
        {
            assert(0);
            result = false;
        }
    }

    {
        int32_t test_value = 0x00008000;
        int32_t expected_value = 0xFFFF8000;
        int32_t test_sign_ext = bit_manip::sign_extend(test_value, 15u);

        if (expected_value != test_sign_ext)
        {
            assert(0);
            result = false;
        }
    }

    {
        int32_t test_value = 0x00004000;
        int32_t expected_value = 0xFFFFC000;
        int32_t test_sign_ext = bit_manip::sign_extend(test_value, 14u);

        if (expected_value != test_sign_ext)
        {
            assert(0);
            result = false;
        }
    }

    {
        int32_t test_value = 0x00007FFF;
        int32_t expected_value = -1;
        int32_t test_sign_ext = bit_manip::sign_extend(test_value, 14u);

        if (expected_value != test_sign_ext)
        {
            assert(0);
            result = false;
        }
    }

    return result;
}

static bool test_bit_manip_sign_extend_u64()
{
    bool result = true;

        int64_t test_value = 0x80000000ull;
        int64_t test_sign_ext = bit_manip::sign_extend(test_value, 63u);

        if (test_value != test_sign_ext)
        {
            assert(0);
            result = false;
        }

    return result;
}


int main()
{
    bool result_sign_extend_u32 = test_bit_manip_sign_extend_u32();
    bool result_sign_extend_u64 = test_bit_manip_sign_extend_u64();

    bool result_bit_manip_set_u32 = test_bit_manip_set_u32();
    bool result_bit_manip_set_u64 = test_bit_manip_set_u64();

    int const result = (result_sign_extend_u32   &&
                        result_sign_extend_u64   &&
                        result_bit_manip_set_u32 &&
                        result_bit_manip_set_u64 &&
                        true) ? 0 : -1;

    if (result == 0)
    {
        std::cout << "--- Tests Pass" << std::endl;
    }
    else
    {
        std::cout << "--- Tests FAIL" << std::endl;
    }
    return result;
}
