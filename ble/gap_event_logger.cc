/**
 * @file ble/gap_event_logger.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "ble/gap_event_logger.h"
#include "utility/array_u8_to_chars.h"

namespace ble
{
namespace gap
{

static constexpr size_t const address_conv_length =
    utility::array_uint8_conversion_size<gap::address::octet_length>;

void event_logger::connect(uint16_t connection_handle,
                           gap::address const& peer_address,
                           uint8_t peer_address_id)
{
    char peer_buffer[address_conv_length];

    utility::to_chars(peer_address.octets,
                      std::begin(peer_buffer), std::end(peer_buffer));

    logger::instance().write(this->log_level,
                             "gap::connect: h: 0x%04x, peer:(%s, id: %u)",
                             connection_handle, peer_buffer, peer_address_id);
}

void event_logger::disconnect(uint16_t          connection_handle,
                              hci::error_code   error_code)
{
    logger::instance().write(this->log_level,
                             "gap::disconnect: 0x%04x, reason: 0x%02x",
                             connection_handle, error_code);
}

void event_logger::timeout_expiration(uint16_t          connection_handle,
                                      timeout_reason    reason)
{
    logger::instance().write(this->log_level,
                             "gap::timeout_expiration: 0x%04x, reason: 0x%02x",
                             connection_handle, reason);
}

void event_logger::connection_parameter_update(
    uint16_t                        connection_handle,
    connection_parameters const&    connection_parameters)
{
    logger::instance().write(this->log_level,
                             "gap::connection_parameter_update: h: 0x%04x, "
                             "interval: (%u, %u), latency: %u, sup_timeout: %u",
                             connection_handle,
                             connection_parameters.interval_min,
                             connection_parameters.interval_max,
                             connection_parameters.slave_latency,
                             connection_parameters.supervision_timeout);
}

void event_logger::connection_parameter_update_request(
    uint16_t                        connection_handle,
    connection_parameters const&    connection_parameters)
{
    logger::instance().write(this->log_level,
                             "gap::connection_parameter_update_request: h: 0x%04x, "
                             "interval: (%u, %u), latency: %u, sup_timeout: %u",
                             connection_handle,
                             connection_parameters.interval_min,
                             connection_parameters.interval_max,
                             connection_parameters.slave_latency,
                             connection_parameters.supervision_timeout);
}

void event_logger::phy_update_request(uint16_t              connection_handle,
                                      phy_layer_parameters  phy_rx_preferred,
                                      phy_layer_parameters  phy_tx_preferred)
{
    logger::instance().write(this->log_level,
                             "gap::phy_update_request: h: 0x%04x, "
                             "phy_tx_preferred: %u phy_rx_preferred: %u",
                             connection_handle,
                             phy_tx_preferred, phy_rx_preferred);
}

void event_logger::phy_update(uint16_t              connection_handle,
                              ble::hci::error_code  status,
                              phy_layer_parameters  phy_rx,
                              phy_layer_parameters  phy_tx)
{
    logger::instance().write(this->log_level,
                             "gap::phy_update: h: 0x%04x, status: %u, "
                             "phy_tx_preferred: %u, phy_rx_preferred: %u",
                             connection_handle, status, phy_tx, phy_rx);
}

void event_logger::link_layer_update_request(uint16_t connection_handle,
                                             uint16_t rx_length_max,
                                             uint16_t rx_interval_usec_max,
                                             uint16_t tx_length_max,
                                             uint16_t tx_interval_usec_max)
{
    logger::instance().write(this->log_level,
                             "gap::link_layer_update_request: h: 0x%04x, "
                             "tx max: (len: %4u, interval: %6u usec), "
                             "rx max: (len: %4u, interval: %6u usec)",
                             connection_handle,
                             tx_length_max, tx_interval_usec_max,
                             rx_length_max, rx_interval_usec_max);
}

void event_logger::link_layer_update(uint16_t connection_handle,
                                     uint16_t rx_length_max,
                                     uint16_t rx_interval_usec_max,
                                     uint16_t tx_length_max,
                                     uint16_t tx_interval_usec_max)
{
    logger::instance().write(this->log_level,
                             "gap::link_layer_update: h: 0x%04x, "
                             "tx max: (len: %4u, interval: %6u usec), "
                             "rx max: (len: %4u, interval: %6u usec)",
                             connection_handle,
                             tx_length_max, tx_interval_usec_max,
                             rx_length_max, rx_interval_usec_max);
}

void event_logger::security_request(
    uint16_t                                    connection_handle,
    bool                                        bonding,
    security::authentication_required const&    auth_req)
{
    logger::instance().write(this->log_level,
                             "gap::security_request: h: 0x%04x, bonding: %u, "
                             "auth_req: (mitm: %u, lesc: %u, keyp: %u, ct2: %u)",
                             connection_handle, bonding,
                             auth_req.mitm, auth_req.lesc,
                             auth_req.keypress, auth_req.ct2);
}

void event_logger::security_pairing_request(
    uint16_t                            connection_handle,
    bool                                bonding,
    security::pairing_request const&    pair_req)
{
    logger::instance().write(this->log_level,
        "gap::security_pairing_request: h: 0x%04x, bonding: %u, "
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

void event_logger::security_authentication_key_request(
    uint16_t    connection_handle,
    uint8_t     key_type)
{
    logger::instance().write(this->log_level,
                             "gap::security_authentication_key_request: "
                             "h: 0x%04x, key_type: %u",
                             connection_handle, key_type);
}

void event_logger::security_information_request(
    uint16_t                            connection_handle,
    security::key_distribution const&   key_dist,
    security::master_id const&          master_id,
    gap::address const&                 peer_address)
{
    logger::instance().write(this->log_level,
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

void event_logger::security_passkey_display(
    uint16_t                    connection_handle,
    security::pass_key const&   passkey,
    bool                        match_request)
{
    logger::instance().write(this->log_level,"gap::security_passkey_display: h: 0x%04x, "
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

void event_logger::security_key_pressed(
    uint16_t                connection_handle,
    security::passkey_event key_press_event)
{
}

void event_logger::security_DH_key_calculation_request(
    uint16_t                connection_handle,
    security::pubk const&   public_key,
    bool                    oob_required)
{
    logger::instance().write(this->log_level,
        "gap::security_DH_key_calculation_request: h: 0x%04x, "
        "public_key: %02x %02x %02x %02x %02x %02x %02x %02x, "
        "oob_req: %u",
        connection_handle,
        public_key[0u], public_key[1u], public_key[2u], public_key[3u],
        public_key[4u], public_key[5u], public_key[6u], public_key[7u],
        oob_required);
}

void event_logger::security_authentication_status(
    uint16_t                            connection_handle,
    ble::gap::security::pairing_failure pairing_status,
    uint8_t                             error_source,
    bool                                is_bonded,
    uint8_t                             sec_mode_1_levels,
    uint8_t                             sec_mode_2_levels,
    security::key_distribution const&   kdist_own,
    security::key_distribution const&   kdist_peer)
{
    logger::instance().write(this->log_level,
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

void event_logger::connection_security_update(
    uint16_t    connection_handle,
    uint8_t     security_mode,
    uint8_t     security_level,
    uint8_t     key_size)
{
    logger::instance().write(this->log_level,"gap::connection_security_update: h: 0x%04x, "
                            "mode: %u, level: %u, key_size: %u",
                            connection_handle,
                            security_mode, security_level, key_size);
}

void event_logger::rssi_update(uint16_t connection_handle,
                               int8_t   rssi_dBm)
{
    logger::instance().write(this->log_level,"gap::rssi_update: h: 0x%04x, rssi: %d dBm",
                            connection_handle, rssi_dBm);
}

void event_logger::advertising_report(uint16_t              connection_handle,
                                      gap::address const&   peer_address,
                                      gap::address const&   direct_address,
                                      int8_t                rssi_dBm,
                                      bool                  scan_response,
                                      void const*           data,
                                      uint8_t               data_length)
{
    char peer_buffer[address_conv_length];
    char direct_buffer[address_conv_length];

    utility::to_chars(peer_address.octets,
                      std::begin(peer_buffer), std::end(peer_buffer));
    utility::to_chars(direct_address.octets,
                      std::begin(direct_buffer), std::end(direct_buffer));

    logger::instance().write(this->log_level,
        "gap::advertising_report: h: 0x%04x, "
        "peer: %s, direct: %s, rssi: %d, scan_resp: %u",
        connection_handle,
        peer_buffer, direct_buffer, rssi_dBm, scan_response);
}

void event_logger::scan_report_request(uint16_t             connection_handle,
                                       gap::address const&  peer_address,
                                       int8_t               rssi_dBm)
{
}

} // namespace gap
} // namespace ble

