/**
 * @file ble/gattc_service_builder.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "ble/gatt_service_container.h"
#include "ble/gattc_discovery_observer.h"
#include "ble/gattc_operations.h"

#include <system_error>
#include <utility>

namespace ble
{
namespace gattc
{

/**
 * @class ble::gattc:service_builder
 * Implement the service discovery observer to recieve service discovery
 * responses and aggregate the service discovery operations to perform requests,
 * thereby building a container of services.
 */
class service_builder: public ble::gattc::discovery_observer
{
public:
    ~service_builder()                                  = default;

    service_builder()                                   = delete;
    service_builder(service_builder const&)             = delete;
    service_builder(service_builder &&)                 = delete;
    service_builder& operator=(service_builder const&)  = delete;
    service_builder& operator=(service_builder&&)       = delete;

    service_builder(ble::gattc::discovery_operations& service_discovery)
    : ble::gattc::discovery_observer(),
      service_container_(nullptr),
      service_discovery_(service_discovery),
      handle_discovery_range_(ble::att::handle_invalid, ble::att::handle_invalid)
    {
    }

    /**
     * Discover all possible services published by the GATT server identified
     * by the connection handle.
     *
     * @param connection_handle The connection handle specifying the GATT server
     *                          to which a BLE has been established.
     * @param service_container The service container to build up with services
     *                          and the services attributes (characteristics,
     *                          descriptors, etc.).
     *
     * @return std::errc        Whether the call to the silicon vendor call
     *                          was a success or failure.
     */
    std::errc discover_services(uint16_t                      connection_handle,
                                ble::gatt::service_container& service_container);
    /**
     * Discover the services published by the GATT server identified by the
     * connection handle within the GATT handle range [handle_bein: handle_last].
     */
    std::errc discover_services(uint16_t                      connection_handle,
                                ble::gatt::service_container& service_container,
                                uint16_t                      handle_first,
                                uint16_t                      handle_last);

    virtual void service_discovered(
        uint16_t                    connection_handle,
        ble::att::error_code        error_code,
        uint16_t                    gatt_handle_error,
        uint16_t                    gatt_handle_first,
        uint16_t                    gatt_handle_last,
        ble::att::uuid const&       uuid,
        bool                        response_end) override;

    virtual void relationship_discovered(
        uint16_t                    connection_handle,
        ble::att::error_code        error_code,
        uint16_t                    gatt_handle_error,
        uint16_t                    gatt_handle_first,
        uint16_t                    gatt_handle_last,
        uint16_t                    service_handle,
        ble::att::uuid const&       uuid,
        bool                        response_end) override;

    virtual void characteristic_discovered(
        uint16_t                    connection_handle,
        ble::att::error_code        error_code,
        uint16_t                    gatt_handle_error,
        uint16_t                    gatt_handle_declaration,
        uint16_t                    gatt_handle_value,
        ble::att::uuid const&       uuid,
        ble::gatt::properties       properties,
        bool                        response_end) override;

    virtual void descriptor_discovered(
        uint16_t                    connection_handle,
        ble::att::error_code        error_code,
        uint16_t                    gatt_handle_error,
        uint16_t                    gatt_handle_desciptor,
        ble::att::uuid const&       uuid,
        bool                        response_end) override;

    virtual void attribute_discovered(
        uint16_t                    connection_handle,
        ble::att::error_code        error_code,
        uint16_t                    gatt_handle_error,
        uint16_t                    gatt_handle_attribute,
        ble::att::uuid const&       uuid,
        bool                        response_end) override;

private:
    ble::gatt::service_container*       service_container_;
    ble::gattc::discovery_operations&   service_discovery_;
    std::pair<uint16_t, uint16_t>       handle_discovery_range_;
};

} // namespace gattc
} // namespace ble

