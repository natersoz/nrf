/**
 * @file nordic_ble_gap_event_observable.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Provide attachment and notifications for the ble::gap::event_observer class
 * into the Nordic BLE observables.
 */

#include "ble/nordic_ble_event_observable.h"
#include "ble/nordic_ble_event_observer.h"

#include "ble_gap.h"                    // Nordic softdevice header
#include "make_array.h"
#include "logger.h"
#include <cstring>

namespace nordic
{

/**
 * Convert Nordic BLE_GAP_SEC_STATUS GAP Security status values to
 * ble::gap::pairing_failure enum values.
 */
static ble::gap::security::pairing_failure auth_status_to_pairing_failure(uint8_t auth_status)
{
    if (auth_status > BLE_GAP_SEC_STATUS_RFU_RANGE1_END)
    {
        uint8_t pairing_fail = auth_status - BLE_GAP_SEC_STATUS_RFU_RANGE1_END;
        return static_cast<ble::gap::security::pairing_failure>(pairing_fail);
    }

    switch (auth_status)
    {
    case BLE_GAP_SEC_STATUS_TIMEOUT:
        return ble::gap::security::pairing_failure::timeout;
    case BLE_GAP_SEC_STATUS_PDU_INVALID:
        return ble::gap::security::pairing_failure::pdu_invalid;
    default:
        break;
    }

    logger::instance().warn("unhandled nordic::auth_status_to_pairing_failure(%u)", auth_status);
    return ble::gap::security::pairing_failure::failure_unknown;
}

template<>
void ble_event_observable<ble_gap_event_observer>::notify(
    ble_gap_event_observer::event_enum_t event_type,
    ble_gap_event_observer::event_data_t const& event_data)
{
    logger &logger = logger::instance();

    for (auto observer_iter  = this->observer_list_.begin();
              observer_iter != this->observer_list_.end(); )
    {
        // Increment the iterator prior to using it.
        // If the client removes itself during the completion callback
        // then the iterator will be valid and can continue.
        ble_gap_event_observer &observer = *observer_iter;
        ++observer_iter;

        switch (event_type)
        {
        case BLE_GAP_EVT_CONNECTED:
            {
                logger::instance().debug(
                    "GAP connect: h: 0x%04x, peer: %02x:%02x:%02x:%02x:%02x:%02x, type: %u, id: %u, role: %u",
                    event_data.conn_handle,
                    event_data.params.connected.peer_addr.addr[0],
                    event_data.params.connected.peer_addr.addr[1],
                    event_data.params.connected.peer_addr.addr[2],
                    event_data.params.connected.peer_addr.addr[3],
                    event_data.params.connected.peer_addr.addr[4],
                    event_data.params.connected.peer_addr.addr[5],
                    event_data.params.connected.peer_addr.addr_type,
                    event_data.params.connected.peer_addr.addr_id_peer,
                    event_data.params.connected.role
                    );

                ble::gap::address peer_address(
                    event_data.params.connected.peer_addr.addr,
                    event_data.params.connected.peer_addr.addr_type);

                observer.interface_reference.connect(
                    event_data.conn_handle,
                    peer_address,
                    event_data.params.connected.peer_addr.addr_id_peer);
            }
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            {
                logger::instance().debug(
                    "GAP disconnect: h: 0x%04x, hci error: 0x%02x",
                    event_data.conn_handle, event_data.params.disconnected.reason);

                observer.interface_reference.disconnect(
                    event_data.conn_handle,
                    static_cast<ble::hci::error_code>(event_data.params.disconnected.reason));
            }
            break;

        // The connection interval, slave latency, supervision timeout have been updated.
        case BLE_GAP_EVT_CONN_PARAM_UPDATE:
            {
                ble::gap::connection_parameters const conn_params = {
                    event_data.params.conn_param_update.conn_params.min_conn_interval,
                    event_data.params.conn_param_update.conn_params.max_conn_interval,
                    event_data.params.conn_param_update.conn_params.slave_latency,
                    event_data.params.conn_param_update.conn_params.conn_sup_timeout
                };

                logger::instance().debug(
                    "GAP connection params update: h: 0x%04x, interval: (%u, %u), latency: %u, sup_timeout: %u",
                    event_data.conn_handle,
                    conn_params.interval_min, conn_params.interval_max,
                    conn_params.slave_latency, conn_params.supervision_timeout);

                observer.interface_reference.connection_parameter_update(
                    event_data.conn_handle,
                    conn_params);
            }
            break;

        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
            /// @todo Nordic secific: Reply with sd_ble_gap_sec_params_reply.
            /// Receive ble_gap_evt_sec_params_request_t
            {
                ble_gap_sec_params_t const &sec_params = event_data.params.sec_params_request.peer_params;
                ble::gap::security::pairing_request const pairing_request = {
                    .io_caps = static_cast<ble::gap::security::io_capabilities>(sec_params.io_caps),
                    .oob     = static_cast<ble::gap::security::oob_data_flags>(sec_params.oob),
                    .auth_required = {
                        .mitm       = sec_params.mitm,
                        .lesc       = sec_params.lesc,
                        .keypress   = sec_params.keypress,
                        .ct2        = false   /// @todo ??
                    },

                    .encryption_key_size_max = sec_params.max_key_size,
                    // The peer is making the request,
                    // therefore the peer is the initiator.
                    .initiator_key_distribution = {
                        .enc_key    = sec_params.kdist_peer.enc,
                        .id_key     = sec_params.kdist_peer.id,
                        .sign_key   = sec_params.kdist_peer.sign,
                        .link_key   = sec_params.kdist_peer.link
                    },
                    .responder_key_distribution = {
                        .enc_key    = sec_params.kdist_own.enc,
                        .id_key     = sec_params.kdist_own.id,
                        .sign_key   = sec_params.kdist_own.sign,
                        .link_key   = sec_params.kdist_own.link
                    },
                };

                logger::instance().debug("GAP secutiry pairing request: h: 0x%04x", event_data.conn_handle);
                logger::instance().debug("io_caps: 0x%04x, oob: %u", pairing_request.io_caps, pairing_request.oob);
                logger::instance().debug("auth_req: mitm: %u, lesc: %u, keypress: %u, ct2: %u",
                                         pairing_request.auth_required.mitm,
                                         pairing_request.auth_required.lesc,
                                         pairing_request.auth_required.keypress,
                                         pairing_request.auth_required.ct2);
                logger::instance().debug("key dist init: enc: %u, id: %u, sign: %u, link: %u",
                                         pairing_request.initiator_key_distribution.enc_key,
                                         pairing_request.initiator_key_distribution.id_key,
                                         pairing_request.initiator_key_distribution.sign_key,
                                         pairing_request.initiator_key_distribution.link_key);
                logger::instance().debug("key dist resp: enc: %u, id: %u, sign: %u, link: %u",
                                         pairing_request.initiator_key_distribution.enc_key,
                                         pairing_request.initiator_key_distribution.id_key,
                                         pairing_request.initiator_key_distribution.sign_key,
                                         pairing_request.initiator_key_distribution.link_key);

                observer.interface_reference.security_pairing_request(
                    event_data.conn_handle,
                    bool(sec_params.bond),
                    pairing_request);
            }
            break;

        case BLE_GAP_EVT_SEC_INFO_REQUEST:
            /// Receive ble_gap_evt_sec_info_request_t.
            /// @todo Nordic specific: Reply with sd_ble_gap_sec_info_reply.
            {
                ble_gap_evt_sec_info_request_t const& info_request = event_data.params.sec_info_request;

                ble::gap::security::key_distribution const key_dist = {
                    .enc_key  = bool(info_request.enc_info),
                    .id_key   = bool(info_request.id_info),
                    .sign_key = bool(info_request.sign_info),
                    .link_key = false
                };

                ble::gap::security::master_id const master_id = {
                    .ediv = info_request.master_id.ediv,
                    .rand = utility::to_array(info_request.master_id.rand)
                };

                ble::gap::address const peer_address(info_request.peer_addr.addr,
                                                     info_request.peer_addr.addr_type);

                logger::instance().debug("GAP secutiry info request: h: 0x%04x", event_data.conn_handle);
                logger::instance().debug("key dist: enc: %u, id: %u, sign: %u, link: %u",
                                         key_dist.enc_key, key_dist.id_key, key_dist.sign_key, key_dist.link_key);

                observer.interface_reference.security_information_request(
                    event_data.conn_handle,
                    key_dist,
                    master_id,
                    peer_address);
            }
            break;

        case BLE_GAP_EVT_PASSKEY_DISPLAY:
            /// Receive ble_gap_evt_passkey_display_t
            /// @todo Nordic specific: reply with sd_ble_gap_auth_key_reply
            {
                ble::gap::security::pass_key const pass_key =
                    utility::to_array(event_data.params.passkey_display.passkey);

                logger::instance().debug("GAP passkey display: h: 0x%04x, '%c%c%c%c%c%c'",
                                         event_data.conn_handle,
                                         pass_key[0], pass_key[1], pass_key[2],
                                         pass_key[3], pass_key[4], pass_key[5]);

                observer.interface_reference.security_passkey_display(
                    event_data.conn_handle,
                    pass_key,
                    bool(event_data.params.passkey_display.match_request));
            }
            break;

        case BLE_GAP_EVT_KEY_PRESSED:
            {
                logger::instance().debug("GAP key press event: h: 0x%04x, %u",
                                         event_data.conn_handle,
                                         event_data.params.key_pressed.kp_not);

                // Since Nordic BLE_GAP_KP_NOT_TYPES also take their values from the
                // Core BT specification we can just cast to the passkey_event enum type.
                observer.interface_reference.security_key_pressed(
                    event_data.conn_handle,
                    static_cast<ble::gap::security::passkey_event>(event_data.params.key_pressed.kp_not));
            }
            break;

        case BLE_GAP_EVT_AUTH_KEY_REQUEST:
            {
                logger::instance().debug("GAP auth key request: h: 0x%04x, %u",
                                         event_data.conn_handle,
                                         event_data.params.auth_key_request.key_type);

                /// @todo Nordic Specific: What part of the BT Core does this map to?
                /// Investigate how this works in practice and understand it.
                /// For now just pass through.
                observer.interface_reference.security_authentication_key_request(
                    event_data.conn_handle,
                    event_data.params.auth_key_request.key_type);
            }
            break;

        case BLE_GAP_EVT_LESC_DHKEY_REQUEST:
            {
                /// The peer has requested a Diffie-Hellman key calculation.
                /// @todo Nordic Specific: Reply with sd_ble_gap_lesc_dhkey_reply.
                ble::gap::security::pubk const public_key = utility::to_array(
                    event_data.params.lesc_dhkey_request.p_pk_peer->pk);

                logger::instance().debug("GAP DH key request: h: 0x%04x", event_data.conn_handle);
                logger::instance().write_data(logger::level::debug,
                                              event_data.params.lesc_dhkey_request.p_pk_peer->pk,
                                              sizeof(event_data.params.lesc_dhkey_request.p_pk_peer->pk));

                observer.interface_reference.security_DH_key_calculation_request(
                    event_data.conn_handle,
                    public_key,
                    bool(event_data.params.lesc_dhkey_request.oobd_req));
            }
            break;

        case BLE_GAP_EVT_AUTH_STATUS:
            {
                /// @todo Nordic Specific: Determine how this maps to Core spec.
                ble_gap_evt_auth_status_t const &auth_status = event_data.params.auth_status;

                ble::gap::security::pairing_failure const pairing_status =
                    auth_status_to_pairing_failure(auth_status.auth_status);

                uint8_t const sec_mode_1_levels =
                    (auth_status.sm1_levels.lv1 ? 1u : 0u) |
                    (auth_status.sm1_levels.lv2 ? 2u : 0u) |
                    (auth_status.sm1_levels.lv3 ? 4u : 0u) |
                    (auth_status.sm1_levels.lv4 ? 8u : 0u) ;

                uint8_t const sec_mode_2_levels =
                    (auth_status.sm2_levels.lv1 ? 1u : 0u) |
                    (auth_status.sm2_levels.lv2 ? 2u : 0u) |
                    (auth_status.sm2_levels.lv3 ? 4u : 0u) |
                    (auth_status.sm2_levels.lv4 ? 8u : 0u) ;

                ble::gap::security::key_distribution const kdist_own = {
                    .enc_key  = bool(auth_status.kdist_own.enc),
                    .id_key   = bool(auth_status.kdist_own.id),
                    .sign_key = bool(auth_status.kdist_own.sign),
                    .link_key = bool(auth_status.kdist_own.link)
                };

                ble::gap::security::key_distribution const kdist_peer = {
                    .enc_key  = bool(auth_status.kdist_peer.enc),
                    .id_key   = bool(auth_status.kdist_peer.id),
                    .sign_key = bool(auth_status.kdist_peer.sign),
                    .link_key = bool(auth_status.kdist_peer.link)
                };

                logger::instance().debug("GAP auth status: h: 0x%04x, sm_1: %u, sm_2: %u, status: %u",
                                         event_data.conn_handle, sec_mode_1_levels, sec_mode_2_levels, pairing_status);
                logger::instance().debug("key dist own : enc: %u, id: %u, sign: %u, link: %u",
                                         kdist_own.enc_key, kdist_own.id_key, kdist_own.sign_key, kdist_own.link_key);
                logger::instance().debug("key dist peer: enc: %u, id: %u, sign: %u, link: %u",
                                         kdist_peer.enc_key, kdist_peer.id_key, kdist_peer.sign_key, kdist_peer.link_key);

                observer.interface_reference.security_authentication_status(
                    event_data.conn_handle,
                    pairing_status,
                    auth_status.error_src,
                    bool(auth_status.bonded),
                    sec_mode_1_levels,
                    sec_mode_2_levels,
                    kdist_own,
                    kdist_peer);
            }
            break;

        case BLE_GAP_EVT_CONN_SEC_UPDATE:
            {
                logger::instance().debug("GAP security update: h: 0x%04x, mode: %u, level: %u, key size: %u",
                                         event_data.conn_handle,
                                         event_data.params.conn_sec_update.conn_sec.sec_mode.sm,
                                         event_data.params.conn_sec_update.conn_sec.sec_mode.lv,
                                         event_data.params.conn_sec_update.conn_sec.encr_key_size);

                observer.interface_reference.connection_security_update(
                    event_data.conn_handle,
                    event_data.params.conn_sec_update.conn_sec.sec_mode.sm,
                    event_data.params.conn_sec_update.conn_sec.sec_mode.lv,
                    event_data.params.conn_sec_update.conn_sec.encr_key_size);
            }
            break;

        case BLE_GAP_EVT_TIMEOUT:
            {
                logger::instance().debug("GAP timeout: h: 0x%04x, reason: %u",
                                         event_data.conn_handle, event_data.params.timeout.src);

                observer.interface_reference.timeout_expiration(
                    event_data.conn_handle,
                    static_cast<ble::gap::timeout_reason>(event_data.params.timeout.src));
            }
            break;

        case BLE_GAP_EVT_RSSI_CHANGED:
            {
                logger::instance().debug("GAP rssi changed: h: 0x%04x, rssi: %d",
                                         event_data.conn_handle, event_data.params.rssi_changed.rssi);

                observer.interface_reference.rssi_update(
                    event_data.conn_handle,
                    event_data.params.rssi_changed.rssi);
            }
            break;

        case BLE_GAP_EVT_ADV_REPORT:
            {
                ble::gap::address const peer_address(
                    event_data.params.adv_report.peer_addr.addr,
                    event_data.params.adv_report.peer_addr.addr_type);

                ble::gap::address const direct_address(
                    event_data.params.adv_report.direct_addr.addr,
                    event_data.params.adv_report.direct_addr.addr_type);

                logger::instance().debug("GAP advert report: h: 0x%04x, rssi: %d",
                                         event_data.conn_handle, event_data.params.adv_report.rssi);

                observer.interface_reference.advertising_report(
                    event_data.conn_handle,
                    peer_address,
                    direct_address,
                    event_data.params.adv_report.rssi,
                    event_data.params.adv_report.scan_rsp,
                    event_data.params.adv_report.data,
                    event_data.params.adv_report.dlen);
            }
            break;

        case BLE_GAP_EVT_SEC_REQUEST:
            {
                ble::gap::security::authentication_required const auth_req = {
                    .mitm       = bool(event_data.params.sec_request.mitm),
                    .lesc       = bool(event_data.params.sec_request.lesc),
                    .keypress   = bool(event_data.params.sec_request.keypress),
                    .ct2        = false     /// @todo check this value.
                };

                logger::instance().debug("GAP secutiry request: h: 0x%04x", event_data.conn_handle);
                logger::instance().debug("auth_req: mitm: %u, lesc: %u, keypress: %u, ct2: %u",
                                         auth_req.mitm, auth_req.lesc, auth_req.keypress, auth_req.ct2);

                observer.interface_reference.security_request(
                    event_data.conn_handle,
                    bool(event_data.params.sec_request.bond),
                    auth_req);
            }
            break;

        case BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST:
            {
                /// @todo Nordic Specific: Reply with sd_ble_gap_conn_param_update.
                ble::gap::connection_parameters const conn_params = {
                    event_data.params.conn_param_update_request.conn_params.min_conn_interval,
                    event_data.params.conn_param_update_request.conn_params.max_conn_interval,
                    event_data.params.conn_param_update_request.conn_params.slave_latency,
                    event_data.params.conn_param_update_request.conn_params.conn_sup_timeout
                };

                logger::instance().debug(
                    "GAP connection params update request: h: 0x%04x, interval: (%u, %u), latency: %u, sup_timeout: %u",
                    event_data.conn_handle,
                    conn_params.interval_min, conn_params.interval_max,
                    conn_params.slave_latency, conn_params.supervision_timeout);

                observer.interface_reference.connection_parameter_update_request(
                    event_data.conn_handle,
                    conn_params);
            }
            break;

        case BLE_GAP_EVT_SCAN_REQ_REPORT:
            {
                ble::gap::address const peer_address(
                    event_data.params.scan_req_report.peer_addr.addr,
                    event_data.params.scan_req_report.peer_addr.addr_type);

                logger::instance().debug(
                    "GAP scan request report: h: 0x%04x, peer: %02x:%02x:%02x:%02x:%02x:%02x, type: %u, id: %u",
                    event_data.conn_handle,
                    event_data.params.connected.peer_addr.addr[0],
                    event_data.params.connected.peer_addr.addr[1],
                    event_data.params.connected.peer_addr.addr[2],
                    event_data.params.connected.peer_addr.addr[3],
                    event_data.params.connected.peer_addr.addr[4],
                    event_data.params.connected.peer_addr.addr[5],
                    event_data.params.connected.peer_addr.addr_type,
                    event_data.params.connected.peer_addr.addr_id_peer
                    );

                observer.interface_reference.scan_report_request(
                    event_data.conn_handle,
                    peer_address,
                    event_data.params.scan_req_report.rssi);
            }
            break;

        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
            /// @todo Nordic Specific: Reply with sd_ble_gap_phy_update.
            {
                logger::instance().debug(
                    "GAP phy update request: h: 0x%04x, rx: %u, tx: %u",
                    event_data.conn_handle,
                    event_data.params.phy_update_request.peer_preferred_phys.rx_phys,
                    event_data.params.phy_update_request.peer_preferred_phys.tx_phys);

                observer.interface_reference.phy_update_request(
                    event_data.conn_handle,
                    static_cast<ble::gap::phy_layer_parameters>(event_data.params.phy_update_request.peer_preferred_phys.rx_phys),
                    static_cast<ble::gap::phy_layer_parameters>(event_data.params.phy_update_request.peer_preferred_phys.tx_phys));
            }
            break;

        case BLE_GAP_EVT_PHY_UPDATE:
            {
                logger::instance().debug(
                    "GAP phy update request: h: 0x%04x, rx: %u, tx: %u",
                    event_data.conn_handle,
                    event_data.params.phy_update.rx_phy, event_data.params.phy_update.tx_phy);

                observer.interface_reference.phy_update(
                    event_data.conn_handle,
                    static_cast<ble::hci::error_code>(event_data.params.phy_update.status),
                    static_cast<ble::gap::phy_layer_parameters>(event_data.params.phy_update.rx_phy),
                    static_cast<ble::gap::phy_layer_parameters>(event_data.params.phy_update.tx_phy));
            }
            break;

        case BLE_GAP_EVT_DATA_LENGTH_UPDATE_REQUEST:
            /// @todo Nordic specific: reply with sd_ble_gap_data_length_update
            {
                logger::instance().debug(
                    "GAP phy update request: h: 0x%04x, rx: (%u, %u), tx: (%u, %u)",
                    event_data.conn_handle,
                    event_data.params.data_length_update_request.peer_params.max_rx_octets,
                    event_data.params.data_length_update_request.peer_params.max_rx_time_us,
                    event_data.params.data_length_update_request.peer_params.max_tx_octets,
                    event_data.params.data_length_update_request.peer_params.max_tx_time_us);

                observer.interface_reference.link_layer_update_request(
                    event_data.conn_handle,
                    event_data.params.data_length_update_request.peer_params.max_rx_octets,
                    event_data.params.data_length_update_request.peer_params.max_rx_time_us,
                    event_data.params.data_length_update_request.peer_params.max_tx_octets,
                    event_data.params.data_length_update_request.peer_params.max_tx_time_us);
            }
            break;

        case BLE_GAP_EVT_DATA_LENGTH_UPDATE:
            {
                logger::instance().debug(
                    "GAP phy update request: h: 0x%04x, rx: (%u, %u), tx: (%u, %u)",
                    event_data.conn_handle,
                    event_data.params.data_length_update_request.peer_params.max_rx_octets,
                    event_data.params.data_length_update_request.peer_params.max_rx_time_us,
                    event_data.params.data_length_update_request.peer_params.max_tx_octets,
                    event_data.params.data_length_update_request.peer_params.max_tx_time_us);

                observer.interface_reference.link_layer_update(
                    event_data.conn_handle,
                    event_data.params.data_length_update.effective_params.max_rx_octets,
                    event_data.params.data_length_update.effective_params.max_rx_time_us,
                    event_data.params.data_length_update.effective_params.max_tx_octets,
                    event_data.params.data_length_update.effective_params.max_tx_time_us);
            }
            break;

        default:
            logger.warn("unhandled GAP event: %u", event_type);
            break;
        }
    }
}

} // namespace nordic
