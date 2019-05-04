/**
 * @file test_fixed_allocator.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "gtest/gtest.h"
#include "fixed_allocator.h"

#include <vector>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <cassert>


// For debugging
template<typename data_type, typename allocator_type>
std::ostream &operator<<(std::ostream &os, std::vector<data_type, allocator_type> const &vector)
{
    os << "{ ";
    for (auto const &node : vector)
    {
        os << node;
        if (&node != &*vector.end() - 1u)
        {
            os << ", ";
        }
    }
    os << " }";

    return os;
}

// Note: test data is global since test_to_array will need to see it
// as an array and not a pointer.
static int const test_data[] = {
    0x00u, 0x01u, 0x02u, 0x03u, 0x04u, 0x05u, 0x06u, 0x07u,
    0x08u, 0x09u, 0x0au, 0x0bu, 0x0cu, 0x0du, 0x0eu, 0x0fu,
    0x10u, 0x11u,
};

TEST(FixedAllocator, Allocate)
{
    int data[std::size(test_data)];
    fixed_allocator<int> test_allocator(data, std::size(data));
    std::vector<int, fixed_allocator<int>> test_vector(test_allocator);

    test_vector.reserve(std::size(data));

    test_vector.insert(test_vector.begin(),
                       std::begin(test_data),
                       std::end(test_data));

    // Did the allocator actually use the data assigned to it?
    ASSERT_TRUE(std::equal(test_vector.begin(), test_vector.end(), test_data));
    ASSERT_EQ(test_vector.size(), std::size(test_data));
}
