/**
 * @file ble/central_connection.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "ble/att.h"
#include "ble/gap_connection.h"
#include "ble/gap_advertising.h"
#include "ble/gap_operations.h"

namespace ble
{
namespace gap
{

/**
 * @class ble::gap::central_connection
 * A BLE central connection.
 * This is the ble::gap::connection with scanning.
 */
class central_connection: public connection
{
public:
    virtual ~central_connection()                             = default;

    central_connection()                                      = delete;
    central_connection(central_connection const&)             = delete;
    central_connection(central_connection &&)                 = delete;
    central_connection& operator=(central_connection const&)  = delete;
    central_connection& operator=(central_connection&&)       = delete;

    /** Constructor which uses the default connection parameters. */
    central_connection(ble::gap::operations& operations):
        super(operations)
    {}

    /** Constructor which specifies the central_connection parameters. */
    central_connection(
        ble::gap::operations&                   operations,
        ble::gap::connection_parameters const&  connection_parameters)
        : super(operations, connection_parameters)
    {}

    /**
     * Scan for advertising peripherals.
     * @todo If the peripheral supports responses then scan for that also?
     */
    void scan();

protected:
    void connect(uint16_t                   connection_handle,
                 ble::gap::address const&   peer_address,
                 uint8_t                    peer_address_id) override
    {
        super::connect(connection_handle, peer_address, peer_address_id);
    }

    void disconnect(uint16_t                connection_handle,
                    ble::hci::error_code    error_code) override
    {
        super::disconnect(connection_handle, error_code);
    }

private:
    using super = ble::gap::connection;
};

} // namespace gap
} // namespace ble

