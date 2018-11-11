/**
 * @file ble/nordic_ble_att.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "ble/nordic_ble_att.h"
#include "ble_gatt.h"

namespace nordic
{

uint16_t from_att_error_code(ble::att::error_code error_code)
{
    if (error_code == ble::att::error_code::success)
    {
        return BLE_GATT_STATUS_SUCCESS;
    }

    return static_cast<uint16_t>(error_code) + BLE_GATT_STATUS_ATTERR_INVALID;
}

ble::att::error_code to_att_error_code(uint16_t nordic_error_code)
{
    if (nordic_error_code == BLE_GATT_STATUS_SUCCESS)
    {
        return ble::att::error_code::success;
    }

    if ((nordic_error_code > BLE_GATT_STATUS_ATTERR_INVALID) &&
        (nordic_error_code < BLE_GATT_STATUS_ATTERR_RFU_RANGE1_BEGIN))
    {
        return static_cast<ble::att::error_code>(
            nordic_error_code - BLE_GATT_STATUS_ATTERR_INVALID);
    }

    if (nordic_error_code >= BLE_GATT_STATUS_ATTERR_RFU_RANGE1_BEGIN)
    {
        uint16_t const error_code =
            static_cast<uint16_t>(ble::att::error_code::vendor_extension_begin) +
            (nordic_error_code - BLE_GATT_STATUS_ATTERR_RFU_RANGE1_BEGIN);
        return static_cast<ble::att::error_code>(error_code);
    }

    return ble::att::error_code::unknown;
}

} // namespace nordic
