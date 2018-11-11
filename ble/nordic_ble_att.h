/**
 * @file ble/nordic_ble_att.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "ble/att.h"
#include <cstdint>

namespace nordic
{

uint16_t from_att_error_code(ble::att::error_code error_code);

ble::att::error_code to_att_error_code(uint16_t nordic_error_code);

} // namespace nordic
