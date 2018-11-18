/**
 * @file ble/central_connection.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "ble/att.h"
#include "ble/gap_connection.h"
#include "ble/gap_scanning.h"
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
private:
    using super = ble::gap::connection;

public:
    virtual ~central_connection()                             = default;

    central_connection()                                      = delete;
    central_connection(central_connection const&)             = delete;
    central_connection(central_connection &&)                 = delete;
    central_connection& operator=(central_connection const&)  = delete;
    central_connection& operator=(central_connection&&)       = delete;

    central_connection(ble::gap::operations&    operations,
                       ble::gap::scanning&      scanning)
    : super(operations),
      scanning_(scanning)
    {}

    ble::gap::scanning const& scanning() const { return this->scanning_; }
    ble::gap::scanning&       scanning()       { return this->scanning_; }

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
        this->scanning().start();
    }

private:
    ble::gap::scanning& scanning_;
};

} // namespace gap
} // namespace ble

