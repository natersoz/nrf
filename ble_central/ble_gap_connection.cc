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
        mtu_size_(mtu_size),
        negotiation_complete_(this)
{
    this->get_negotiation_state().set_completion_notification(&this->negotiation_complete_);
}

void ble_gap_connection::connect(uint16_t                    connection_handle,
                                 ble::gap::address const&    peer_address,
                                 uint8_t                     peer_address_id)
{
    super::connect(connection_handle, peer_address, peer_address_id);
    logger::instance().info("gap::connect: 0x%04x", this->get_connection_handle());

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
    logger::instance().info(
        "gap::disconnect: 0x%04x -> 0x%04x, reason: 0x%02x",
        connection_handle, this->get_connection_handle(), error_code);

    /// @todo Note that scanning restarts automatically when the Nordic
    /// central is disconnected.
    /// This is observered behavior and specific to Nordic.
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
    logger::instance().info(
        "gap::connection_parameter_update: h: 0x%04x, "
        "interval: (%u, %u), latency: %u, sup_timeout: %u",
        connection_handle,
        connection_parameters.interval_min,
        connection_parameters.interval_max,
        connection_parameters.slave_latency,
        connection_parameters.supervision_timeout);

    this->get_negotiation_state().set_gap_connection_parameters_pending(false);
}

void ble_gap_connection::connection_parameter_update_request(
    uint16_t                                connection_handle,
    ble::gap::connection_parameters const&  connection_parameters)
{
    logger::instance().info(
        "gap::connection_parameter_update_request: h: 0x%04x, "
        "interval: (%u, %u), latency: %u, sup_timeout: %u",
        connection_handle,
        connection_parameters.interval_min,
        connection_parameters.interval_max,
        connection_parameters.slave_latency,
        connection_parameters.supervision_timeout);
}

void ble_gap_connection::phy_update_request(
    uint16_t                        connection_handle,
    ble::gap::phy_layer_parameters  phy_tx_preferred,
    ble::gap::phy_layer_parameters  phy_rx_preferred)
{
    logger::instance().info("gap::phy_update_request: h: 0x%04x, "
                            "phy_tx_preferred: %u phy_rx_preferred: %u",
                            connection_handle,
                            phy_tx_preferred, phy_rx_preferred);
}

void ble_gap_connection::phy_update(
    uint16_t                        connection_handle,
    ble::hci::error_code            status,
    ble::gap::phy_layer_parameters  phy_tx,
    ble::gap::phy_layer_parameters  phy_rx)
{
    logger::instance().info("gap::phy_update: h: 0x%04x, status: %u "
                            "phy_tx_preferred: %u phy_rx_preferred: %u",
                            connection_handle, status, phy_tx, phy_rx);
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
    logger::instance().info("gap::link_layer_update: h: 0x%04x, "
                            "tx max: (len: %4u, interval: %6u usec), "
                            "rx max: (len: %4u, interval: %6u usec)",
                            connection_handle,
                            tx_length_max, tx_interval_usec_max,
                            rx_length_max, rx_interval_usec_max);
}

void ble_gap_connection::security_request(
    uint16_t                                            connection_handle,
    bool                                                bonding,
    ble::gap::security::authentication_required const&  auth_req)
{
    logger::instance().info("gap::security_request: h: 0x%04x, bonding: %u "
                            "auth_req: (mitm: %u, lesc: %u, keyp: %u, ct2: %u)",
                            connection_handle, bonding,
                            auth_req.mitm, auth_req.lesc,
                            auth_req.keypress, auth_req.ct2);
}

void ble_gap_connection::security_pairing_request(
    uint16_t                                  connection_handle,
    bool                                      bonding,
    ble::gap::security::pairing_request const &pair_req)
{
    logger::instance().info(
        "gap::security_pairing_request: h: 0x%04x, bonding: %u "
        "pair_req: io: %u, oob: %u, "
        "auth_req: (mitm: %u, lesc: %u, keyp: %u, ct2: %u), "
        "key_size: (%u, %u), ",
        "init_key_dist: (enc: %u, id: %u, sign: %u, link: %u), "
        "resp_key_dist: (enc: %u, id: %u, sign: %u, link: %u)",
        connection_handle, bonding,
        pair_req.io_caps, pair_req.oob,
        pair_req.auth_required.mitm,
        pair_req.auth_required.lesc,
        pair_req.auth_required.keypress,
        pair_req.auth_required.ct2,
        pair_req.encryption_key_size_min,
        pair_req.encryption_key_size_max,
        pair_req.initiator_key_distribution.enc_key,
        pair_req.initiator_key_distribution.id_key,
        pair_req.initiator_key_distribution.sign_key,
        pair_req.initiator_key_distribution.link_key,
        pair_req.responder_key_distribution.enc_key,
        pair_req.responder_key_distribution.id_key,
        pair_req.responder_key_distribution.sign_key,
        pair_req.responder_key_distribution.link_key);
}

void ble_gap_connection::security_authentication_key_request(
    uint16_t    connection_handle,
    uint8_t     key_type)
{
    logger::instance().info(
        "gap::security_authentication_key_request: h: 0x%04x, key_type: %u ",
        connection_handle, key_type);
}

