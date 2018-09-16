/**
 * @file ble/gap_connection.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "ble/att.h"
#include "ble/gap_types.h"
#include "ble/gap_connection_parameters.h"
#include "ble/gap_event_observer.h"

namespace ble
{
namespace gap
{

/**
 * @class ble::gap::connection
 * @note The inherited GAP event observer is protected from meddling.
 */
class connection: protected ble::gap::event_observer
{
public:
    virtual ~connection()                       = default;

    connection(connection const&)               = delete;
    connection(connection &&)                   = delete;
    connection& operator=(connection const&)    = delete;
    connection& operator=(connection&&)         = delete;

    /** Constructor which uses the default connection parameters. */
    connection():
        super(),
        handle_(invalid_handle),
        mtu_size_(ble::att::mtu_length_minimum),
        parameters_()
    {}

    /** Constructor which specifies the connection parameters. */
    connection(ble::gap::connection_parameters const& connection_parameters):
        super(),
        handle_(invalid_handle),
        mtu_size_(ble::att::mtu_length_minimum),
        parameters_(connection_parameters)
    {}

    uint16_t get_handle() const { return this->handle_; }
    bool is_connected() const { return (this->handle_ != gap::invalid_handle); }

    connection_parameters const& get_parameters() const {
        return this->parameters_;
    }

    void set_parameters(connection_parameters const& parameters) {
        this->parameters_ = parameters;
    }

    /// Override these virtual functions with device specific implementations.
    /// @todo TBD:
//    virtual void negotiate_ppcp() = 0;
//    virtual void negotiate_mtu() = 0;
//    virtual void disconnect(ble::hci::error_code reason) = 0;

protected:
    void set_handle(uint16_t handle) { this->handle_ = handle; }

    void connect(uint16_t                   connection_handle,
                 ble::gap::address const&   peer_address,
                 uint8_t                    peer_address_id) override
    {
        this->set_handle(connection_handle);
    }

    void disconnect(uint16_t                connection_handle,
                    ble::hci::error_code    error_code) override
    {
        this->set_handle(ble::gap::invalid_handle);
    }





private:
    using super = ble::gap::event_observer;

    uint16_t                        handle_;
    ble::att::length_t              mtu_size_;
    ble::gap::connection_parameters parameters_;
};

} // namespace gap
} // namespace ble

