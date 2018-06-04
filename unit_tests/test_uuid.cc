/**
 * @file test_uuid.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "ble/uuid.h"
#include <boost/uuid/uuid_io.hpp>
#include <cstdint>
#include <iostream>
#include <strstream>
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

int main()
{
    int result = 0;

    ble::att::uuid const uuid_test_ble_1 = 0x2800u;
    ble::att::uuid const uuid_test_ble_2 = 0x2801u;

    char uuid_char_buffer_1[ble::att::uuid::conversion_length];
    char uuid_char_buffer_2[ble::att::uuid::conversion_length];
    uuid_test_ble_1.to_chars(uuid_char_buffer_1, uuid_char_buffer_1 + ble::att::uuid::conversion_length);
    uuid_test_ble_2.to_chars(uuid_char_buffer_2, uuid_char_buffer_2 + ble::att::uuid::conversion_length);

    std::ostrstream str_stream_1;
    std::ostrstream str_stream_2;

    str_stream_1 << uuid_test_ble_1;
    str_stream_2 << uuid_test_ble_2;

    assert(strncmp(str_stream_1.str(), uuid_char_buffer_1, ble::att::uuid::conversion_length) == 0);
    assert(strncmp(str_stream_2.str(), uuid_char_buffer_2, ble::att::uuid::conversion_length) == 0);

    if (debug_print)
    {
        std::cout << "0x2800: " << uuid_test_ble_1 << std::endl;
        std::cout << "0x2801: " << uuid_test_ble_2 << std::endl;

        printf("0x2800: %s to_chars()\n", uuid_char_buffer_1);
        printf("0x2801: %s to_chars()\n", uuid_char_buffer_2);
    }

    if (debug_print)
    {
        std::cout << "test: " << "sizeof(ble::att::uuid) == 16u" << std::endl;
    }

    if (not(sizeof(ble::att::uuid) == 16u))
    {
        result = -1;
        assert(false);
    }

    if (debug_print)
    {
        std::cout << "test: " << uuid_test_ble_1 << " ,  " << uuid_test_ble_2 << " is_ble() == true" << std::endl;
    }

    if (not(uuid_test_ble_1.is_ble() && uuid_test_ble_2.is_ble()))
    {
        result = -1;
        assert(false);
    }

    if (debug_print)
    {
        std::cout << "test: " << uuid_test_ble_1 << " <  " << uuid_test_ble_2 << " == true " << std::endl;
    }

    if (not (uuid_test_ble_1 < uuid_test_ble_2))
    {
        result = -1;
        assert(false);
    }

    if (debug_print)
    {
        std::cout << "test: " << uuid_test_ble_1 << " >  " << uuid_test_ble_2 << " == false " << std::endl;
    }

    if (uuid_test_ble_1 > uuid_test_ble_2)
    {
        result = -1;
        assert(false);
    }

    if (debug_print)
    {
        std::cout << "test: " << uuid_test_ble_1 << " >= " << uuid_test_ble_2 << " == false " << std::endl;
    }

    if (uuid_test_ble_1 >= uuid_test_ble_2)
    {
        result = -1;
        assert(false);
    }

    if (debug_print)
    {
        std::cout << "test: " << uuid_test_ble_1 << " == " << uuid_test_ble_2 << " == false " << std::endl;
    }

    if (uuid_test_ble_1 == uuid_test_ble_2)
    {
        result = -1;
        assert(false);
    }

    if (debug_print)
    {
        std::cout << "test: " << uuid_test_ble_1 << " != " << uuid_test_ble_2 << " == true " << std::endl;
    }

    if (not(uuid_test_ble_1 != uuid_test_ble_2))
    {
        result = -1;
        assert(false);
    }

    ble::att::uuid const uuid_test_1 = {{{
        0x12, 0x34, 0x56, 0x78,
        0x9a, 0xbc,
        0xde, 0xf0,
        0xa1, 0xb2,
        0xc3, 0xd4, 0xca, 0xfe, 0xba, 0xbe
    }}};

    ble::att::uuid const uuid_test_rev = uuid_test_1.reverse();

    if (debug_print)
    {
        std::cout << "test: " << uuid_test_1 << ".reverse() == " << uuid_test_rev << std::endl;
    }

    if (not(uuid_test_rev.reverse() == uuid_test_1))
    {
        result = -1;
        assert(false);
    }

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
