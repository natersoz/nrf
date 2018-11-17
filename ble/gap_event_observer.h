/**
 * @file ble/gap_event_observer.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "ble/address.h"
#include "ble/hci_error_codes.h"
#include "ble/gap_types.h"

#include <cstdint>

namespace ble
{
namespace gap
{

/**
 * @interface ble::gap::event_observer
 * An interface for processing GAP events.
 */
class event_observer
{
public:
    virtual ~event_observer()                         = default;

    event_observer()                                  = default;
    event_observer(event_observer const&)             = delete;
    event_observer(event_observer &&)                 = delete;
    event_observer& operator=(event_observer const&)  = delete;
    event_observer& operator=(event_observer&&)       = delete;

    /**
     * A new connection has been established.
     *
     * @param conection_handle  The new connection handle.
     * @param peer_address      The peer address of the associated device.
     * @param peer_address_id   @todo Nordic Specific: index into the device
     *                          identities list when the peer is using privacy.
     *                          (as set with sd_ble_gap_device_identities_set)
     */
    virtual void connect(
        uint16_t            connection_handle,
        gap::address const& peer_address,
        uint8_t             peer_address_id
        ) {} // BLE_GAP_EVT_CONNECTED

    /**
     * A conection has been dropped.
     *
     * @param connection_handle The connection handle that was disconnected.
     * @param error_code       The HCI error code indicating why the connection
     *                         dropped.
     */
    virtual void disconnect(
        uint16_t            connection_handle,
        hci::error_code     error_code
        ) {} // BLE_GAP_EVT_DISCONNECTED

    /**
     * The connection has timed out.
     *
     * @param connection_handle The peer's connection handle.
     * @param reason See @enum timeout_reason.
     */
    virtual void timeout_expiration(
        uint16_t        connection_handle,
        timeout_reason  reason
        ) {}  // BLE_GAP_EVT_TIMEOUT

    /**
     * A request from the peer for updating the conection parameters.
     *
     * @param connection_parameters The newly requested connection setting.
     */
    virtual void connection_parameter_update(
        uint16_t                        connection_handle,
        connection_parameters const&    connection_parameters
        ) {} // BLE_GAP_EVT_CONN_PARAM_UPDATE

    /**
     * A request from the peer for updating the conection parameters.
     *
     * @param connection_handle The connection handle identifying the peer.
     * @param connection_parameters The newly requested connection setting.
     */
    virtual void connection_parameter_update_request(
        uint16_t                                    connection_handle,
        connection_parameters const&                connection_parameters
        ) {} // BLE_GAP_EVT_CONN_PARAM_UPDATE

    /**
     * The peer is requesting a PHY layer change.
     *
     * @param connection_handle The connection handle identifying the peer.
     * @param phy_tx_preferred The preferred PHY for our Tx link.
     * @param phy_rx_preferred The preferred PHY for out Rx link.
     */
    virtual void phy_update_request(
        uint16_t                        connection_handle,
        phy_layer_parameters            phy_rx_preferred,
        phy_layer_parameters            phy_tx_preferred
        ) {} // BLE_GAP_EVT_PHY_UPDATE_REQUEST

    /**
     * The PHY layer has changed.
     *
     * BLUETOOTH SPECIFICATION Version 5.0 | Vol 6, Part B
     * Section 2.4.2.22 LL_PHY_REQ and LL_PHY_RSP
     *
     * @param connection_handle The connection handle identifying the peer.
     * @param status An HCI error which may indicate why the change occurred.
     * @param phy_rx Our updated PHY Rx link.
     * @param phy_tx Our updated PHY Tx link.
     */
    virtual void phy_update(
        uint16_t                        connection_handle,
        ble::hci::error_code            status,
        phy_layer_parameters            phy_rx,
        phy_layer_parameters            phy_tx
        ) {} // BLE_GAP_EVT_PHY_UPDATE

    /**
     * The peer has requested a link layer update.
     *
     * BLUETOOTH SPECIFICATION Version 5.0 | Vol 6, Part B
     * Section 2.4.2.21 LL_LENGTH_REQ and LL_LENGTH_RS
     *
     * @param connection_handle The connection handle identifying the peer.
     * @param rx_length_max The maximum link layer Rx PDU in bytes.
     * @param rx_interval_usec_max The maximum time required for one
     *                             link later PDU to be received.
     * @param tx_length_max The maximum link layer Tx PDU in bytes.
     * @param tx_interval_usec_max The maximum time required for one
     *                             link later PDU to be sent.
     */
    virtual void link_layer_update_request(
        uint16_t                        connection_handle,
        uint16_t                        rx_length_max,
        uint16_t                        rx_interval_usec_max,
        uint16_t                        tx_length_max,
        uint16_t                        tx_interval_usec_max
        ) {} // BLE_GAP_EVT_DATA_LENGTH_UPDATE_REQUEST

