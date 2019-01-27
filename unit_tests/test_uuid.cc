/**
 * @file test_uuid.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "gtest/gtest.h"
#include "ble/uuid.h"
#include <boost/uuid/uuid_io.hpp>

#include <cstdint>
#include <iostream>
#include <sstream>
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

TEST(UUID, BLE_compare)
{
    ble::att::uuid const uuid_test_ble_1 = 0x2800u;
    ble::att::uuid const uuid_test_ble_2 = 0x2801u;

    char uuid_char_buffer_1[ble::att::uuid::conversion_length];
    char uuid_char_buffer_2[ble::att::uuid::conversion_length];
    uuid_test_ble_1.to_chars(uuid_char_buffer_1, uuid_char_buffer_1 + ble::att::uuid::conversion_length);
    uuid_test_ble_2.to_chars(uuid_char_buffer_2, uuid_char_buffer_2 + ble::att::uuid::conversion_length);

    std::ostringstream str_stream_1;
    std::ostringstream str_stream_2;

    str_stream_1 << uuid_test_ble_1;
    str_stream_2 << uuid_test_ble_2;

    ASSERT_EQ(str_stream_1.str(), uuid_char_buffer_1);
    ASSERT_EQ(str_stream_2.str(), uuid_char_buffer_2);

    if (debug_print)
    {
        std::cout << "0x2800: " << uuid_test_ble_1 << std::endl;
        std::cout << "0x2801: " << uuid_test_ble_2 << std::endl;

        std::cout << "0x2800: to_chars(): " << uuid_char_buffer_1 << std::endl;
        std::cout << "0x2801: to_chars(): " << uuid_char_buffer_2 << std::endl;
    }

    ASSERT_EQ(uuid_test_ble_1.end() - uuid_test_ble_1.begin(),16u);
    ASSERT_TRUE(uuid_test_ble_1.is_ble());
    ASSERT_TRUE(uuid_test_ble_2.is_ble());

    ASSERT_TRUE( uuid_test_ble_2  > uuid_test_ble_1);
    ASSERT_TRUE( uuid_test_ble_2 >= uuid_test_ble_1);
    ASSERT_FALSE(uuid_test_ble_2 <  uuid_test_ble_1);
    ASSERT_FALSE(uuid_test_ble_2 <= uuid_test_ble_1);
    ASSERT_FALSE(uuid_test_ble_2 == uuid_test_ble_1);
    ASSERT_TRUE( uuid_test_ble_2 != uuid_test_ble_1);
}

TEST(UUID, UUID_reverse)
{
    ble::att::uuid const uuid_test = {{{
        0x12, 0x34, 0x56, 0x78,
        0x9a, 0xbc,
        0xde, 0xf0,
        0xa1, 0xb2,
        0xc3, 0xd4, 0xca, 0xfe, 0xba, 0xbe
    }}};

    ble::att::uuid const uuid_test_rev = uuid_test.reverse();
    ASSERT_EQ(uuid_test_rev.reverse(), uuid_test);
}
