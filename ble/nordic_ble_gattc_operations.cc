/**
 * @file ble/nordic_ble_gattc_operations.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "ble/nordic_ble_gattc_operations.h"
#include "logger.h"
#include "nordic_error.h"

#include "ble_gatt.h"
#include "ble_gattc.h"

namespace nordic
{

std::errc ble_gattc_service_discovery::discover_primary_services(
    uint16_t connection_handle,
    uint16_t gatt_handle_start)
{
    uint32_t const error_code = sd_ble_gattc_primary_services_discover(
        connection_handle,
        gatt_handle_start,
        nullptr);

    if (error_code != NRF_SUCCESS)
    {
        logger::instance().error(
            "sd_ble_gattc_primary_services_discover("
            "c: 0x%04x, h: 0x%04x) failed: 0x%04x '%s'",
            connection_handle, gatt_handle_start,
            error_code, nordic_error_string(error_code));
    }

    return nordic_to_system_error(error_code);
}

std::errc ble_gattc_service_discovery::discover_primary_services(
    uint16_t connection_handle,
    uint16_t gatt_handle_start,
    ble::att::uuid const& uuid)
{
    /// @todo Figure out how to wedge in Nordic's UUID here:
    uint32_t const error_code = sd_ble_gattc_primary_services_discover(
        connection_handle,
        gatt_handle_start,
        nullptr);

    if (error_code != NRF_SUCCESS)
    {
        logger::instance().error(
            "sd_ble_gattc_primary_services_discover("
            "c: 0x%04x, h: 0x%04x) failed: 0x%04x '%s'",
            connection_handle, gatt_handle_start,
            error_code, nordic_error_string(error_code));
    }

    return nordic_to_system_error(error_code);
}

std::errc ble_gattc_service_discovery::discover_service_relationships(
    uint16_t            connection_handle,
    uint16_t            gatt_handle_start,
    uint16_t            gatt_handle_stop)
{
    ble_gattc_handle_range_t const gatt_handle_range = {
        .start_handle = gatt_handle_start,
        .end_handle   = gatt_handle_stop
    };

    uint32_t const error_code = sd_ble_gattc_relationships_discover(
        connection_handle, &gatt_handle_range);

    if (error_code != NRF_SUCCESS)
    {
        logger::instance().error(
            "sd_ble_gattc_relationships_discover("
            "c: 0x%04x, h: [0x%04x:0x%04x]) failed: 0x%04x '%s'",
            connection_handle, gatt_handle_start, gatt_handle_stop,
            error_code, nordic_error_string(error_code));
    }

    return nordic_to_system_error(error_code);
}

std::errc ble_gattc_service_discovery::discover_characteristics(
    uint16_t            connection_handle,
    uint16_t            gatt_handle_start,
    uint16_t            gatt_handle_stop)
{
    ble_gattc_handle_range_t const gatt_handle_range = {
        .start_handle = gatt_handle_start,
        .end_handle   = gatt_handle_stop
    };

    uint32_t const error_code = sd_ble_gattc_characteristics_discover(
        connection_handle, &gatt_handle_range);

    if (error_code != NRF_SUCCESS)
    {
        logger::instance().error(
            "sd_ble_gattc_characteristics_discover("
            "c: 0x%04x, h: [0x%04x:0x%04x]) failed: 0x%04x '%s'",
            connection_handle, gatt_handle_start, gatt_handle_stop,
            error_code, nordic_error_string(error_code));
    }

    return nordic_to_system_error(error_code);
}

std::errc ble_gattc_service_discovery::discover_descriptors(
    uint16_t            connection_handle,
    uint16_t            gatt_handle_start,
    uint16_t            gatt_handle_stop)
{
    ble_gattc_handle_range_t const gatt_handle_range = {
        .start_handle = gatt_handle_start,
        .end_handle   = gatt_handle_stop
    };

    uint32_t const error_code = sd_ble_gattc_descriptors_discover(
        connection_handle, &gatt_handle_range);

    if (error_code != NRF_SUCCESS)
    {
        logger::instance().error(
            "sd_ble_gattc_descriptors_discover("
            "c: 0x%04x, h: [0x%04x:0x%04x]) failed: 0x%04x '%s'",
            connection_handle, gatt_handle_start, gatt_handle_stop,
            error_code, nordic_error_string(error_code));
    }

    return nordic_to_system_error(error_code);
}

std::errc ble_gattc_service_discovery::discover_attributes(
    uint16_t            connection_handle,
    uint16_t            gatt_handle_start,
    uint16_t            gatt_handle_stop)
{
    ble_gattc_handle_range_t const gatt_handle_range = {
        .start_handle = gatt_handle_start,
        .end_handle   = gatt_handle_stop
    };

    uint32_t const error_code = sd_ble_gattc_attr_info_discover(
        connection_handle, &gatt_handle_range);

    if (error_code != NRF_SUCCESS)
    {
        logger::instance().error(
            "sd_ble_gattc_attr_info_discover("
            "c: 0x%04x, h: [0x%04x:0x%04x]) failed: 0x%04x '%s'",
            connection_handle, gatt_handle_start, gatt_handle_stop,
            error_code, nordic_error_string(error_code));
    }

    return nordic_to_system_error(error_code);
}

std::errc ble_gattc_operations::read(
    uint16_t            connection_handle,
    uint16_t            attribute_handle,
    ble::att::length_t  offset)
{
    uint32_t error_code = sd_ble_gattc_read(connection_handle,
                                            attribute_handle,
                                            offset);
    if (error_code != NRF_SUCCESS)
    {
        logger::instance().error("sd_ble_gattc_read() failed: 0x%04x '%s'",
                                 error_code, nordic_error_string(error_code));
    }

    return nordic_to_system_error(error_code);
}

std::errc ble_gattc_operations::write_request(
    uint16_t            connection_handle,
    uint16_t            attribute_handle,
    void const*         data,
    ble::att::length_t  offset,
    ble::att::length_t  length)
{
    ble_gattc_write_params_t const write_params = {
        .write_op   = BLE_GATT_OP_WRITE_REQ,
        .flags      = BLE_GATT_EXEC_WRITE_FLAG_PREPARED_WRITE,
        .handle     = attribute_handle,
        .offset     = offset,
        .len        = length,
        .p_value    = reinterpret_cast<uint8_t const*>(data)
    };

    uint32_t error_code = sd_ble_gattc_write(connection_handle, &write_params);
    if (error_code != NRF_SUCCESS)
    {
        logger::instance().error("sd_ble_gattc_write(REQ) failed: 0x%04x '%s'",
                                 error_code, nordic_error_string(error_code));
    }

    return nordic_to_system_error(error_code);
}

std::errc ble_gattc_operations::write_command(
    uint16_t            connection_handle,
    uint16_t            attribute_handle,
    void const*         data,
    ble::att::length_t  offset,
    ble::att::length_t  length)
{
    ble_gattc_write_params_t const write_params = {
        .write_op   = BLE_GATT_OP_WRITE_CMD,
        .flags      = BLE_GATT_EXEC_WRITE_FLAG_PREPARED_WRITE,
        .handle     = attribute_handle,
        .offset     = offset,
        .len        = length,
        .p_value    = reinterpret_cast<uint8_t const*>(data)
    };

    uint32_t error_code = sd_ble_gattc_write(connection_handle, &write_params);
    if (error_code != NRF_SUCCESS)
    {
        logger::instance().error("sd_ble_gattc_write(CMD) failed: 0x%04x '%s'",
                                 error_code, nordic_error_string(error_code));
    }

    return nordic_to_system_error(error_code);
}

std::errc ble_gattc_operations::write_command_signed(
    uint16_t            connection_handle,
    uint16_t            attribute_handle,
    void const*         data,
    ble::att::length_t  offset,
    ble::att::length_t  length)
{
    ble_gattc_write_params_t const write_params = {
        .write_op   = BLE_GATT_OP_SIGN_WRITE_CMD,
        .flags      = BLE_GATT_EXEC_WRITE_FLAG_PREPARED_WRITE,
        .handle     = attribute_handle,
        .offset     = offset,
        .len        = length,
        .p_value    = reinterpret_cast<uint8_t const*>(data)
    };

    uint32_t error_code = sd_ble_gattc_write(connection_handle, &write_params);
    if (error_code != NRF_SUCCESS)
    {
        logger::instance().error("sd_ble_gattc_write(SIGN) failed: 0x%04x '%s'",
                                 error_code, nordic_error_string(error_code));
    }

    return nordic_to_system_error(error_code);
}

std::errc ble_gattc_operations::write_prepare(
    uint16_t            connection_handle,
    uint16_t            attribute_handle,
    void const*         data,
    ble::att::length_t  offset,
    ble::att::length_t  length)
{
    ble_gattc_write_params_t const write_params = {
        .write_op   = BLE_GATT_OP_PREP_WRITE_REQ,
        .flags      = BLE_GATT_EXEC_WRITE_FLAG_PREPARED_WRITE,
        .handle     = attribute_handle,
        .offset     = offset,
        .len        = length,
        .p_value    = reinterpret_cast<uint8_t const*>(data)
    };

    uint32_t error_code = sd_ble_gattc_write(connection_handle, &write_params);
    if (error_code != NRF_SUCCESS)
    {
        logger::instance().error("sd_ble_gattc_write(PREP) failed: 0x%04x '%s'",
                                 error_code, nordic_error_string(error_code));
    }

    return nordic_to_system_error(error_code);
}

std::errc ble_gattc_operations::write_execute(
    uint16_t            connection_handle,
    uint16_t            attribute_handle,
    void const*         data,
    ble::att::length_t  offset,
    ble::att::length_t  length)
{
    ble_gattc_write_params_t const write_params = {
        .write_op   = BLE_GATT_OP_EXEC_WRITE_REQ,
        .flags      = BLE_GATT_EXEC_WRITE_FLAG_PREPARED_WRITE,
        .handle     = attribute_handle,
        .offset     = offset,
        .len        = length,
        .p_value    = reinterpret_cast<uint8_t const*>(data)
    };

    uint32_t error_code = sd_ble_gattc_write(connection_handle, &write_params);
    if (error_code != NRF_SUCCESS)
    {
        logger::instance().error("sd_ble_gattc_write(EXEC) failed: 0x%04x '%s'",
                                 error_code, nordic_error_string(error_code));
    }

    return nordic_to_system_error(error_code);
}

std::errc ble_gattc_operations::write_cancel(
    uint16_t            connection_handle,
    uint16_t            attribute_handle,
    void const*         data,
    ble::att::length_t  offset,
    ble::att::length_t  length)
{
    ble_gattc_write_params_t const write_params = {
        .write_op   = BLE_GATT_OP_EXEC_WRITE_REQ,
        .flags      = BLE_GATT_EXEC_WRITE_FLAG_PREPARED_CANCEL,
        .handle     = attribute_handle,
        .offset     = offset,
        .len        = length,
        .p_value    = reinterpret_cast<uint8_t const*>(data)
    };

    uint32_t error_code = sd_ble_gattc_write(connection_handle, &write_params);
    if (error_code != NRF_SUCCESS)
    {
        logger::instance().error("sd_ble_gattc_write(EXEC) failed: 0x%04x '%s'",
                                 error_code, nordic_error_string(error_code));
    }

    return nordic_to_system_error(error_code);
}

std::errc ble_gattc_operations::handle_value_confirm(
    uint16_t            connection_handle,
    uint16_t            attribute_handle)
{
    uint32_t error_code = sd_ble_gattc_hv_confirm(connection_handle,
                                                  attribute_handle);
    if (error_code != NRF_SUCCESS)
    {
        logger::instance().error(
            "sd_ble_gattc_hv_confirm(c: 0x%04x, h: 0x%04x): failed: 0x%04x '%s'",
            connection_handle, attribute_handle,
            error_code, nordic_error_string(error_code));
    }

    return nordic_to_system_error(error_code);
}

std::errc ble_gattc_operations::exchange_mtu_request(
    uint16_t            connection_handle,
    ble::att::length_t  mtu_size)
{
    uint32_t error_code = sd_ble_gattc_exchange_mtu_request(connection_handle,
                                                            mtu_size);
    if (error_code != NRF_SUCCESS)
    {
        logger::instance().error(
            "sd_ble_gattc_exchange_mtu_request(c: 0x%04x, mtu: %u): failed: 0x%04x '%s'",
            connection_handle, mtu_size,
            error_code, nordic_error_string(error_code));
    }

    return nordic_to_system_error(error_code);
}

} // namespace nordic
