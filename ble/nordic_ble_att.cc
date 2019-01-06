/**
 * @file ble/nordic_ble_att.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "ble/uuid.h"
#include "ble/nordic_ble_att.h"
#include "logger.h"
#include "project_assert.h"

#include "ble.h"
#include "ble_gatt.h"

namespace nordic
{

ble::att::uuid to_att_uuid(ble_uuid_t const& nrf_uuid)
{
    if (nrf_uuid.type == BLE_UUID_TYPE_BLE)
    {
        return ble::att::uuid(nrf_uuid.uuid);
    }

    ble::att::uuid uuid;
    uint8_t uuid_len = 0u;
    sd_ble_uuid_encode(&nrf_uuid, &uuid_len, uuid.data);
    return uuid.reverse();
}

ble::att::uuid to_att_uuid(ble_uuid128_t const& nrf_uuid)
{
    return ble::att::uuid(nrf_uuid.uuid128);
}

ble_uuid128_t from_att_uuid_128(ble::att::uuid const& att_uuid)
{
    ble_uuid128_t const uuid_128 = {
        .uuid128 = {
            att_uuid.data[15u], att_uuid.data[14u], att_uuid.data[13u], att_uuid.data[12u],
            att_uuid.data[11u], att_uuid.data[10u],
            att_uuid.data[ 9u], att_uuid.data[ 8u],
            att_uuid.data[ 7u], att_uuid.data[ 6u],
            att_uuid.data[ 5u], att_uuid.data[ 4u], att_uuid.data[ 3u],
            att_uuid.data[ 2u], att_uuid.data[ 1u], att_uuid.data[ 0u]
        }
    };

    return uuid_128;
}

ble_uuid_t from_att_uuid_16(ble::att::uuid const& uuid)
{
    ASSERT(uuid.is_ble());

    ble_uuid_t const nrf_uuid = {
        .uuid = uuid.get_u16(),
        .type = BLE_UUID_TYPE_BLE
    };

    return nrf_uuid;
}

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

ble::gatt::properties to_att_properties(ble_gatt_char_props_t     props,
                                        ble_gatt_char_ext_props_t props_ext)
{
    uint16_t const prop_bits =
        (props.broadcast       ? ble::gatt::properties::broadcast              : 0u) |
        (props.read            ? ble::gatt::properties::read                   : 0u) |
        (props.write_wo_resp   ? ble::gatt::properties::write_without_response : 0u) |
        (props.write           ? ble::gatt::properties::write                  : 0u) |
        (props.notify          ? ble::gatt::properties::notify                 : 0u) |
        (props.indicate        ? ble::gatt::properties::indicate               : 0u) |
        (props.auth_signed_wr  ? ble::gatt::properties::write_with_signature   : 0u) |
        (props_ext.reliable_wr ? ble::gatt::properties::write_reliable         : 0u) |
        (props_ext.wr_aux      ? ble::gatt::properties::write_aux              : 0u) ;

    ble::gatt::properties const properties(prop_bits);
    return properties;
}

ble::gatt::properties to_att_properties(ble_gatt_char_props_t props)
{
    uint16_t const prop_bits =
        (props.broadcast       ? ble::gatt::properties::broadcast              : 0u) |
        (props.read            ? ble::gatt::properties::read                   : 0u) |
        (props.write_wo_resp   ? ble::gatt::properties::write_without_response : 0u) |
        (props.write           ? ble::gatt::properties::write                  : 0u) |
        (props.notify          ? ble::gatt::properties::notify                 : 0u) |
        (props.indicate        ? ble::gatt::properties::indicate               : 0u) |
        (props.auth_signed_wr  ? ble::gatt::properties::write_with_signature   : 0u) ;

    ble::gatt::properties const properties(prop_bits);
    return properties;
}

ble::att::op_code to_att_write_op_code(uint8_t nordic_gatt_write_op)
{
    switch (nordic_gatt_write_op)
    {
    case  BLE_GATT_OP_INVALID:          return ble::att::op_code::invalid;
    case  BLE_GATT_OP_WRITE_REQ:        return ble::att::op_code::write_request;
    case  BLE_GATT_OP_WRITE_CMD:        return ble::att::op_code::write_command;
    case  BLE_GATT_OP_SIGN_WRITE_CMD:   return ble::att::op_code::write_signed_command;
    case  BLE_GATT_OP_PREP_WRITE_REQ:   return ble::att::op_code::write_prepare_request;
    case  BLE_GATT_OP_EXEC_WRITE_REQ:   return ble::att::op_code::write_execute_request;
    default:                            return ble::att::op_code::invalid;
    }
}

#if 0   /// @todo remove once tested.
ble::att::error_code nordic_gatts_to_att_error(uint16_t nordic_error)
{
    if (nordic_error == BLE_GATT_STATUS_SUCCESS)
    {
        return ble::att::error_code::success;
    }

    if ((nordic_error == BLE_GATT_STATUS_UNKNOWN) ||
        (nordic_error == BLE_GATT_STATUS_ATTERR_INVALID))
    {
        return ble::att::error_code::unknown;
    }

    if ((nordic_error > BLE_GATT_STATUS_ATTERR_INVALID) &&
        (nordic_error < BLE_GATT_STATUS_ATTERR_RFU_RANGE1_BEGIN))
    {
        return static_cast<ble::att::error_code>(nordic_error - BLE_GATT_STATUS_ATTERR_INVALID);
    }

    uint16_t vendor_specific = nordic_error - BLE_GATT_STATUS_ATTERR_INVALID;
    vendor_specific += static_cast<uint16_t>(ble::att::error_code::vendor_extension_begin);
    return static_cast<ble::att::error_code>(vendor_specific);
}

/** @brief Encode a @ref ble_uuid_t structure into little endian raw UUID bytes (16-bit or 128-bit).
 *
 * @note The pointer to the destination buffer p_uuid_le may be NULL, in which case only the validity and size of p_uuid is computed.
 *
 * @param[in]   p_uuid        Pointer to a @ref ble_uuid_t structure that will be encoded into bytes.
 * @param[out]  p_uuid_le_len Pointer to a uint8_t that will be filled with the encoded length (2 or 16 bytes).
 * @param[out]  p_uuid_le     Pointer to a buffer where the little endian raw UUID bytes (2 or 16) will be stored.
 *
 * @retval ::NRF_SUCCESS Successfully encoded into the buffer.
 * @retval ::NRF_ERROR_INVALID_ADDR Invalid pointer supplied.
 * @retval ::NRF_ERROR_INVALID_PARAM Invalid UUID type.
 */
uint32_t sd_ble_uuid_encode(ble_uuid_t const *p_uuid,
                            uint8_t *p_uuid_le_len, uint8_t *p_uuid_le));

#endif

} // namespace nordic
