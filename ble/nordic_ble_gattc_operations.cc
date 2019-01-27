/**
 * @file ble/nordic_ble_gattc_operations.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "ble/nordic_ble_gattc_operations.h"
#include "ble/nordic_ble_att.h"
#include "logger.h"
#include "nordic_error.h"
#include "project_assert.h"

#include "ble.h"
#include "ble_gatt.h"
#include "ble_gattc.h"

namespace nordic
{

std::errc ble_gattc_discovery_operations::discover_primary_services(
    uint16_t connection_handle,
    uint16_t gatt_handle_start,
    uint16_t gatt_handle_stop)
{
    logger& logger = logger::instance();

    logger.info("discover_services(c: 0x%04x, h: [0x%04x, 0x%04x])",
                 connection_handle, gatt_handle_start, gatt_handle_stop);

    uint32_t const error_code = sd_ble_gattc_primary_services_discover(
        connection_handle,
        gatt_handle_start,
        nullptr);

    if (error_code == NRF_SUCCESS)
    {
        this->last_requested_.first  = gatt_handle_start;
        this->last_requested_.second = gatt_handle_stop;
    }
    else
    {
        logger.error("sd_ble_gattc_primary_services_discover("
                     "c: 0x%04x, h: 0x%04x) failed: 0x%04x '%s'",
                     connection_handle, gatt_handle_start,
                     error_code, nordic_error_string(error_code));
    }

    return nordic_to_system_error(error_code);
}

std::errc ble_gattc_discovery_operations::discover_service_relationships(
    uint16_t            connection_handle,
    uint16_t            gatt_handle_start,
    uint16_t            gatt_handle_stop)
{
    logger& logger = logger::instance();

    logger.info("discover_relationships(c: 0x%04x, h: [0x%04x, 0x%04x])",
                 connection_handle, gatt_handle_start, gatt_handle_stop);

    ble_gattc_handle_range_t const gatt_handle_range = {
        .start_handle = gatt_handle_start,
        .end_handle   = gatt_handle_stop
    };

    uint32_t const error_code = sd_ble_gattc_relationships_discover(
        connection_handle, &gatt_handle_range);

    if (error_code == NRF_SUCCESS)
    {
        this->last_requested_.first  = gatt_handle_start;
        this->last_requested_.second = gatt_handle_stop;
    }
    else
    {
        logger.error("sd_ble_gattc_relationships_discover("
                     "c: 0x%04x, h: [0x%04x, 0x%04x]) failed: 0x%04x '%s'",
                     connection_handle, gatt_handle_start, gatt_handle_stop,
                     error_code, nordic_error_string(error_code));
    }

    return nordic_to_system_error(error_code);
}

std::errc ble_gattc_discovery_operations::discover_characteristics(
    uint16_t            connection_handle,
    uint16_t            gatt_handle_start,
    uint16_t            gatt_handle_stop)
{
    logger& logger = logger::instance();

    logger.info("discover_characteristics(c: 0x%04x, h: [0x%04x, 0x%04x])",
                 connection_handle, gatt_handle_start, gatt_handle_stop);

    ble_gattc_handle_range_t const gatt_handle_range = {
        .start_handle = gatt_handle_start,
        .end_handle   = gatt_handle_stop
    };

    uint32_t const error_code = sd_ble_gattc_characteristics_discover(
        connection_handle, &gatt_handle_range);

    if (error_code == NRF_SUCCESS)
    {
        this->last_requested_.first  = gatt_handle_start;
        this->last_requested_.second = gatt_handle_stop;
    }
    else
    {
        logger.error("sd_ble_gattc_characteristics_discover("
                     "c: 0x%04x, h: [0x%04x, 0x%04x]) failed: 0x%04x '%s'",
                     connection_handle, gatt_handle_start, gatt_handle_stop,
                     error_code, nordic_error_string(error_code));
    }

    return nordic_to_system_error(error_code);
}

std::errc ble_gattc_discovery_operations::discover_descriptors(
    uint16_t            connection_handle,
    uint16_t            gatt_handle_start,
    uint16_t            gatt_handle_stop)
{
    logger& logger = logger::instance();

    logger.info("discover_descriptors(c: 0x%04x, h: [0x%04x, 0x%04x])",
                 connection_handle, gatt_handle_start, gatt_handle_stop);

    ble_gattc_handle_range_t const gatt_handle_range = {
        .start_handle = gatt_handle_start,
        .end_handle   = gatt_handle_stop
    };

    uint32_t const error_code = sd_ble_gattc_descriptors_discover(
        connection_handle, &gatt_handle_range);

    if (error_code == NRF_SUCCESS)
    {
        this->last_requested_.first  = gatt_handle_start;
        this->last_requested_.second = gatt_handle_stop;
    }
    else
    {
        logger.error("sd_ble_gattc_descriptors_discover("
                     "c: 0x%04x, h: [0x%04x, 0x%04x]) failed: 0x%04x '%s'",
                     connection_handle, gatt_handle_start, gatt_handle_stop,
                     error_code, nordic_error_string(error_code));
    }

    return nordic_to_system_error(error_code);
}

std::errc ble_gattc_discovery_operations::discover_attributes(
    uint16_t            connection_handle,
    uint16_t            gatt_handle_start,
    uint16_t            gatt_handle_stop)
{
    logger& logger = logger::instance();

    logger.info("discover_attributes(c: 0x%04x, h: [0x%04x, 0x%04x])",
                 connection_handle, gatt_handle_start, gatt_handle_stop);

    ble_gattc_handle_range_t const gatt_handle_range = {
        .start_handle = gatt_handle_start,
        .end_handle   = gatt_handle_stop
    };

    uint32_t const error_code = sd_ble_gattc_attr_info_discover(
        connection_handle, &gatt_handle_range);

    if (error_code == NRF_SUCCESS)
    {
        this->last_requested_.first  = gatt_handle_start;
        this->last_requested_.second = gatt_handle_stop;
    }
    else
    {
        logger.error("sd_ble_gattc_attr_info_discover("
                     "c: 0x%04x, h: [0x%04x, 0x%04x]) failed: 0x%04x '%s'",
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
    logger& logger = logger::instance();

    logger.info("gattc read(c: 0x%04x, h: 0x%04x)",
                 connection_handle, attribute_handle);

    uint32_t error_code = sd_ble_gattc_read(connection_handle,
                                            attribute_handle,
                                            offset);
    if (error_code != NRF_SUCCESS)
    {
        logger.error("sd_ble_gattc_read() failed: 0x%04x '%s'",
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
    logger& logger = logger::instance();

    logger.info("gattc write_request(c: 0x%04x, h: 0x%04x)",
                 connection_handle, attribute_handle);

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
        logger.error("sd_ble_gattc_write(REQ) failed: 0x%04x '%s'",
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
    logger& logger = logger::instance();

    logger.info("gattc write_command(c: 0x%04x, h: 0x%04x)",
                 connection_handle, attribute_handle);

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
        logger.error("sd_ble_gattc_write(CMD) failed: 0x%04x '%s'",
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
    logger& logger = logger::instance();

    logger.info("gattc write_command_signed(c: 0x%04x, h: 0x%04x)",
                 connection_handle, attribute_handle);

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
        logger.error("sd_ble_gattc_write(SIGN) failed: 0x%04x '%s'",
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
    logger& logger = logger::instance();

    logger.info("gattc write_prepare(c: 0x%04x, h: 0x%04x)",
                 connection_handle, attribute_handle);

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
        logger.error("sd_ble_gattc_write(PREP) failed: 0x%04x '%s'",
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
    logger& logger = logger::instance();

    logger.info("gattc write_prepare(c: 0x%04x, h: 0x%04x)",
                 connection_handle, attribute_handle);

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
        logger.error("sd_ble_gattc_write(EXEC) failed: 0x%04x '%s'",
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
    logger& logger = logger::instance();

    logger.info("gattc write_cancel(c: 0x%04x, h: 0x%04x)",
                 connection_handle, attribute_handle);

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
        logger.error("sd_ble_gattc_write(EXEC) failed: 0x%04x '%s'",
                     error_code, nordic_error_string(error_code));
    }

    return nordic_to_system_error(error_code);
}

std::errc ble_gattc_operations::handle_value_confirm(
    uint16_t            connection_handle,
    uint16_t            attribute_handle)
{
    logger& logger = logger::instance();

    logger.info("gattc handle_value_confirm(c: 0x%04x, h: 0x%04x)",
                 connection_handle, attribute_handle);

    uint32_t error_code = sd_ble_gattc_hv_confirm(connection_handle,
                                                  attribute_handle);
    if (error_code != NRF_SUCCESS)
    {
        logger.error("sd_ble_gattc_hv_confirm(c: 0x%04x, h: 0x%04x): "
                     "failed: 0x%04x '%s'", connection_handle, attribute_handle,
                     error_code, nordic_error_string(error_code));
    }

    return nordic_to_system_error(error_code);
}

std::errc ble_gattc_operations::exchange_mtu_request(
    uint16_t            connection_handle,
    ble::att::length_t  mtu_size)
{
    logger& logger = logger::instance();

    logger.info("gattc exchange_mtu_request(c: 0x%04x, mtu: %u)",
                 connection_handle, mtu_size);

    uint32_t error_code = sd_ble_gattc_exchange_mtu_request(connection_handle,
                                                            mtu_size);
    if (error_code != NRF_SUCCESS)
    {
        logger.error("sd_ble_gattc_exchange_mtu_request(c: 0x%04x, mtu: %u): "
                     "failed: 0x%04x '%s'", connection_handle, mtu_size,
                     error_code, nordic_error_string(error_code));
    }

    return nordic_to_system_error(error_code);
}

std::errc ble_gattc_operations::preload_custom_uuid(ble::att::uuid const &uuid)
{
    ble_uuid128_t const uuid_128 = nordic::from_att_uuid_128(uuid);
    uint8_t uuid_type = BLE_UUID_TYPE_VENDOR_BEGIN;
    uint32_t const error_code = sd_ble_uuid_vs_add(&uuid_128, &uuid_type);

    char buffer[ble::att::uuid::conversion_length];
    uuid.to_chars(std::begin(buffer), std::end(buffer));

    logger& logger = logger::instance();
    logger.info("sd_ble_uuid_vs_add(%s): %u, uuid_type = %u",
                 buffer, error_code, uuid_type);

    ASSERT(error_code == NRF_SUCCESS);
    return nordic_to_system_error(error_code);
}

} // namespace nordic
