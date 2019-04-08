/**
 * @file ble/gattc_service_builder.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "ble/gatt_service_container.h"
#include "ble/gatt_characteristic.h"
#include "ble/gatt_attribute.h"
#include "ble/gatt_descriptors.h"
#include "ble/gattc_discovery_observer.h"
#include "ble/gattc_operations.h"

#include <iterator>
#include <utility>
#include <system_error>

namespace ble
{
namespace gattc
{

/**
 * @class ble::gattc:service_builder
 * Implement the service discovery observer to recieve service discovery
 * responses and aggregate the service discovery operations to perform requests,
 * thereby building a container of services.
 *
 * @see BLUETOOTH SPECIFICATION Version 5.0 | Vol 3, Part G
 * 4.4 PRIMARY SERVICE DISCOVERY
 * 4.5 RELATIONSHIP DISCOVERY
 * 4.6 CHARACTERISTIC DISCOVERY
 * 4.7 CHARACTERISTIC DESCRIPTOR DISCOVERY
 * 4.8 CHARACTERISTIC VALUE READ
 */
class service_builder: public ble::gattc::discovery_observer
{
public:

    /**
     * @interface completion_notify
     * The abstract class for which service_builder notifications are performed.
     */
    struct completion_notify
    {
        virtual ~completion_notify()                            = default;

        completion_notify()                                     = default;
        completion_notify(completion_notify const&)             = delete;
        completion_notify(completion_notify &&)                 = delete;
        completion_notify& operator=(completion_notify const&)  = delete;
        completion_notify& operator=(completion_notify&&)       = delete;

        /**
         * When the requested discovery operation completes this method is
         * called to notify the client that all services have been acquired.
         * This notification callback will be in BLE ISR context.
         *
         * @param ble::att::error_code
         *        ble::att::error_code::success if successful
         *        Otherwise the reason for service discovery failure.
         */
        virtual void notify(ble::att::error_code error) = 0;
    };

    ~service_builder()                                  = default;

    service_builder()                                   = delete;
    service_builder(service_builder const&)             = delete;
    service_builder(service_builder &&)                 = delete;
    service_builder& operator=(service_builder const&)  = delete;
    service_builder& operator=(service_builder&&)       = delete;

    service_builder(ble::gattc::discovery_operations& operations)
    : ble::gattc::discovery_observer(),
      service_discovery(operations),
      service_container(nullptr),
      discovery_handle_range(ble::att::handle_invalid, ble::att::handle_invalid),
      discovery_iterator(),
      completion_notification(nullptr)
    {
    }

    /**
     * Discover the services published by the GATT server within the GATT
     * handle range [gatt_handle_first: gatt_handle_last].
     *
     * @param connection_handle The connection handle specifying the GATT server
     *                          to which a BLE has been established.
     * @param service_container The service container to build up with services
     *                          and the services attributes (characteristics,
     *                          descriptors, etc.).
     * @param gatt_handle_first The first GATT handle within the range.
     * @param gatt_handle_last  The last GATT handle within the range (inclusive).
     * @param notify            When service discovery is complete this
     *                          completion function is called.
     *
     * @return std::errc        Whether the call to the silicon vendor call
     *                          was a success or failure.
     */
    std::errc discover_services(uint16_t                      connection_handle,
                                ble::gatt::service_container& service_container,
                                uint16_t                      gatt_handle_first,
                                uint16_t                      gatt_handle_last,
                                completion_notify*            notify);

    /**
     * This method is implemented but not used. I don't have a specific use
     * casee for this since the call to discover_services will aquire all
     * services, characteristics, and descriptors from the GATT server; thus
     * all attributes are acquired. Also, the implementation for
     * attribute_discovered() is not complete.
     *
     * @note For the Nordic softdevice passing in ble::att::handle_maximum will
     * cause a wait for every condition if this function is called.
     * The gatt_handle_last must be known - which requires service discovery -
     * so this functon is pretty useless. I don't know why it exists.
     */
    std::errc discover_attributes(uint16_t                      connection_handle,
                                  ble::gatt::service_container& service_container,
                                  uint16_t                      gatt_handle_first,
                                  uint16_t                      gatt_handle_last,
                                  completion_notify*            notify);

    virtual void service_discovered(
        uint16_t                    connection_handle,
        ble::att::error_code        gatt_error,
        uint16_t                    gatt_handle_error,
        uint16_t                    gatt_handle_first,
        uint16_t                    gatt_handle_last,
        ble::att::uuid const&       uuid,
        bool                        response_end) override;

    virtual void relationship_discovered(
        uint16_t                    connection_handle,
        ble::att::error_code        gatt_error,
        uint16_t                    gatt_handle_error,
        uint16_t                    gatt_handle_first,
        uint16_t                    gatt_handle_last,
        uint16_t                    service_handle,
        ble::att::uuid const&       uuid,
        bool                        response_end) override;

    virtual void characteristic_discovered(
        uint16_t                    connection_handle,
        ble::att::error_code        gatt_error,
        uint16_t                    gatt_handle_error,
        uint16_t                    gatt_handle_declaration,
        uint16_t                    gatt_handle_value,
        ble::att::uuid const&       uuid,
        ble::gatt::properties       properties,
        bool                        response_end) override;

    virtual void descriptor_discovered(
        uint16_t                    connection_handle,
        ble::att::error_code        gatt_error,
        uint16_t                    gatt_handle_error,
        uint16_t                    gatt_handle_desciptor,
        ble::att::uuid const&       uuid,
        bool                        response_end) override;

    virtual void attribute_discovered(
        uint16_t                    connection_handle,
        ble::att::error_code        gatt_error,
        uint16_t                    gatt_handle_error,
        uint16_t                    gatt_handle_attribute,
        ble::att::uuid const&       uuid,
        bool                        response_end) override;

    struct gatt_free_list {
        ble::gatt::service_list_type    services;
        ble::gatt::attribute::list_type characteristics;
        ble::gatt::attribute::list_type descriptors;
    };

    gatt_free_list free_list;

private:
    ble::gattc::discovery_operations&                   service_discovery;
    ble::gatt::service_container*                       service_container;
    std::pair<uint16_t, uint16_t>                       discovery_handle_range;
    ble::gatt::service_container::discovery_iterator    discovery_iterator;
    completion_notify*                                  completion_notification;

    /**
     * Once the characteristic descriptor discovery is complete this function
     * will trim the discovery_handle_range.second value to equal the last
     * characteristic handle in the service container. Unless there is a
     * disconnect or a service changed indication this value will mark the
     * service discovery end handle.
     */
    void trim_discovery_handle_range();
};

} // namespace gattc
} // namespace ble

