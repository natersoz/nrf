/**
 * @file ble/gap_event_loggger.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "ble/gap_connection.h"
#include "logger.h"

namespace ble
{
namespace gap
{

/**
 * @class event_logger
 * Inherit from gap::event_observer so that this class can be registered as
 * an observer. It can be used merely for logging events without any other
 * behavior modification.
 */
class event_logger: public ble::gap::event_observer
{
public:
    logger::level log_level;

    virtual ~event_logger() override              = default;

    event_logger(event_logger const&)             = delete;
    event_logger(event_logger &&)                 = delete;
    event_logger& operator=(event_logger const&)  = delete;
    event_logger& operator=(event_logger&&)       = delete;

    event_logger() : log_level(logger::level::debug) {}

    explicit event_logger(logger::level level): log_level(level) {}

    void connect(
        uint16_t                            connection_handle,
        gap::address const&                 peer_address,
        uint8_t                             peer_address_id) override;

    void disconnect(
        uint16_t                            connection_handle,
        hci::error_code                     error_code) override;

    void timeout_expiration(
        uint16_t                            connection_handle,
        timeout_reason                      reason) override;

    void connection_parameter_update(
        uint16_t                            connection_handle,
        connection_parameters const&        connection_parameters) override;

    void connection_parameter_update_request(
        uint16_t                            connection_handle,
        connection_parameters const&        connection_parameters) override;

    void phy_update_request(
        uint16_t                            connection_handle,
        phy_layer_parameters                phy_rx_preferred,
        phy_layer_parameters                phy_tx_preferred) override;

    void phy_update(
        uint16_t                            connection_handle,
        ble::hci::error_code                status,
        phy_layer_parameters                phy_rx,
        phy_layer_parameters                phy_tx) override;

    void link_layer_update_request(
        uint16_t                            connection_handle,
        uint16_t                            rx_length_max,
        uint16_t                            rx_interval_usec_max,
        uint16_t                            tx_length_max,
        uint16_t                            tx_interval_usec_max) override;

    void link_layer_update(
        uint16_t                            connection_handle,
        uint16_t                            rx_length_max,
        uint16_t                            rx_interval_usec_max,
        uint16_t                            tx_length_max,
        uint16_t                            tx_interval_usec_max) override;

    void security_request(
        uint16_t                                    connection_handle,
        bool                                        bonding,
        security::authentication_required const&    auth_req) override;

    void security_pairing_request(
        uint16_t                            connection_handle,
        bool                                bonding,
        security::pairing_request           const& pair_req) override;

    void security_authentication_key_request(
        uint16_t                            connection_handle,
        uint8_t                             key_type) override;

    void security_information_request(
        uint16_t                            connection_handle,
        security::key_distribution const&   key_dist,
        security::master_id const&          master_id,
        gap::address const&                 peer_address) override;

    void security_passkey_display(
        uint16_t                            connection_handle,
        security::pass_key const&           passkey,
        bool                                match_request) override;

    void security_key_pressed(
        uint16_t                            connection_handle,
        security::passkey_event             key_press_event) override;

    void security_DH_key_calculation_request(
        uint16_t                            connection_handle,
        security::pubk const&               public_key,
        bool                                oob_required) override;

    void security_authentication_status(
        uint16_t                            connection_handle,
        ble::gap::security::pairing_failure pairing_status,
        uint8_t                             error_source,
        bool                                is_bonded,
        uint8_t                             sec_mode_1_levels,
        uint8_t                             sec_mode_2_levels,
        security::key_distribution const&   kdist_own,
        security::key_distribution const&   kdist_peer) override;

    void connection_security_update(
        uint16_t                            connection_handle,
        uint8_t                             security_mode,
        uint8_t                             security_level,
        uint8_t                             key_size) override;

    void rssi_update(
        uint16_t                            connection_handle,
        int8_t                              rssi_dBm) override;

    void advertising_report(
        uint16_t                            connection_handle,
        gap::address const&                 peer_address,
        gap::address const&                 direct_address,
        int8_t                              rssi_dBm,
        bool                                scan_response,
        void const*                         data,
        uint8_t                             data_length) override;

    void scan_report_request(
        uint16_t                            connection_handle,
        gap::address const&                 peer_address,
        int8_t                              rssi_dBm) override;
};

} // namespace gap
} // namespace ble

