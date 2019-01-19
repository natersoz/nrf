/**
 * @file ble_peripheral_class/ble_gap_connection.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "ble_gap_connection.h"

#include "ble/profile_connectable.h"
#include "ble/gap_advertising_data.h"

#include "logger.h"
#include "std_error.h"

#include <algorithm>
#include <cstring>

ble_gap_connection::~ble_gap_connection()
{
}

ble_gap_connection::ble_gap_connection(
    ble::gap::operations&                   operations,
    ble::gap::scanning&                     scanning,
    ble::gap::connection_parameters const&  conn_params,
    ble::att::length_t                      mtu_size)
    :   super(operations, scanning, conn_params),
        mtu_size_(mtu_size)
{
}

void ble_gap_connection::connect(uint16_t                    connection_handle,
                                 ble::gap::address const&    peer_address,
                                 uint8_t                     peer_address_id)
{
    super::connect(connection_handle, peer_address, peer_address_id);
    logger::instance().debug("gap::connect: 0x%04x", this->get_connection_handle());

    this->get_negotiation_state().set_gap_connection_parameters_pending(true);
    this->get_negotiation_state().set_gatt_mtu_exchange_pending(true);

    this->operations().connection_parameter_update_request(
        this->get_connection_handle(), this->get_connection_parameters());
    this->get_connecteable()->gattc()->exchange_mtu_request(
        connection_handle, this->mtu_size_);
}

void ble_gap_connection::disconnect(uint16_t                connection_handle,
                                    ble::hci::error_code    error_code)
{
    super::disconnect(connection_handle, error_code);
    logger::instance().debug("gap::disconnect: 0x%04x -> 0x%04x, reason: 0x%02x",
                             connection_handle, this->get_connection_handle(), error_code);

    /// @todo Note that scanning restarts automatically when the Nordic
    /// central is disconnected. This is observered behavior and specific to Nordic.
//    this->scanning().start();
}

void ble_gap_connection::timeout_expiration(
    uint16_t                    connection_handle,
    ble::gap::timeout_reason    reason)
{
}

void ble_gap_connection::connection_parameter_update(
    uint16_t                                connection_handle,
    ble::gap::connection_parameters const&  connection_parameters)
{
    logger::instance().debug(
        "gap::connection_parameter_update: h: 0x%04x, "
        "interval: (%u, %u), latency: %u, sup_timeout: %u",
        connection_handle,
        connection_parameters.interval_min,
        connection_parameters.interval_max,
        connection_parameters.slave_latency,
        connection_parameters.supervision_timeout);

    this->get_negotiation_state().set_gap_connection_parameters_pending(false);
    if (not this->get_negotiation_state().is_any_update_pending())
    {
        logger::instance().debug("--- pending updates complete ---");

        /// @todo This is a kludgey way to do initiate service discovery
        /// From the get_negotiation_state object.
        /// Clean it up by adding an observable interface to negotiation_state
        /// completion.
        /// @note This is also making the assumption that the last update is
        /// the conection parameter update.
        this->get_connecteable()->service_builder()->discover_services(
            connection_handle, this->get_connecteable()->service_container());
    }
}

void ble_gap_connection::connection_parameter_update_request(
    uint16_t                                connection_handle,
    ble::gap::connection_parameters const&  connection_parameters)
{
}

void ble_gap_connection::phy_update_request(
    uint16_t                        connection_handle,
    ble::gap::phy_layer_parameters  phy_tx_preferred,
    ble::gap::phy_layer_parameters  phy_rx_preferred)
{
}

void ble_gap_connection::phy_update(
    uint16_t                        connection_handle,
    ble::hci::error_code            status,
    ble::gap::phy_layer_parameters  phy_tx,
    ble::gap::phy_layer_parameters  phy_rx)
{
}

void ble_gap_connection::link_layer_update_request(
    uint16_t    connection_handle,
    uint16_t    tx_length_max,
    uint16_t    rx_length_max,
    uint16_t    tx_interval_usec_max,
    uint16_t    rx_interval_usec_max)
{
}

void ble_gap_connection::link_layer_update(uint16_t    connection_handle,
                                           uint16_t    tx_length_max,
                                           uint16_t    rx_length_max,
                                           uint16_t    tx_interval_usec_max,
                                           uint16_t    rx_interval_usec_max)
{
}

void ble_gap_connection::security_request(
    uint16_t                                            connection_handle,
    bool                                                bonding,
    ble::gap::security::authentication_required const&  auth_req)
{
}

void ble_gap_connection::security_pairing_request(
    uint16_t                                  connection_handle,
    bool                                      bonding,
    ble::gap::security::pairing_request const &pair_req)
{
}

void ble_gap_connection::security_authentication_key_request(
    uint16_t    connection_handle,
    uint8_t     key_type)
{
}

void ble_gap_connection::security_information_request(
    uint16_t                                    connection_handle,
    ble::gap::security::key_distribution const& key_dist,
    ble::gap::security::master_id const&        mater_id,
    ble::gap::address const&                    peer_address)
{
}

void ble_gap_connection::security_passkey_display(
    uint16_t                                connection_handle,
    ble::gap::security::pass_key const&     passkey,
    bool                                    match_request)
{
}

void ble_gap_connection::security_key_pressed(
    uint16_t                            connection_handle,
    ble::gap::security::passkey_event   key_press_event)
{
}

void ble_gap_connection::security_DH_key_calculation_request(
    uint16_t                                connection_handle,
    ble::gap::security::pubk const&         public_key,
    bool                                    oob_required)
{
}

void ble_gap_connection::security_authentication_status(
    uint16_t                                    connection_handle,
    ble::gap::security::pairing_failure         pairing_status,
    uint8_t                                     error_source,
    bool                                        is_bonded,
    uint8_t                                     sec_mode_1_levels,
    uint8_t                                     sec_mode_2_levels,
    ble::gap::security::key_distribution const& kdist_own,
    ble::gap::security::key_distribution const& kdist_peer)
{
}

void ble_gap_connection::connection_security_update(uint16_t connection_handle,
                                                    uint8_t  security_mode,
                                                    uint8_t  security_level,
                                                    uint8_t  key_size)
{
}

void ble_gap_connection::rssi_update(uint16_t connection_handle,
                                     int8_t   rssi_dBm)
{
}

static bool check_name(ble::gap::ltv_data const &ltv)
{
    constexpr char const  device_name[]      = "periph";
    constexpr std::size_t device_name_length = std::size(device_name) - 1u;

    return strncmp(device_name, reinterpret_cast<char const *>(ltv.data()),
                   device_name_length) == 0;
}

void ble_gap_connection::advertising_report(
    uint16_t                    connection_handle,
    ble::gap::address const&    peer_address,
    ble::gap::address const&    direct_address,
    int8_t                      rssi_dBm,
    bool                        scan_response,
    void const*                 data,
    uint8_t                     data_length)
{
    super::advertising_report(connection_handle,
                              peer_address,
                              direct_address,
                              rssi_dBm,
                              scan_response,
                              data,
                              data_length);

    ble::gap::advertising_data const advertising_data(data, data_length);

    // Note: Since advertising data may be malformed, and the iterator relies
    // on the length parameter, don't rely on the iterator != operator for
    // loop completion.
    // Use < rather than != to bound malformed advertising data.
    for (ble::gap::advertising_data::iterator adv_iter = advertising_data.begin();
         adv_iter < advertising_data.end(); ++adv_iter)
    {
        ble::gap::ltv_data const& ltv_data = *adv_iter;
        switch (ltv_data.type())
        {
        case ble::gap::type::local_name_complete:
        case ble::gap::type::local_name_short:
            logger::instance().debug("name: ");
            logger::instance().write_data(logger::level::debug,
                                          ltv_data.data(),
                                          ltv_data.length() - 1u,
                                          true);
            if (check_name(ltv_data))
            {
                logger &logger = logger::instance();
                logger.debug("connect attempt: addr type: %u", peer_address.type);
                logger.write_data(logger::level::debug,
                                  peer_address.octets.data(),
                                  ble::gap::address::octet_length);
                std::errc const error_code = this->scanning().connect(
                    peer_address, this->get_connection_parameters());

                if (is_success(error_code))
                {
                    // Connection established.
                    // Return here to avoid calling scanning functions.
                    // We are done scanning.
                    return;
                }
                else
                {
                    ASSERT(0);
                }
            }
            break;

        default:
            break;
        }
    }

    // For Nordic, each time a report is issued scanning needs to be stopped and
    // restarted to get another report.
    // See comments at the top of struct ble_gap_evt_adv_report_t:
    // "scanning will be paused."
    // If the scan is not stopped before restarting then the
    // error NRF_ERROR_INVALID_STATE is returned.
    // This is a rather stupid implementation on Nordic's part.
    this->scanning().stop();
    this->scanning().start();
}

void ble_gap_connection::scan_report_request(
    uint16_t                    connection_handle,
    ble::gap::address const&    peer_address,
    int8_t                      rssi_dBm)
{
}

