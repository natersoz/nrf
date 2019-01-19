/**
 * @file ble/gap_connection.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "ble/att.h"
#include "ble/gap_types.h"
#include "ble/gap_connection_parameters.h"
#include "ble/gap_event_observer.h"
#include "ble/gap_operations.h"
#include "ble/profile_connectable_accessor.h"

namespace ble
{
namespace gap
{

/**
 * @class ble::gap::connection
 * Tie together the gap::event_observer (inheritance) and
 * gap_operations (aggregation).
 */
class connection: public ble::gap::event_observer,
                  public ble::profile::connectable_accessor
{
public:
    class negotiation_state
    {
    public:
        virtual ~negotiation_state()                           = default;

        negotiation_state(negotiation_state const&)            = delete;
        negotiation_state(negotiation_state &&)                = delete;
        negotiation_state& operator=(negotiation_state const&) = delete;
        negotiation_state& operator=(negotiation_state&&)      = delete;

        negotiation_state();

        bool is_any_update_pending() const;
        void clear_all_pending();

        bool is_gatt_mtu_exchange_pending() const;
        bool is_gap_connection_parameters_pending() const;
        bool is_link_layer_update_pending() const;
        bool is_phy_layer_update_pending() const;

        void set_gatt_mtu_exchange_pending(bool is_pending);
        void set_gap_connection_parameters_pending(bool is_pending);
        void set_link_layer_update_pending(bool is_pending);
        void set_phy_layer_update_pending(bool is_pending);

    protected:
        void set_pending_state_update(bool is_pending);
        virtual void timer_start() {};
        virtual void timer_stop() {};

    private:
        bool gatt_mtu_exchange_pending_;
        bool gap_connection_parameters_update_pending_;
        bool link_layer_update_pending_;
        bool phy_layer_update_pending_;
    };

    virtual ~connection()                       = default;

    connection()                                = delete;
    connection(connection const&)               = delete;
    connection(connection &&)                   = delete;
    connection& operator=(connection const&)    = delete;
    connection& operator=(connection&&)         = delete;

    /** Constructor which uses the default connection parameters. */
    connection(ble::gap::operations& gap_operations):
        super(),
        handle_(handle_invalid),
        mtu_size_(ble::att::mtu_length_minimum),
        operations_(gap_operations),
        connection_parameters_()
    {}

    /** Constructor which specifies the connection parameters. */
    connection(ble::gap::operations&                    gap_operations,
               ble::gap::connection_parameters const&   connection_parameters):
        super(),
        handle_(handle_invalid),
        mtu_size_(ble::att::mtu_length_minimum),
        operations_(gap_operations),
        connection_parameters_(connection_parameters),
        negotiation_state_()
    {}

    ble::gap::operations& operations() {
        return this->operations_;
    }

    ble::gap::operations const& operations() const {
        return this->operations_;
    }

    uint16_t get_connection_handle() const { return this->handle_; }
    bool is_connected() const { return (this->handle_ != gap::handle_invalid); }

    connection_parameters const& get_connection_parameters() const {
        return this->connection_parameters_;
    }

    void set_connection_parameters(connection_parameters const& parameters) {
        this->connection_parameters_ = parameters;
    }

    negotiation_state const& get_negotiation_state() const {
        return this->negotiation_state_;
    }

    negotiation_state& get_negotiation_state() {
        return this->negotiation_state_;
    }

protected:
    void set_handle(uint16_t handle) { this->handle_ = handle; }

    /**
     * @note This method overrides ble::gap::event_observer::connect()
     * with the default behavior of setting this classes' connection handle
     * A client wishing to connect would call this->operations_.connect();
     *
     * @param connection_handle The conneciton handle of the new connection.
     * @param peer_address      The peer address to which we connected.
     * @param peer_address_id   A nordic index into nordic's stored addresses.
     *
     * @todo remove peer_address_id - this is Nordic specific.
     */
    void connect(uint16_t                   connection_handle,
                 ble::gap::address const&   peer_address,
                 uint8_t                    peer_address_id) override;

    /**
     * @note This method overrides ble::gap::event_observer::disconnect()
     * with the default behavior of invalidating the connection handle.
     * A client wishing to connect would call this->operations_.disconnect();
     *
     * @param connection_handle The connection that was disconnected.
     * @param error_code        The reason for disconnection.
     */
    void disconnect(uint16_t                connection_handle,
                    ble::hci::error_code    error_code) override;

private:
    using super = ble::gap::event_observer;

    uint16_t                        handle_;
    ble::att::length_t              mtu_size_;
    ble::gap::operations&           operations_;
    ble::gap::connection_parameters connection_parameters_;
    negotiation_state               negotiation_state_;
};

} // namespace gap
} // namespace ble
