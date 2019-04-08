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
    uint16_t                            conenction_handle,
    ble::gatt::service_container&       svc_container,
    uint16_t                            gatt_handle_first,
    uint16_t                            gatt_handle_last,
    service_builder::completion_notify* notify)
{
    std::errc const error = this->service_discovery.discover_primary_services(
        conenction_handle, gatt_handle_first, gatt_handle_last);

    if (is_success(error))
    {
        this->service_container             = &svc_container;
        this->discovery_handle_range.first  = gatt_handle_first;
        this->discovery_handle_range.second = gatt_handle_last;
        this->completion_notification       = notify;
    }

    return error;
}

std::errc service_builder::discover_attributes(
    uint16_t                            conenction_handle,
    ble::gatt::service_container&       svc_container,
    uint16_t                            gatt_handle_first,
    uint16_t                            gatt_handle_last,
    service_builder::completion_notify* notify)
{
    std::errc const error = this->service_discovery.discover_attributes(
        conenction_handle, gatt_handle_first, gatt_handle_last);

    if (is_success(error))
    {
        this->service_container             = &svc_container;
        this->discovery_handle_range.first  = gatt_handle_first;
        this->discovery_handle_range.second = gatt_handle_last;
        this->completion_notification       = notify;
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
        logger.debug("service discovered: h: [0x%04x, 0x%04x]: %s",
                     gatt_handle_first, gatt_handle_last, uuid_char_buffer);

        if (this->free_list.services.empty())
        {
            logger.debug(
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

        if (this->completion_notification)
        {
            completion_notify *completion = this->completion_notification;
            this->completion_notification = nullptr;
            completion->notify(gatt_error);
        }
        return;
    }

    if (response_end)
    {
        uint16_t const gatt_handle_next = gatt_handle_last + 1u;
        if ((gatt_handle_last == ble::att::handle_maximum) ||
            (gatt_handle_next > this->discovery_handle_range.second))
        {
            // Service discovery complete. Begin characteristics discovery.
            /// @todo this should be relationship discovery.
            logger.debug("service discovery complete");
            std::errc const error =
                this->service_discovery.discover_characteristics(
                    connection_handle,
                    this->discovery_handle_range.first,
                    this->discovery_handle_range.second);

            if (not is_success(error))
            {
                logger.error("service_builder::discover_characteristics: "
                             "[0x%04x:0x%04x]: failed: %u",
                    this->discovery_handle_range.first,
                    this->discovery_handle_range.second, error);
            }
        }
        else
        {
            // Continue the discovery of services
            std::errc const error =
                this->service_discovery.discover_primary_services(
                    connection_handle,
                    gatt_handle_next,
                    this->discovery_handle_range.second);

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
        logger.debug("characteristic discovered: h:[0x%04x, 0x%04x]: %s",
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
                characteristic.uuid         = uuid;
                characteristic.value_handle = gatt_handle_value;
                characteristic.decl.handle  = gatt_handle_declaration;
                characteristic.decl.attribute_type =
                    ble::gatt::attribute_type::characteristic;
                service->characteristic_add(characteristic);
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

        if (this->completion_notification)
        {
            completion_notify *completion = this->completion_notification;
            this->completion_notification = nullptr;
            completion->notify(gatt_error);
        }
        return;
    }

    if (response_end)
    {
        uint16_t const gatt_handle_next = gatt_handle_value + 1u;
        if ((gatt_handle_value == ble::att::handle_maximum) ||
            (gatt_handle_next > this->discovery_handle_range.second))
        {
            // Characteristic discovery complete. Begin descriptors discovery.
            logger.debug("characteristic discovery complete");

            this->discovery_iterator =
                this->service_container->next_open_characteristic(
                    this->service_container->discovery_begin()
                    );

            if (this->discovery_iterator ==
                this->service_container->discovery_end())
            {
                // Descriptor discovery complete.
                // Aggregate GATT service discovery is complete.
                logger.debug("descriptor discovery complete");
                this->trim_discovery_handle_range();
                logger.debug(
                    "service discovery handle range: h: [0x%04x, 0x%04x]",
                    this->discovery_handle_range.first,
                    this->discovery_handle_range.second);

                if (this->completion_notification)
                {
                    completion_notify *completion = this->completion_notification;
                    this->completion_notification = nullptr;
                    completion->notify(gatt_error);
                }
                return;
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
                    this->discovery_handle_range.second);

            if (not is_success(error))
            {
                logger.error("service_builder::discover_characteristics: "
                             "h: [0x%04x, 0x%04x]: failed: %u",
                             gatt_handle_next,
                             this->discovery_handle_range.second,
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
        logger.debug("descriptor discovered: 0x%04x: %s",
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
            descriptor.decl.handle = gatt_handle_desciptor;

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

        if (this->completion_notification)
        {
            completion_notify *completion = this->completion_notification;
            this->completion_notification = nullptr;
            completion->notify(gatt_error);
        }
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
            // Aggregate GATT service discovery is complete.
            logger.debug("descriptor discovery complete");
            this->trim_discovery_handle_range();
            logger.debug("service discovery handle range: h: [0x%04x, 0x%04x]",
                         this->discovery_handle_range.first,
                         this->discovery_handle_range.second);
            if (this->completion_notification)
            {
                completion_notify *completion = this->completion_notification;
                this->completion_notification = nullptr;
                completion->notify(gatt_error);
            }
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
    uint16_t                    gatt_handle_attribute,
    ble::att::uuid const&       uuid,
    bool                        response_end)
{
    logger& logger = logger::instance();

    char uuid_char_buffer[ble::att::uuid::conversion_length];
    uuid.to_chars(std::begin(uuid_char_buffer), std::end(uuid_char_buffer));

    if (gatt_error == ble::att::error_code::success)
    {
        logger.debug("attribute discovered: 0x%04x: %s",
                     gatt_handle_attribute, uuid_char_buffer);

        /// @todo Need to add an attributes free list.
        /// For now use the characteristics list. Change later:
        /// this->free_list.characteristics => this->free_list.attributes
        if (this->free_list.characteristics.empty())
        {
            logger.error("attribute discovered: 0x%04x: %s, free list empty",
                         gatt_handle_attribute, uuid_char_buffer);
        }
        else
        {
#if 0
            ble::gatt::attribute& attribute =
                this->free_list.characteristics.front();
            this->free_list.characteristics.pop_front();

            /// @todo Use the discovery_iterator to find the characteristic
            /// associated with this gatt_handle_attribute.
//            ble::gatt::service_container::discovery_iterator::iterator_node
//                const node = *this->discovery_iterator;

            /// @todo Add the attribute to the characteristic here:
            (void) attribute;
            /// node.characteristic.attribute_add(attribute);   @todo
#endif
        }
    }
    else if (gatt_error == ble::att::error_code::attribute_not_found)
    {
        // This error indicates that there are no more atributes
        // to be found in the range requested. Set response_end and
        // gatt_handle_attribute to complete service discovery below.
        response_end = true;
        gatt_handle_attribute = ble::att::handle_maximum;
    }
    else
    {
        logger.warn("attribute discovered: 0x%04x: %s: "
                    "error: %u, gatt_handle: 0x%04x",
                    gatt_handle_attribute, uuid_char_buffer,
                    gatt_error, gatt_handle_error);

        if (this->completion_notification)
        {
            completion_notify *completion = this->completion_notification;
            this->completion_notification = nullptr;
            completion->notify(gatt_error);
        }
        return;
    }

    if (response_end)
    {
        /// @todo This is a hack for determining the last attribute within
        /// the service container. It will be the last characteristic handle
        /// (which is the declaration handle) +1 for the attribute handle.
        /// There should be a better way. Find it and implement it.
        ble::gatt::service_container::discovery_iterator disco_iter =
           this->service_container->discovery_end();
        --disco_iter;
        ble::att::handle_range const range = disco_iter.handle_range();
        uint16_t const last_attribute_handle = range.first + 1u;

        uint16_t const gatt_handle_next = gatt_handle_attribute + 1u;
        if ((gatt_handle_attribute == ble::att::handle_maximum) ||
            (gatt_handle_next > last_attribute_handle))
        {
            // Attribute discovery complete. This concludes attribute discovery.
            logger.debug("attribute discovery complete");
        }
        else
        {
            // Continue the discovery of attributes.
            std::errc const error =
                this->service_discovery.discover_attributes(
                    connection_handle,
                    gatt_handle_next,
                    last_attribute_handle);

            if (not is_success(error))
            {
                logger.error("service_builder::discover_attributes: "
                             "h: [0x%04x, 0x%04x]: failed: %u",
                             gatt_handle_next,
                             this->discovery_handle_range.second,
                             error);
            }
        }
    }
}

void service_builder::trim_discovery_handle_range()
{
    ble::gatt::service_container::discovery_iterator disco_iter =
       this->service_container->discovery_end();
    --disco_iter;

    this->discovery_handle_range.second =
        (*disco_iter).characteristic.hande_range().second;
}

} // namespace gattc
} // namespace ble

