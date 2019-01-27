/**
 * @file test_make_array.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "gtest/gtest.h"
#include "make_array.h"

#include <cstdint>
#include <cstring>
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

template<typename data_type, std::size_t data_size>
std::ostream &operator<<(std::ostream &os, std::array<data_type, data_size> const &array)
{
    os << '{';
    for (auto const &node : array)
    {
        os << node;
        if (&node != array.end() - 1u)
        {
            os << ", ";
        }
    }
    os << '}';

    return os;
}

// Note: test data is global since test_to_array will need to see it
// as an array and not a pointer.
static uint8_t const test_data[] = {
    0x00u, 0x01u, 0x02u, 0x03u, 0x04u, 0x05u, 0x06u, 0x07u,
    0x08u, 0x09u, 0x0au, 0x0bu, 0x0cu, 0x0du, 0x0eu, 0x0fu,
    0x10u, 0x11u,
};

TEST(MakeArray, MakeArray)
{
    std::array<uint8_t, sizeof(test_data)> test_array =
        utility::make_array<uint8_t, sizeof(test_data)>(test_data);

    if (debug_print)
    {
        std::cout << "test_mk_array: " << test_array << std::endl;
    }

    ASSERT_EQ(memcmp(test_data, test_array.data(), test_array.size()), 0);
    ASSERT_EQ(std::size(test_data), test_array.size());
}

TEST(MakeArray, ToArray)
{
    std::array<uint8_t, sizeof(test_data)> test_array =
        utility::to_array(test_data);

    if (debug_print)
    {
        std::cout << "test_to_array: " << test_array << std::endl;
    }

    ASSERT_EQ(memcmp(test_data, test_array.data(), test_array.size()), 0);
    ASSERT_EQ(std::size(test_data), test_array.size());
}