    /**
     * The link layer has been updated.
     *
     * @param connection_handle The connection handle identifying the peer.
     * @param rx_length_max         The maximum link layer Rx PDU in bytes.
     * @param rx_interval_usec_max  The maximum time required for one
     *                              link later PDU to be received.
     * @param tx_length_max         The maximum link layer Tx PDU in bytes.
     * @param tx_interval_usec_max  The maximum time required for one
     *                              link later PDU to be sent.
     */
    virtual void link_layer_update(
        uint16_t                        connection_handle,
        uint16_t                        rx_length_max,
        uint16_t                        rx_interval_usec_max,
        uint16_t                        tx_length_max,
        uint16_t                        tx_interval_usec_max
        ) {} // BLE_GAP_EVT_DATA_LENGTH_UPDATE

    /**
     * The peer wishes to initiate a bond.
     *
     * @param connection_handle The connection handle identifying the peer.
     * @param bonding          @todo Should this always be true?
     * @param auth_req         The peer's authentication parameters required.
     */
    virtual void security_request(
        uint16_t                                    connection_handle,
        bool                                        bonding,
        security::authentication_required const&    auth_req
        ) {} // BLE_GAP_EVT_SEC_REQUEST

    /**
     * See BLUETOOTH SPECIFICATION Version 5.0 | Vol 3, Part H, pages 2340-2342
     * Figure 3.3: Authentication Requirements Flags
     */
    virtual void security_pairing_request(
        uint16_t                            connection_handle,
        bool                                bonding,
        security::pairing_request const&    pair_req
        ) {} // BLE_GAP_EVT_SEC_PARAMS_REQUEST

    /**
     * @param key_type BLE_GAP_AUTH_KEY_TYPES @todo TBD, nordic specific
     * 0: None
     * 1: Pass Key
     * 2: OOB
     */
    virtual void security_authentication_key_request(
        uint16_t                connection_handle,
        uint8_t                 key_type
        ) {} // BLE_GAP_EVT_AUTH_KEY_REQUEST

    /**
     * The peer has requested a secure connection.
     * @see BLUETOOTH SPECIFICATION Version 5.0 | Vol 3, Part H page 2350-2355
     *      Figure 3.11: LE Key Distribution Format
     *
     * @param connection_handle The peer's connection handle.
     *
     * @param enk_key In LE legacy pairing: Indicates that the device shall
     * distribute the LTK using the Encryption Information command followed by
     * EDIV and Rand using the Master Identification command.
     * In LE Secure Connections: ignored. EDIV and Rand shall be set to zero.
     *
     * @param id_key Indicates that the device shall distribute IRK using the
     * Identity Information command followed by its public device or
     * static random address using Identity Address Information.
     *
     * @param sign_key Indicates that the device shall distribute CSRK using
     * the Signing Information command.
     *
     * @param peer_address The peer's address.
     */
    virtual void security_information_request(
        uint16_t                            connection_handle,
        security::key_distribution const&   key_dist,
        security::master_id const&          mater_id,
        gap::address const&                 peer_address
        ) {} // BLE_GAP_EVT_SEC_INFO_REQUEST

    /**
     * Request to display a passkey to the user.
     *
     * @param passkey A six digit ASCII passkey value ['0':'9'].
     * @param match_request Requires a response to determine if the
     *                      passkey matched.
     */
    virtual void security_passkey_display(
        uint16_t                          connection_handle,
        security::pass_key const&         passkey,
        bool                              match_request
        ) {} // BLE_GAP_EVT_PASSKEY_DISPLAY

    /**
     * @see BLUETOOTH SPECIFICATION Version 5.0 | Vol 3, Part H page 2349
     *      Table 3.8: Notification Type
     * @param key_press_event key_press
     */
    virtual void security_key_pressed(
        uint16_t                connection_handle,
        security::passkey_event key_press_event
        ) {} // BLE_GAP_EVT_KEY_PRESSED

