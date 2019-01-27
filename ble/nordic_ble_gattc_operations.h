/**
 * @file ble/nordic_ble_gattc_operations.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Nordic GATTC BLE interface implementation.
 */

#pragma once

#include "ble/gattc_operations.h"

namespace nordic
{

class ble_gattc_discovery_operations: public ble::gattc::discovery_operations
{
private:
    using super = ble::gattc::discovery_operations;

public:
    virtual ~ble_gattc_discovery_operations()                                         = default;

    ble_gattc_discovery_operations(ble_gattc_discovery_operations const&)             = delete;
    ble_gattc_discovery_operations(ble_gattc_discovery_operations &&)                 = delete;
    ble_gattc_discovery_operations& operator=(ble_gattc_discovery_operations const&)  = delete;
    ble_gattc_discovery_operations& operator=(ble_gattc_discovery_operations&&)       = delete;

    ble_gattc_discovery_operations() :
        super(),
        last_requested_(ble::att::handle_invalid, ble::att::handle_invalid)
    {}

    virtual std::errc discover_primary_services(
        uint16_t connection_handle,
        uint16_t gatt_handle_start,
        uint16_t gatt_handle_stop) override;

    virtual std::errc discover_service_relationships(
        uint16_t connection_handle,
        uint16_t gatt_handle_start,
        uint16_t gatt_handle_stop) override;

    virtual std::errc discover_characteristics(
        uint16_t connection_handle,
        uint16_t gatt_handle_start,
        uint16_t gatt_handle_stop) override;

    virtual std::errc discover_descriptors(
        uint16_t connection_handle,
        uint16_t gatt_handle_start,
        uint16_t gatt_handle_stop) override;

    virtual std::errc discover_attributes(
        uint16_t connection_handle,
        uint16_t gatt_handle_start,
        uint16_t gatt_handle_stop) override;

    virtual ble::att::handle_range gatt_handles_requested() const override {
        return this->last_requested_;
    }

private:
    ble::att::handle_range last_requested_;
};

class ble_gattc_operations: public ble::gattc::operations
{
public:
    virtual ~ble_gattc_operations()                                 = default;

    ble_gattc_operations()                                          = default;
    ble_gattc_operations(ble_gattc_operations const&)               = delete;
    ble_gattc_operations(ble_gattc_operations &&)                   = delete;
    ble_gattc_operations& operator=(ble_gattc_operations const&)    = delete;
    ble_gattc_operations& operator=(ble_gattc_operations&&)         = delete;

    virtual std::errc read(uint16_t                 connection_handle,
                           uint16_t                 attribute_handle,
                           ble::att::length_t       offset) override;

    /// @todo Implement read_multiple, (maybe)
//    virtual std::errc read_multiple(uint16_t    connection_handle) override;

    virtual std::errc write_request(uint16_t            connection_handle,
                                    uint16_t            attribute_handle,
                                    void const*         data,
                                    ble::att::length_t  offset,
                                    ble::att::length_t  length) override;

    virtual std::errc write_command(uint16_t            connection_handle,
                                    uint16_t            attribute_handle,
                                    void const*         data,
                                    ble::att::length_t  offset,
                                    ble::att::length_t  length) override;

    virtual std::errc write_command_signed(uint16_t             connection_handle,
                                           uint16_t             attribute_handle,
                                           void const*          data,
                                           ble::att::length_t   offset,
                                           ble::att::length_t   length) override;

    virtual std::errc write_prepare(uint16_t            connection_handle,
                                    uint16_t            attribute_handle,
                                    void const*         data,
                                    ble::att::length_t  offset,
                                    ble::att::length_t  length) override;

    virtual std::errc write_execute(uint16_t            connection_handle,
                                    uint16_t            attribute_handle,
                                    void const*         data,
                                    ble::att::length_t  offset,
                                    ble::att::length_t  length) override;

    virtual std::errc write_cancel(uint16_t             connection_handle,
                                   uint16_t             attribute_handle,
                                   void const*          data,
                                   ble::att::length_t   offset,
                                   ble::att::length_t   length) override;

    /**
     * Provide a Handle Value Confirmation Response.
     * Used to provide a confirmation to the server that the indication has
     * been processed.
     *
     * @see BLUETOOTH SPECIFICATION Version 5.0 | Vol 3, Part G page 2268
     * 4.11 CHARACTERISTIC VALUE INDICATIONS, Figure 4.18: Indications example
     */
    virtual std::errc handle_value_confirm(uint16_t connection_handle,
                                           uint16_t attribute_handle) override;

    /**
     * Send a request to the server to set a desired GATT MTU size.
     *
     * @see BLUETOOTH SPECIFICATION Version 5.0 | Vol 3, Part G page 2276
     * 5.2 LE L2CAP INTEROPERABILITY REQUIREMENTS, 5.2.1 ATT_MTU
     *
     * @return std::errc
     */
    virtual std::errc exchange_mtu_request(uint16_t connection_handle,
                                           ble::att::length_t mtu_size) override;

    /**
     * At this point it appears that 128-bit UUIDs cannot be handled by the
     * Nordic softdevice unless they are pre-loaded into the softdevice UUID
     * database. This is a complete kludge and a huge disappointment.
     * It rules out using the Nordic softdevice in a generic GATT client.
     *
     * @note This operation must be performed after the softdevice has been both
     * initialized and enabled.
     *
     * @param uuid The generic UUID to preload into the softdevice database.
     *
     * @return std::errc The result of this operation.
     */
    std::errc preload_custom_uuid(ble::att::uuid const &uuid);
};

} // namespace nordic
