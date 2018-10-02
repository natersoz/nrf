/**
 * @file ble/peripheral_connection.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "ble/att.h"
#include "ble/gap_connection.h"
#include "ble/gap_advertising.h"
#include "ble/gap_request_response.h"

namespace ble
{
namespace gap
{

/**
 * @class ble::gap::peripheral_connection
 * A BLE peripheral connection.
 * This is the ble::gap::connection with advertising.
 */
class peripheral_connection: public connection
{
public:
    virtual ~peripheral_connection()                                = default;

    peripheral_connection()                                         = delete;
    peripheral_connection(peripheral_connection const&)             = delete;
    peripheral_connection(peripheral_connection &&)                 = delete;
    peripheral_connection& operator=(peripheral_connection const&)  = delete;
    peripheral_connection& operator=(peripheral_connection&&)       = delete;

    /** Constructor which uses the default connection parameters. */
    peripheral_connection(ble::gap::request_response&     request_response,
                          ble::gap::advertising&          advertising):
        super(request_response),
        advertising_(advertising)
    {}

    /** Constructor which specifies the peripheral_connection parameters. */
    peripheral_connection(ble::gap::request_response&            request_response,
                          ble::gap::advertising&                 advertising,
                          ble::gap::connection_parameters const& connection_parameters):
        super(request_response, connection_parameters),
        advertising_(advertising)
    {}

    ble::gap::advertising const& advertising() const {
        return this->advertising_;
    }

    ble::gap::advertising& advertising() { return this->advertising_; }

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
        this->advertising().start();
    }

private:
    using super = ble::gap::connection;

    ble::gap::advertising& advertising_;
};

} // namespace gap
} // namespace ble

