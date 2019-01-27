/**
 * @file ble/gattc_service_discovery.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * BLE GATT Client Service Discovery.
 */

#include "ble/uuid.h"
#include "ble/gatt_service_container.h"
#include "ble/gattc_service_builder.h"
#include "ble/profile_connectable.h"

#include "std_error.h"
#include "logger.h"
#include "project_assert.h"

/**
 * @todo @bug
 * + On gap::disconnect we need to free the service_container and move all
 *   entries to their free lists.
 * + When the GATT service changed 0x1801 indication (not notification) is
 *   received clear all handles within the range and repopulate.
 *   It appears that the indication value is [handle_start:handle_stop].
 * + Ignoring secondary and relationship discovery for now.
 *   Transitioning from primary service discovery to characteristics discovery.
 *   See code line:
 *          logger.info("service discovery complete");
 */

namespace ble
{
namespace gattc
{

std::errc service_builder::discover_services(
    uint16_t                        conenction_handle,
    ble::gatt::service_container&   svc_container,
    uint16_t                        gatt_handle_first,
    uint16_t                        gatt_handle_last)
{
    std::errc const error = this->service_discovery.discover_primary_services(
        conenction_handle, gatt_handle_first, gatt_handle_last);

    if (is_success(error))
    {
        this->service_container             = &svc_container;
        this->handle_discovery_range.first  = gatt_handle_first;
        this->handle_discovery_range.second = gatt_handle_last;
    }

    return error;
}

void service_builder::service_discovered(
    uint16_t                    connection_handle,
    ble::att::error_code        gatt_error,
    uint16_t                    gatt_handle_error,
    uint16_t                    gatt_handle_first,
    uint16_t                    gatt_handle_last,
    ble::att::uuid const&       uuid,
    bool                        response_end)
{
    logger& logger = logger::instance();

    char uuid_char_buffer[ble::att::uuid::conversion_length];
    uuid.to_chars(std::begin(uuid_char_buffer), std::end(uuid_char_buffer));

    if (gatt_error == ble::att::error_code::success)
    {
        logger.info("service discovered: h: [0x%04x, 0x%04x]: %s",
                    gatt_handle_first, gatt_handle_last, uuid_char_buffer);

        if (this->free_list.services.empty())
        {
            logger.error(
                "service discovered: h: [0x%04x, 0x%04x]: %s, free list empty",
                gatt_handle_first, gatt_handle_last, uuid_char_buffer);
        }
        else
        {
            ble::gatt::service& service = this->free_list.services.front();
            this->free_list.services.pop_front();
            service.uuid = uuid;
            service.decl.attribute_type =
                ble::gatt::attribute_type::primary_service;
            service.decl.handle = gatt_handle_first;
            this->service_container->push_back(service);
        }
    }
    else if (gatt_error == ble::att::error_code::attribute_not_found)
    {
        // This error indicates that there are no more services
        // to be found in the range requested. Set response_end and
        // gatt_handle_last to complete service discovery below.
        response_end = true;
        gatt_handle_last = ble::att::handle_maximum;
    }
    else
    {
        logger.warn("service_builder::service_discovered: [0x%04x, 0x%04x]: "
                    "%s: error: %u, gatt_handle: 0x%04x",
                    gatt_handle_first, gatt_handle_last, uuid_char_buffer,
                    gatt_error, gatt_handle_error);

        /// @todo If this is a permissions error, like security level,
        /// then what? For now, exit.
        return;
    }

    if (response_end)
    {
        uint16_t const gatt_handle_next = gatt_handle_last + 1u;
        if ((gatt_handle_last == ble::att::handle_maximum) ||
            (gatt_handle_next > this->handle_discovery_range.second))
        {
            // Service discovery complete. Begin characteristics discovery.
            /// @todo this should be relationship discovery.
            logger.info("service discovery complete");
            std::errc const error =
                this->service_discovery.discover_characteristics(
                    connection_handle,
                    this->handle_discovery_range.first,
                    this->handle_discovery_range.second);

            if (not is_success(error))
            {
                logger.error("service_builder::discover_characteristics: "
                             "[0x%04x:0x%04x]: failed: %u",
                    this->handle_discovery_range.first,
                    this->handle_discovery_range.second, error);
            }
        }
        else
        {
            // Continue the discovery of services
            std::errc const error =
                this->service_discovery.discover_primary_services(
                    connection_handle,
                    gatt_handle_next,
                    this->handle_discovery_range.second);

            if (not is_success(error))
            {
                logger.error(
                    "service_builder::discover_primary_services: "
                    "[0x%04x:0x%04x]: failed: %u",
                    gatt_handle_next, gatt_handle_last, error);
            }
        }
    }
}

void service_builder::relationship_discovered(
    uint16_t                    connection_handle,
    ble::att::error_code        gatt_error,
    uint16_t                    gatt_handle_error,
    uint16_t                    gatt_handle_first,
    uint16_t                    gatt_handle_last,
    uint16_t                    service_handle,
    ble::att::uuid const&       uuid,
    bool                        response_end)
{
}

void service_builder::characteristic_discovered(
    uint16_t                    connection_handle,
    ble::att::error_code        gatt_error,
    uint16_t                    gatt_handle_error,
    uint16_t                    gatt_handle_declaration,
    uint16_t                    gatt_handle_value,
    ble::att::uuid const&       uuid,
    ble::gatt::properties       properties,
    bool                        response_end)
{
    logger& logger = logger::instance();

    char uuid_char_buffer[ble::att::uuid::conversion_length];
    uuid.to_chars(std::begin(uuid_char_buffer), std::end(uuid_char_buffer));

    if (gatt_error == ble::att::error_code::success)
    {
        logger.info("characteristic discovered: h:[0x%04x, 0x%04x]: %s",
                    gatt_handle_declaration, gatt_handle_value,
                    uuid_char_buffer);

        ble::gatt::service* service =
            this->service_container->find_service_handle_assoc(
                gatt_handle_declaration);

        if (service)
        {
            if (this->free_list.characteristics.empty())
            {
                logger.error("characteristic discovered: h: [0x%04x, 0x%04x]: "
                             "%s, free list empty",
                             gatt_handle_declaration, gatt_handle_value,
                             uuid_char_buffer);
            }
            else
            {
                ble::gatt::attribute& list_node =
                    this->free_list.characteristics.front();
                this->free_list.characteristics.pop_front();

                ble::gatt::characteristic& characteristic =
                    reinterpret_cast<ble::gatt::characteristic&>(list_node);

                // Note that the default ctor for ble::gatt::characteristic has
                // set the attribute_type properly. Doing it here anyway.
                characteristic.uuid = uuid;
                characteristic.decl.attribute_type =
                    ble::gatt::attribute_type::characteristic;
                characteristic.decl.handle = gatt_handle_declaration;
                service->characteristic_add(characteristic);
                /// @todo Need gatt_handle_value assignment
            }
        }
        else
        {
            logger.error("service handle association for 0x%04x not found",
                         gatt_handle_declaration);
        }
    }
    else if (gatt_error == ble::att::error_code::attribute_not_found)
    {
        // This error indicates that there are no more characeteristics
        // to be found in the range requested. Set response_end and
        // gatt_handle_value to complete service discovery below.
        response_end = true;
        gatt_handle_value = ble::att::handle_maximum;
    }
    else
    {
        logger.warn("characteristic discovered: (0x%04x, 0x%04x): %s: "
                    "error: %u, gatt_handle: 0x%04x",
                    gatt_handle_declaration, gatt_handle_value,
                    uuid_char_buffer, gatt_error, gatt_handle_error);

        /// @todo If this is a permissions error, like security level,
        /// then what? For now, exit.
        return;
    }

    if (response_end)
    {
        uint16_t const gatt_handle_next = gatt_handle_value + 1u;
        if ((gatt_handle_value == ble::att::handle_maximum) ||
            (gatt_handle_next > this->handle_discovery_range.second))
        {
            // Characteristic discovery complete. Begin descriptors discovery.
            logger.info("characteristic discovery complete");

            this->discovery_iterator =
                this->service_container->next_open_characteristic(
                    this->service_container->discovery_begin()
                    );

            if (this->discovery_iterator ==
                this->service_container->discovery_end())
            {
                logger.info("descriptor discovery complete");

                // No descriptors to discover

                /// @todo next phase of discovery goes here...
            }
            else
            {
                ble::att::handle_range const handle_range =
                    this->discovery_iterator.handle_range();
                std::errc const error =
                    this->service_discovery.discover_descriptors(
                        connection_handle,
                        handle_range.first,
                        handle_range.second);

                if (not is_success(error))
                {
                    logger.error(
                        "service_builder::discover_descriptors: "
                        "h: [0x%04x, 0x%04x]: failed: %u",
                        handle_range.first, handle_range.second, error);
                }
            }
        }
        else
        {
            // Continue the discovery of characteristics
            std::errc const error =
                this->service_discovery.discover_characteristics(
                    connection_handle,
                    gatt_handle_next,
                    this->handle_discovery_range.second);

            if (not is_success(error))
            {
                logger.error("service_builder::discover_characteristics: "
                             "h: [0x%04x, 0x%04x]: failed: %u",
                             gatt_handle_next,
                             this->handle_discovery_range.second,
                             error);
            }
        }
    }
}

void service_builder::descriptor_discovered(
    uint16_t                    connection_handle,
    ble::att::error_code        gatt_error,
    uint16_t                    gatt_handle_error,
    uint16_t                    gatt_handle_desciptor,
    ble::att::uuid const&       uuid,
    bool                        response_end)
{
    logger& logger = logger::instance();

    char uuid_char_buffer[ble::att::uuid::conversion_length];
    uuid.to_chars(std::begin(uuid_char_buffer), std::end(uuid_char_buffer));

    if (gatt_error == ble::att::error_code::success)
    {
        logger.info("descriptor discovered: 0x%04x: %s",
                    gatt_handle_desciptor, uuid_char_buffer);

        if (this->free_list.descriptors.empty())
        {
            logger.error("descriptor discovered: 0x%04x: %s, free list empty",
                         gatt_handle_desciptor, uuid_char_buffer);
        }
        else
        {
            ble::gatt::attribute& list_node =
                this->free_list.characteristics.front();
            this->free_list.characteristics.pop_front();

            ble::gatt::descriptor_base& descriptor =
                reinterpret_cast<ble::gatt::descriptor_base&>(list_node);

            ble::gatt::service_container::discovery_iterator::iterator_node
                const node = *this->discovery_iterator;
            node.characteristic.descriptor_add(descriptor);
        }
    }
    else if (gatt_error == ble::att::error_code::attribute_not_found)
    {
        // This error indicates that there are no more characeteristics
        // to be found in the range requested. Set response_end and
        // gatt_handle_desciptor to complete service discovery below.
        response_end = true;
        gatt_handle_desciptor = ble::att::handle_maximum;
    }
    else
    {
        logger.warn("descriptor discovered: 0x%04x: %s: "
                    "error: %u, gatt_handle: 0x%04x",
                    gatt_handle_desciptor, uuid_char_buffer,
                    gatt_error, gatt_handle_error);

        /// @todo If this is a permissions error, like security level,
        /// then what? For now, exit.
        return;
    }

    if (response_end)
    {
        ++this->discovery_iterator;
        this->discovery_iterator =
            this->service_container->next_open_characteristic(
                this->discovery_iterator);

        if (this->discovery_iterator ==
            this->service_container->discovery_end())
        {
            // Descriptor discovery complete.
            logger.info("descriptor discovery complete");
            /// @todo next phase of discovery goes here...
        }
        else
        {
            // Continue the discovery of descriptors.
            ble::att::handle_range handle_range =
                this->discovery_iterator.handle_range();
            std::errc const error =
                this->service_discovery.discover_descriptors(
                    connection_handle, handle_range.first, handle_range.second);

            if (not is_success(error))
            {
                logger.error("service_builder::discover_descriptors: "
                             "h: [0x%04x, 0x%04x]: failed: %u",
                             handle_range.first, handle_range.second, error);
            }
        }
    }
}

void service_builder::attribute_discovered(
    uint16_t                    connection_handle,
    ble::att::error_code        gatt_error,
    uint16_t                    gatt_handle_error,
    uint16_t                    handle,
    ble::att::uuid const&       uuid,
    bool                        response_end)
{
}

} // namespace gattc
} // namespace ble

