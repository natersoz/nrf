/**
 * @file test_make_array.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "fixed_allocator.h"
#include <vector>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <cassert>

static bool const debug_print = false;

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

int main()
{
    int data[std::size(test_data)];
    fixed_allocator<int> test_allocator(data, std::size(data));
    std::vector<int, fixed_allocator<int>> test_vector(test_allocator);

    test_vector.reserve(std::size(data));

    for (int test_data_value : test_data)
    {
        test_vector.push_back(test_data_value);
    }

    bool const result_placement = (test_vector.data() == data);
    bool const result_size      = (test_vector.size() == std::size(test_data));
    bool const result_contents  = memcmp(test_vector.data(), data, test_vector.size()) == 0;

    assert(result_placement);
    assert(result_size);
    if (not result_contents || debug_print)
    {
        std::cout << "test_vector: " << test_vector << std::endl;
    }
    assert(result_contents);

    int result = (result_placement && result_size && result_contents) ? 0 : -1;

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