void ble_gap_connection::security_information_request(
    uint16_t                                    connection_handle,
    ble::gap::security::key_distribution const& key_dist,
    ble::gap::security::master_id const&        master_id,
    ble::gap::address const&                    peer_address)
{
    logger::instance().info(
        "gap::security_information_request: h: 0x%04x "
        "key_dist: (enc: %u, id: %u, sign: %u, link: %u), "
        "master_id: (ediv: %u, rand: %02x %02x %02x %02x %02x %02x %02x %02x), "
        "peer_addr: (type: %u, %02x %02x %02x %02x %02x %02x)",
        connection_handle,
        key_dist.enc_key,
        key_dist.id_key,
        key_dist.sign_key,
        key_dist.link_key,
        master_id.ediv,
        master_id.rand[0u], master_id.rand[1u],
        master_id.rand[2u], master_id.rand[3u],
        master_id.rand[4u], master_id.rand[5u],
        master_id.rand[6u], master_id.rand[7u],
        peer_address.type,
        peer_address.octets[0u], peer_address.octets[1u],
        peer_address.octets[2u], peer_address.octets[3u],
        peer_address.octets[4u], peer_address.octets[5u]);
}

void ble_gap_connection::security_passkey_display(
    uint16_t                                connection_handle,
    ble::gap::security::pass_key const&     passkey,
    bool                                    match_request)
{
    logger::instance().info("gap::security_passkey_display: h: 0x%04x, "
                            "passkey: '%c%c%c%c%c%c' "
                            "%02x %02x %02x %02x %02x %02x, "
                            "match_req: %u",
                            connection_handle,
                            passkey[0u], passkey[1u], passkey[2u],
                            passkey[3u], passkey[4u], passkey[5u],
                            passkey[0u], passkey[1u], passkey[2u],
                            passkey[3u], passkey[4u], passkey[5u],
                            match_request);
}

void ble_gap_connection::security_key_pressed(
    uint16_t                            connection_handle,
    ble::gap::security::passkey_event   key_press_event)
{
    logger::instance().info("gap::security_key_pressed: h: 0x%04x event: %u",
                            connection_handle, key_press_event);
}

void ble_gap_connection::security_DH_key_calculation_request(
    uint16_t                                connection_handle,
    ble::gap::security::pubk const&         public_key,
    bool                                    oob_required)
{
    logger::instance().info(
        "gap::security_DH_key_calculation_request: h: 0x%04x, "
        "public_key: %02x %02x %02x %02x %02x %02x %02x %02x, "
        "oob_req: %u",
        connection_handle,
        public_key[0u], public_key[1u], public_key[2u], public_key[3u],
        public_key[4u], public_key[5u], public_key[6u], public_key[7u],
        oob_required);
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
    logger::instance().info(
        "gap::security_authentication_status: h: 0x%04x, is_bonded: %u, "
        "status: %u, error_source: %u, ",
        "security mode levels: (mode 1: %u, mode 2: %u), "
        "key_dist_own:  (enc: %u, id: %u, sign: %u, link: %u), "
        "key_dist_peer: (enc: %u, id: %u, sign: %u, link: %u)",
        connection_handle, is_bonded, pairing_status, error_source,
        sec_mode_1_levels, sec_mode_2_levels,
        kdist_own.enc_key,  kdist_own.id_key,  kdist_own.sign_key,  kdist_own.link_key,
        kdist_peer.enc_key, kdist_peer.id_key, kdist_peer.sign_key, kdist_peer.link_key);
}

void ble_gap_connection::connection_security_update(uint16_t connection_handle,
                                                    uint8_t  security_mode,
                                                    uint8_t  security_level,
                                                    uint8_t  key_size)
{
    logger::instance().info("gap::connection_security_update: h: 0x%04x, "
                            "mode: %u, level: %u, key_size: %u",
                            connection_handle,
                            security_mode, security_level, key_size);
}

void ble_gap_connection::rssi_update(uint16_t connection_handle,
                                     int8_t   rssi_dBm)
{
    logger::instance().info("gap::rssi_update: h: 0x%04x, rssi: %d dBm",
                            connection_handle, rssi_dBm);
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

    // For Nordic, each time a report is issued scanning needs to be
    // stopped and restarted to get another report.
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

/// @todo The following functions should be moved outside of
/// the ble_gap_connection class and into the ble_central_controller class.
void ble_gap_connection::negotiation_complete::notify(enum reason completion_reason)
{
    logger& logger = logger::instance();
    logger.info("BLE GAP negotiation complete, starting service discovery");

    this->ble_gap_connection_->get_connecteable()->service_builder()->discover_services(
        this->ble_gap_connection_->get_connection_handle(),
        this->ble_gap_connection_->get_connecteable()->service_container(),
        ble::att::handle_minimum,
        ble::att::handle_maximum,
        &this->ble_gap_connection_->service_discovery_complete_);
}

void service_discovery_complete::notify(ble::att::error_code error)
{
    logger& logger = logger::instance();
    if (error == ble::att::error_code::success)
    {
        logger.info("--- Service discovery complete ---");
    }
    else
    {
        logger.warn("Service discovery failure: %u", error);
    }
}
