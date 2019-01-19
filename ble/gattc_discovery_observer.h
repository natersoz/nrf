/**
 * @file ble/gattc_discovery_observer.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "ble/att.h"
#include "ble/gatt_declaration.h"
#include "ble/uuid.h"

namespace ble
{
namespace gattc
{

/**
 * @interface discovery_observer
 * An interface for defining how a class can aquire service discovery response
 * messages.
 */
class discovery_observer
{
public:
    virtual ~discovery_observer()                            = default;

    discovery_observer()                                     = default;
    discovery_observer(discovery_observer const&)            = delete;
    discovery_observer(discovery_observer &&)                = delete;
    discovery_observer& operator=(discovery_observer const&) = delete;
    discovery_observer& operator=(discovery_observer&&)      = delete;

    virtual void service_discovered(
        uint16_t                    connection_handle,
        ble::att::error_code        error_code,
        uint16_t                    gatt_handle_error,
        uint16_t                    gatt_handle_first,
        uint16_t                    gatt_handle_last,
        ble::att::uuid const&       uuid,
        bool                        response_end) = 0;

    virtual void relationship_discovered(
        uint16_t                    connection_handle,
        ble::att::error_code        error_code,
        uint16_t                    gatt_handle_error,
        uint16_t                    gatt_handle_first,
        uint16_t                    gatt_handle_last,
        uint16_t                    service_handle,
        ble::att::uuid const&       uuid,
        bool                        response_end) = 0;

    virtual void characteristic_discovered(
        uint16_t                    connection_handle,
        ble::att::error_code        error_code,
        uint16_t                    gatt_handle_error,
        uint16_t                    gatt_handle_declaration,
        uint16_t                    gatt_handle_value,
        ble::att::uuid const&       uuid,
        ble::gatt::properties       properties,
        bool                        response_end) = 0;

    virtual void descriptor_discovered(
        uint16_t                    connection_handle,
        ble::att::error_code        error_code,
        uint16_t                    gatt_handle_error,
        uint16_t                    gatt_handle_desciptor,
        ble::att::uuid const&       uuid,
        bool                        response_end) = 0;

    virtual void attribute_discovered(
        uint16_t                    connection_handle,
        ble::att::error_code        error_code,
        uint16_t                    gatt_handle_error,
        uint16_t                    gatt_handle_attribute,
        ble::att::uuid const&       uuid,
        bool                        response_end) = 0;
};

} // namespace gattc
} // namespace ble