    /**
     * Perform the Diffie-Hellman key calculation given the peer's public key.
     *
     * @param connection_handle The connection handle identifying the peer.
     * @param public_key The peer's public key.
     * @param oob_required Indicates that OOB authentication is required.
     */
    virtual void security_DH_key_calculation_request(
        uint16_t                connection_handle,
        security::pubk const&   public_key,
        bool                    oob_required
        ) {} // BLE_GAP_EVT_LESC_DHKEY_REQUEST

    /**
     * During the pairing key exchange this function will get called multiple
     * times indicating the state of the pairing process.
     *
     * @param connection_handle  The connection handle identifying the peer.
     * @param pairing_status    The state of the pairing transaction.
     * @param error_source      If an error occurs the source of the error.
     *                          0: A local failure. 1: A remote failure.
     * @param is_bonded         If true, bonding was successful.
     * @param sec_mode_1_levels Security mode 1 levels supported.
     * @param sec_mode_2_levels Security mode 2 levels supported.
     * @param kdist_own         Local key distribution.
     * @param kdist_peer        Peer  key distribution.
     *
     * Security Mode 0 Level 0: No access permissions at all
     *          (this level is not defined by the Bluetooth Core specification).
     * Security Mode 1 Level 1: No security is needed (aka open link).
     * Security Mode 1 Level 2: Encrypted link required, no MITM protection.
     * Security Mode 1 Level 3: MITM protected encrypted link required.
     * Security Mode 1 Level 4: LESC MITM protected encrypted link using a
     *                          128-bit strength encryption key required.
     *
     * Security Mode 2 Level 1: Signing or encryption required,
     *                          MITM protection not necessary.
     * Security Mode 2 Level 2: MITM protected signing required,
     *                          unless link is MITM protected encrypted.
     */
    virtual void security_authentication_status(
        uint16_t                            connection_handle,
        ble::gap::security::pairing_failure pairing_status,
        uint8_t                             error_source,
        bool                                is_bonded,
        uint8_t                             sec_mode_1_levels,
        uint8_t                             sec_mode_2_levels,
        security::key_distribution const&   kdist_own,
        security::key_distribution const&   kdist_peer
        ) {} // BLE_GAP_EVT_AUTH_STATUS

    /**
     * @param security_mode  The security mode updated.
     * @param security_level The security level updated.
     * @see security_authentication_status for modes/levels explanation.
     *
     * @param key_size Length of currently active encryption key,
     *                 7 to 16 octets (only applicable for bonding procedures).
     */
    virtual void connection_security_update(
        uint16_t        connection_handle,
        uint8_t         security_mode,
        uint8_t         security_level,
        uint8_t         key_size
        ) {} // BLE_GAP_EVT_CONN_SEC_UPDATE

    /**
     * The RSSI value from the peer has been updated.
     *
     * @param connection_handle The peer's connection handle.
     * @param rssi_dBm The RSSI value in dBm.
     */
    virtual void rssi_update(
        uint16_t                        connection_handle,
        int8_t                          rssi_dBm
        ) {} // BLE_GAP_EVT_RSSI_CHANGED

    /**
     * When in the central role, report that advertising data has been received.
     *
     * @param connection_handle The connection handle identifying the peer.
     * @param peer_address     The peer's address if it can be resolved.
     * @param direct_address   When the peer address is unresolvable.
     * @param rssi_dBm         The RSSI value in dBm.
     * @param scan_response    true if the data payload is the result of a
     *                         scan/response. false if it is advertising data.
     * @param data             The advertising or scan/response data pointer.
     * @param data_length      The advertising or scan/response data length
     *                         in bytes.
     */
    virtual void advertising_report(
        uint16_t                        connection_handle,
        gap::address const&             peer_address,
        gap::address const&             direct_address,
        int8_t                          rssi_dBm,
        bool                            scan_response,
        void const*                     data,
        uint8_t                         data_length
        ) {} // BLE_GAP_EVT_ADV_REPORT

    /**
     * The peer is requesting a signal strength report.
     *
     * @param connection_handle The connection handle identifying the peer.
     * @param peer_address The peer's address.
     * @param rssi_dBm The peer's RSSI in dBm.
     */
    virtual void scan_report_request(
        uint16_t                                    connection_handle,
        gap::address const&                         peer_address,
        int8_t                                      rssi_dBm
        ) {} // BLE_GAP_EVT_SCAN_REQ_REPORT
};

} // namespace gap
} // namespace ble
