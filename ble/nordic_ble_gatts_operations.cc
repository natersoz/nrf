/**
 * @file ble/nordic_ble_gatts_operations.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "ble/nordic_ble_gatts_operations.h"
#include "ble/nordic_ble_att.h"
#include "ble/nordic_ble_gatts.h"
#include "ble_gatts.h"
#include "logger.h"
#include "project_assert.h"

namespace nordic
{

static ble::gatts::error_code to_gatts_error_code(uint16_t error_code)
{
    switch (error_code)
    {
    case NRF_SUCCESS:                      return ble::gatts::error_code::success;
    case NRF_ERROR_INVALID_STATE:          return ble::gatts::error_code::invalid_state;
    case BLE_ERROR_INVALID_CONN_HANDLE:    return ble::gatts::error_code::invalid_connection;
    case BLE_ERROR_INVALID_ATTR_HANDLE:    return ble::gatts::error_code::invalid_attribute;
    case NRF_ERROR_DATA_SIZE:              return ble::gatts::error_code::invalid_length;
    case NRF_ERROR_INVALID_ADDR:           return ble::gatts::error_code::invalid_parameter;
    case NRF_ERROR_INVALID_PARAM:          return ble::gatts::error_code::invalid_parameter;
    case NRF_ERROR_NOT_FOUND:              return ble::gatts::error_code::invalid_attribute;
    case NRF_ERROR_BUSY:                   return ble::gatts::error_code::resources_in_use;
    case NRF_ERROR_RESOURCES:              return ble::gatts::error_code::resources_exhausted;
    case BLE_ERROR_GATTS_SYS_ATTR_MISSING: return ble::gatts::error_code::missing_attribute;
    case NRF_ERROR_FORBIDDEN:              return ble::gatts::error_code::security_forbidden;
    default:
        logger::instance().error("to_gatts_error_code(%u) unknown", error_code);
        ASSERT(0);
        return ble::gatts::error_code::unknown_error;
    }
}

ble::att::length_t ble_gatts_operations::notify(
    uint16_t                connection_handle,
    uint16_t                attribute_handle,
    ble::att::length_t      offset,
    ble::att::length_t      length,
    void const*             data)
{
    uint16_t length_param = length;

    ble_gatts_hvx_params_t hvx_params = {
        .handle  = attribute_handle,
        .type    = BLE_GATT_HVX_NOTIFICATION,
        .offset  = offset,
        .p_len   = &length_param,
        .p_data  = static_cast<uint8_t const*>(data)
    };

    uint32_t const error_code = sd_ble_gatts_hvx(connection_handle, &hvx_params);

    logger& logger = logger::instance();
    if (error_code != NRF_SUCCESS)
    {
        logger.warn("notify: sd_ble_gatts_hvx(c: 0x%04x, h: 0x%04x, ptr: 0x%p, len: %u): failed: 0x%04x",
                    connection_handle, attribute_handle, data, length, error_code);
    }
    else
    {
        logger.debug("notify: sd_ble_gatts_hvx(c: 0x%04x, h: 0x%04x, ptr: 0x%p, len: %u): sent: %u",
                     connection_handle, attribute_handle, data, length, *hvx_params.p_len);
    }

    return (error_code == NRF_SUCCESS) ? *hvx_params.p_len : 0u;
}

ble::att::length_t ble_gatts_operations::indicate(
    uint16_t                connection_handle,
    uint16_t                attribute_handle,
    ble::att::length_t      offset,
    ble::att::length_t      length,
    void const*             data)
{
    uint16_t length_param = length;

    ble_gatts_hvx_params_t hvx_params = {
        .handle  = attribute_handle,
        .type    = BLE_GATT_HVX_INDICATION,
        .offset  = offset,
        .p_len   = &length_param,
        .p_data  = static_cast<uint8_t const*>(data)
    };

    uint32_t const error_code = sd_ble_gatts_hvx(connection_handle, &hvx_params);

    logger& logger = logger::instance();
    if (error_code != NRF_SUCCESS)
    {
        logger.warn("indicate: sd_ble_gatts_hvx(c: 0x%04x, h: 0x%04x, ptr: 0x%p, len: %u): failed: 0x%04x",
                    connection_handle, attribute_handle, data, length, error_code);
    }
    else
    {
        logger.debug("indicate: sd_ble_gatts_hvx(c: 0x%04x, h: 0x%04x, ptr: 0x%p, len: %u): sent: %u",
                     connection_handle, attribute_handle, data, length, *hvx_params.p_len);
    }

    return (error_code == NRF_SUCCESS)? *hvx_params.p_len : 0u;
}

ble::gatts::error_code ble_gatts_operations::read_authorize_reply(
    uint16_t                connection_handle,
    uint16_t                attribute_handle,
    ble::att::error_code    error_code,
    bool                    atttribute_update,
    ble::att::length_t      offset,
    ble::att::length_t      length,
    void const*             data)
{
    ble_gatts_rw_authorize_reply_params_t const auth_reply = {
        .type = BLE_GATTS_AUTHORIZE_TYPE_READ,
        .params =
        {
            .read = {
                .gatt_status    = from_att_error_code(error_code),
                .update         = false,
                .offset         = offset,
                .len            = length,
                .p_data         = static_cast<uint8_t const*>(data)
            }
        }
    };

    uint32_t const error_rw_auth_reply =
        sd_ble_gatts_rw_authorize_reply(connection_handle, &auth_reply);

    ASSERT(error_rw_auth_reply == NRF_SUCCESS);
    return to_gatts_error_code(error_rw_auth_reply);
}

ble::gatts::error_code ble_gatts_operations::write_authorize_reply(
    uint16_t                connection_handle,
    uint16_t                attribute_handle,
    ble::att::error_code    error_code,
    bool                    atttribute_update,
    ble::att::length_t      offset,
    ble::att::length_t      length,
    void const*             data)
{
    ble_gatts_rw_authorize_reply_params_t const auth_reply = {
        .type = BLE_GATTS_AUTHORIZE_TYPE_WRITE,
        .params =
        {
            .write = {
                .gatt_status    = from_att_error_code(error_code),
                .update         = false,
                .offset         = offset,
                .len            = length,
                .p_data         = static_cast<uint8_t const*>(data)
            }
        }
    };

    uint32_t const error_rw_auth_reply =
        sd_ble_gatts_rw_authorize_reply(connection_handle, &auth_reply);

    ASSERT(error_rw_auth_reply == NRF_SUCCESS);
    return to_gatts_error_code(error_rw_auth_reply);
}

ble::gatts::error_code ble_gatts_operations::exchange_mtu_reply(
        uint16_t                connection_handle,
        ble::att::length_t      att_mtu_length)
{
    uint32_t const error_code =
        sd_ble_gatts_exchange_mtu_reply(connection_handle, att_mtu_length);

    ASSERT(error_code == NRF_SUCCESS);
    return to_gatts_error_code(error_code);
}

ble::gatts::error_code ble_gatts_operations::service_add(
    ble::gatt::service& service)
{
    uint32_t const error_code = gatts_service_add(service);
    ASSERT(error_code == NRF_SUCCESS);
    return to_gatts_error_code(error_code);
}

} // namespace nordic

