/**
 * @file ble/gattc_service_discovery.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * BLE GATT Client Service Discovery.
 */

#include "ble/uuid.h"
#include "ble/gattc_service_builder.h"
#include "ble/profile_connectable.h"

#include "std_error.h"
#include "logger.h"

namespace ble
{
namespace gattc
{

std::errc service_builder::discover_services(
    uint16_t                        conenction_handle,
    ble::gatt::service_container&   service_container)
{
    return this->discover_services(conenction_handle,
                                   service_container,
                                   ble::att::handle_minimum,
                                   ble::att::handle_maximum);
}

std::errc service_builder::discover_services(
    uint16_t                        conenction_handle,
    ble::gatt::service_container&   service_container,
    uint16_t                        gatt_handle_first,
    uint16_t                        gatt_handle_last)
{
    std::errc const error = this->service_discovery_.discover_primary_services(
        conenction_handle, gatt_handle_first, gatt_handle_last);

    if (is_success(error))
    {
        this->service_container_             = &service_container;
        this->handle_discovery_range_.first  = gatt_handle_first;
        this->handle_discovery_range_.second = gatt_handle_last;
    }

    return error;
}

void service_builder::service_discovered(
    uint16_t                    connection_handle,
    ble::att::error_code        error_code,
    uint16_t                    error_handle,
    uint16_t                    gatt_handle_first,
    uint16_t                    gatt_handle_last,
    ble::att::uuid const&       uuid,
    bool                        response_end)
{
    logger& logger = logger::instance();

    char uuid_char_buffer[ble::att::uuid::conversion_length];
    uuid.to_chars(std::begin(uuid_char_buffer), std::end(uuid_char_buffer));

    if (error_code == ble::att::error_code::success)
    {
        logger.info(
            "service discovered: [0x%04x:0x%04x]: %s",
            gatt_handle_first, gatt_handle_last, uuid_char_buffer);

        /// @todo populate the service table with entries.
    }
    else
    {
        logger.warn(
            "service_builder::service_discovered: [0x%04x, 0x%04x]: "
            "%s: error: %u, gatt_handle: 0x%04x",
            gatt_handle_first, gatt_handle_last, uuid_char_buffer, error_code, error_handle);

        /// @todo If this is a permissions error, like security level, then what?
        /// For now, exit.
        return;
    }

    if (response_end)
    {
        uint16_t const gatt_handle_next = gatt_handle_last + 1u;
        if ((gatt_handle_last == ble::att::handle_maximum) ||
            (gatt_handle_next > this->handle_discovery_range_.second))
        {
            logger.info("service discovery complete");
            /// @todo Start discovery of relationships...
        }
        else
        {
            // Continue the discovery of services
            std::errc const error = this->service_discovery_.discover_primary_services(
                connection_handle, gatt_handle_next, this->handle_discovery_range_.second);

            if (not is_success(error))
            {
                logger.error(
                    "service_builder::discover_primary_services: [0x%04x:0x%04x]: failed: %u",
                    gatt_handle_next, gatt_handle_last, error);
            }
        }
    }
}

void service_builder::relationship_discovered(
    uint16_t                    connection_handle,
    ble::att::error_code        error_code,
    uint16_t                    error_handle,
    uint16_t                    gatt_handle_first,
    uint16_t                    gatt_handle_last,
    uint16_t                    service_handle,
    ble::att::uuid const&       uuid,
    bool                        response_end)
{
}

void service_builder::characteristic_discovered(
    uint16_t                    connection_handle,
    ble::att::error_code        error_code,
    uint16_t                    error_handle,
    uint16_t                    handle_declaration,
    uint16_t                    handle_value,
    ble::att::uuid const&       uuid,
    ble::gatt::properties       properties,
    bool                        response_end)
{
}

void service_builder::descriptor_discovered(
    uint16_t                    connection_handle,
    ble::att::error_code        error_code,
    uint16_t                    error_handle,
    uint16_t                    desciptor_handle,
    ble::att::uuid const&       uuid,
    bool                        response_end)
{
}

void service_builder::attribute_discovered(
    uint16_t                    connection_handle,
    ble::att::error_code        error_code,
    uint16_t                    error_handle,
    uint16_t                    handle,
    ble::att::uuid const&       uuid,
    bool                        response_end)
{
}

} // namespace gattc
} // namespace ble

