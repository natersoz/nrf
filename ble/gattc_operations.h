/**
 * @file ble/gattc_operations.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Generic GATTC BLE.
 */

#pragma once

#include "ble/att.h"
#include "ble/uuid.h"
#include "ble/profile_connectable_accessor.h"
#include "std_error.h"

#include <system_error>
#include <utility>
#include <cstdint>

namespace ble
{
namespace gattc
{

class service_discovery
//: public ble::profile::connectable_accessor   /// @todo remove?
{
public:
    virtual ~service_discovery()                            = default;

    service_discovery()                                     = default;
    service_discovery(service_discovery const&)             = delete;
    service_discovery(service_discovery &&)                 = delete;
    service_discovery& operator=(service_discovery const&)  = delete;
    service_discovery& operator=(service_discovery&&)       = delete;

    virtual std::errc discover_primary_services(uint16_t connection_handle,
                                                uint16_t gatt_handle_start) = 0;

    virtual std::errc discover_primary_services(uint16_t connection_handle,
                                                uint16_t gatt_handle_start,
                                                ble::att::uuid const& uuid) = 0;

    virtual std::errc discover_service_relationships(uint16_t connection_handle,
                                                     uint16_t gatt_handle_start,
                                                     uint16_t gatt_handle_stop) = 0;

    virtual std::errc discover_characteristics(uint16_t connection_handle,
                                               uint16_t gatt_handle_start,
                                               uint16_t gatt_handle_stop) = 0;

    virtual std::errc discover_descriptors(uint16_t connection_handle,
                                           uint16_t gatt_handle_start,
                                           uint16_t gatt_handle_stop) = 0;

    virtual std::errc discover_attributes(uint16_t connection_handle,
                                          uint16_t gatt_handle_start,
                                          uint16_t gatt_handle_stop) = 0;
};

class operations: public ble::profile::connectable_accessor
{
public:
    virtual ~operations()                       = default;

    operations()                                = default;
    operations(operations const&)               = delete;
    operations(operations &&)                   = delete;
    operations& operator=(operations const&)    = delete;
    operations& operator=(operations&&)         = delete;

    virtual service_discovery const& sdp() const = 0;

    service_discovery& sdp() {
        return const_cast<service_discovery&>(std::as_const(*this).sdp());
    }

    virtual std::errc read(uint16_t                 connection_handle,
                           uint16_t                 attribute_handle,
                           att::length_t            offset) = 0;

    /// @todo Implement read_multiple, (maybe)
//    virtual std::errc read_multiple(uint16_t    connection_handle) = 0;

    virtual std::errc write_request(uint16_t        connection_handle,
                                    uint16_t        attribute_handle,
                                    void const*     data,
                                    att::length_t   offset,
                                    att::length_t   length) = 0;

    virtual std::errc write_command(uint16_t        connection_handle,
                                    uint16_t        attribute_handle,
                                    void const*     data,
                                    att::length_t   offset,
                                    att::length_t   length) = 0;

    virtual std::errc write_command_signed(uint16_t connection_handle,
                                           uint16_t        attribute_handle,
                                           void const*     data,
                                           att::length_t   offset,
                                           att::length_t   length) = 0;

    virtual std::errc write_prepare(uint16_t        connection_handle,
                                    uint16_t        attribute_handle,
                                    void const*     data,
                                    att::length_t   offset,
                                    att::length_t   length) = 0;

    virtual std::errc write_execute(uint16_t        connection_handle,
                                    uint16_t        attribute_handle,
                                    void const*     data,
                                    att::length_t   offset,
                                    att::length_t   length) = 0;

    virtual std::errc write_cancel(uint16_t         connection_handle,
                                   uint16_t        attribute_handle,
                                   void const*     data,
                                   att::length_t   offset,
                                   att::length_t   length) = 0;

    /**
     * Provide a Handle Value Confirmation Response.
     * Used to provide a confirmation to the server that the indication has
     * been processed.
     *
     * @see BLUETOOTH SPECIFICATION Version 5.0 | Vol 3, Part G page 2268
     * 4.11 CHARACTERISTIC VALUE INDICATIONS, Figure 4.18: Indications example
     */
    virtual std::errc handle_value_confirm(uint16_t connection_handle,
                                           uint16_t attribute_handle) = 0;

    /**
     * Send a request to the server to set a desired GATT MTU size.
     *
     * @see BLUETOOTH SPECIFICATION Version 5.0 | Vol 3, Part G page 2276
     * 5.2 LE L2CAP INTEROPERABILITY REQUIREMENTS, 5.2.1 ATT_MTU
     *
     * @return std::errc
     */
    virtual std::errc exchange_mtu_request(uint16_t         connection_handle,
                                           att::length_t    mtu_size) = 0;
};

} // namespace ble
} // namespace gattc


