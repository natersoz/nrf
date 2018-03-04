/**
 * @file int_to_string.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 * Integer to C language string conversions.
 */

#include <cstddef>
#include <cstring>

#include "int_to_string.h"

char nybble_to_char(uint8_t nybble_value, bool upper_case)
{
    nybble_value &= 0x0Fu;
    if (nybble_value < 0x0Au)
    {
        return '0' + nybble_value;
    }
    else
    {
        return (nybble_value - 0x0Au) + (upper_case? 'A' : 'a');
    }
}

