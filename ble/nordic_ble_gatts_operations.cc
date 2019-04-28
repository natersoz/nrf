/**
 * @file ble/nordic_ble_gatts_operations.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "ble/nordic_ble_gatts_operations.h"
#include "ble/nordic_ble_att.h"
#include "ble/nordic_ble_gatts.h"
#include "nordic_error.h"
#include "logger.h"
#include "project_assert.h"

#include <ble_gatts.h>

namespace nordic
{

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

std::errc ble_gatts_operations::read_authorize_reply(
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
    return nordic_to_system_error(error_rw_auth_reply);
}

std::errc ble_gatts_operations::write_authorize_reply(
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
    return nordic_to_system_error(error_rw_auth_reply);
}

std::errc ble_gatts_operations::exchange_mtu_reply(
        uint16_t                connection_handle,
        ble::att::length_t      att_mtu_length)
{
    uint32_t const error_code =
        sd_ble_gatts_exchange_mtu_reply(connection_handle, att_mtu_length);

    ASSERT(error_code == NRF_SUCCESS);
    return nordic_to_system_error(error_code);
}

std::errc ble_gatts_operations::service_add(
    ble::gatt::service& service)
{
    uint32_t const error_code = gatts_service_add(service);
    ASSERT(error_code == NRF_SUCCESS);
    return nordic_to_system_error(error_code);
}

} // namespace nordic

