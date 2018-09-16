/**
 * @file ble_peripheral_class/ble_gap_connection.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "ble/gap_connection.h"
#include "ble/nordic_ble_event_observable.h"

/**
 * An implementation of the interface ble::gap::event_observer specific
 * to the nrf/ble_peripheral_class.
 */
class ble_gap_connection: public ble::gap::connection
{
private:
    using super = ble::gap::connection;

public:
    virtual ~ble_gap_connection() override;

    /// inherit base class ctors.
    using super::super;

    /** Constructor which uses the default connection parameters. */
    ble_gap_connection();

    /** Constructor which specifies the connection parameters. */
    ble_gap_connection(ble::gap::connection_parameters const& connect_params);

protected:
    /**
     * A new connection has been established.
     *
     * @param conection_handle  The new connection handle.
     * @param peer_address      The peer address of the associated device.
     * @param peer_address_id   @todo Nordic Specific: index into the device
     *                          identities list (as set with
     *                          sd_ble_gap_device_identities_set)
     *                          when the peer is using privacy.
     */
    void connect(
        uint16_t                    conection_handle,
        ble::gap::address const&    peer_address,
        uint8_t                     peer_address_id
        ) override;

    /**
     * A conection has been dropped.
     *
     * @param conection_handle The connection handle that was disconnected.
     * @param error_code       The HCI error code indicating why the connection
     *                         dropped.
     */
    void disconnect(
        uint16_t                conection_handle,
        ble::hci::error_code    error_code
        ) override;

    /**
     * The connection has timed out.
     *
     * @param conection_handle The peer's connection handle.
     * @param reason See @enum timeout_reason.
     */
    void timeout_expiration(
        uint16_t                    conection_handle,
        ble::gap::timeout_reason    reason
    ) override;

    /**
     * A request from the peer for updating the conection parameters.
     *
     * @param connection_parameters The newly requested connection setting.
     */
    void connection_parameter_update(
        uint16_t                                conection_handle,
        ble::gap::connection_parameters const&  connection_parameters
        ) override;

    /**
     * A request from the peer for updating the conection parameters.
     *
     * @param conection_handle The connection handle identifying the peer.
     * @param connection_parameters The newly requested connection setting.
     */
    void connection_parameter_update_request(
        uint16_t                                conection_handle,
        ble::gap::connection_parameters const&  connection_parameters
        ) override;

    /**
     * The peer is requesting a PHY layer change.
     *
     * @param conection_handle The connection handle identifying the peer.
     * @param phy_tx_preferred The preferred PHY for our Tx link.
     * @param phy_rx_preferred The preferred PHY for out Rx link.
     */
    void phy_update_request(
        uint16_t                        conection_handle,
        ble::gap::phy_layer_parameters  phy_tx_preferred,
        ble::gap::phy_layer_parameters  phy_rx_preferred
        ) override;

    /**
     * The PHY layer has changed.
     *
     * @param conection_handle The connection handle identifying the peer.
     * @param status An HCI error which may indicate why the change occurred.
     * @param phy_tx Our updated PHY Tx link.
     * @param phy_rx Our updated PHY Rx link.
     */
    void phy_update(
        uint16_t                        conection_handle,
        ble::hci::error_code            status,
        ble::gap::phy_layer_parameters  phy_tx,
        ble::gap::phy_layer_parameters  phy_rx
        ) override;

    /**
     * The peer has requested a link layer update.
     *
     * @param conection_handle The connection handle identifying the peer.
     * @param tx_length_max The maximum link layer Tx PDU in bytes.
     * @param rx_length_max The maximum link layer Rx PDU in bytes.
     * @param tx_interval_usec_max The maximum time required for one
     *                             link later PDU to be sent.
     * @param rx_interval_usec_max The maximum time required for one
     *                             link later PDU to be received.
     */
    void link_layer_update_request(
        uint16_t    conection_handle,
        uint16_t    tx_length_max,
        uint16_t    rx_length_max,
        uint16_t    tx_interval_usec_max,
        uint16_t    rx_interval_usec_max
        ) override;

    /**
     * The link layer has been updated.
     *
     * @param conection_handle The connection handle identifying the peer.
     * @param tx_length_max The maximum link layer Tx PDU in bytes.
     * @param rx_length_max The maximum link layer Rx PDU in bytes.
     * @param tx_interval_usec_max The maximum time required for one
     *                             link later PDU to be sent.
     * @param rx_interval_usec_max The maximum time required for one
     *                             link later PDU to be received.
     */
    void link_layer_update(
        uint16_t    conection_handle,
        uint16_t    tx_length_max,
        uint16_t    rx_length_max,
        uint16_t    tx_interval_usec_max,
        uint16_t    rx_interval_usec_max
        ) override;

    /**
     * The peer wishes to initiate a bond.
     *
     * @param conection_handle The connection handle identifying the peer.
     * @param bonding          @todo Should this always be true?
     * @param auth_req         The peer's authentication parameters required.
     */
    void security_request(
        uint16_t                                            conection_handle,
        bool                                                bonding,
        ble::gap::security::authentication_required const&  auth_req
        ) override;

    /**
     * See BLUETOOTH SPECIFICATION Version 5.0 | Vol 3, Part H, pages 2340-2342
     * Figure 3.3: Authentication Requirements Flags
     */
    void security_pairing_request(
        uint16_t                                  conection_handle,
        bool                                      bonding,
        ble::gap::security::pairing_request const &pair_req
        ) override;

    /**
     * @param key_type BLE_GAP_AUTH_KEY_TYPES @todo TBD, nordic specific
     * 0: None
     * 1: Pass Key
     * 2: OOB
     */
    void security_authentication_key_request(
        uint16_t    conection_handle,
        uint8_t     key_type
        ) override;

    /**
     * The peer has requested a secure connection.
     * @see BLUETOOTH SPECIFICATION Version 5.0 | Vol 3, Part H page 2350-2355
     *      Figure 3.11: LE Key Distribution Format
     *
     * @param conection_handle The peer's connection handle.
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
     * @param sign_key Indicates that the device shall distribute CSRK using the
     * Signing Information command.
     *
     * @param peer_address The peer's address.
     */
    void security_information_request(
        uint16_t                                    conection_handle,
        ble::gap::security::key_distribution const& key_dist,
        ble::gap::security::master_id const&        mater_id,
        ble::gap::address const&                    peer_address
        ) override;

    /**
     * Request to display a passkey to the user.
     *
     * @param passkey A six digit ASCII passkey value ['0':'9'].
     * @param match_request Requires a response to determine if the passkey
     *                      matched.
     */
    void security_passkey_display(
        uint16_t                                conection_handle,
        ble::gap::security::pass_key const&     passkey,
        bool                                    match_request
        ) override;

    /**
     * @see BLUETOOTH SPECIFICATION Version 5.0 | Vol 3, Part H page 2349
     *      Table 3.8: Notification Type
     * @param key_press_event key_press
     */
    void security_key_pressed(
        uint16_t                            conection_handle,
        ble::gap::security::passkey_event   key_press_event
        ) override;

    /**
     * Perform the Diffie-Hellman key calculation given the peer's public key.
     *
     * @param conection_handle The connection handle identifying the peer.
     * @param public_key The peer's public key.
     * @param oob_required Indicates that OOB authentication is required.
     */
    void security_DH_key_calculation_request(
        uint16_t                                conection_handle,
        ble::gap::security::pubk const&         public_key,
        bool                                    oob_required
        ) override;

    /**
     * During the pairing key exchange this function will get called multiple
     * times indicating the state of the pairing process.
     *
     * @param conection_handle  The connection handle identifying the peer.
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
     * Security Mode 1 Level 2: Encrypted link required,
     *                          MITM protection not necessary.
     * Security Mode 1 Level 3: MITM protected encrypted link required.
     * Security Mode 1 Level 4: LESC MITM protected encrypted link using a
     *                          128-bit strength encryption key required.
     *
     * Security Mode 2 Level 1: Signing or encryption required,
     *                          MITM protection not necessary.
     * Security Mode 2 Level 2: MITM protected signing required,
     *                          unless link is MITM protected encrypted.
     */
    void security_authentication_status(
        uint16_t                                    conection_handle,
        ble::gap::security::pairing_failure         pairing_status,
        uint8_t                                     error_source,
        bool                                        is_bonded,
        uint8_t                                     sec_mode_1_levels,
        uint8_t                                     sec_mode_2_levels,
        ble::gap::security::key_distribution const& kdist_own,
        ble::gap::security::key_distribution const& kdist_peer
        ) override;

    /**
     * @param security_mode  The security mode updated.
     * @param security_level The security level updated.
     * @see security_authentication_status for modes/levels explanation.
     *
     * @param key_size Length of currently active encryption key,
     *                 7 to 16 octets (only applicable for bonding procedures).
     */
    void connection_security_update(
        uint16_t    conection_handle,
        uint8_t     security_mode,
        uint8_t     security_level,
        uint8_t     key_size
        ) override;

    /**
     * The RSSI value from the peer has been updated.
     *
     * @param conection_handle The peer's connection handle.
     * @param rssi_dBm The RSSI value in dBm.
     */
    void rssi_update(
        uint16_t                    conection_handle,
        int8_t                      rssi_dBm
        ) override;

    /**
     * When in the central role, report that advertising data has been received.
     *
     * @param conection_handle The connection handle identifying the peer.
     * @param peer_address     The peer's address if it can be resolved.
     * @param direct_address   When the peer address is unresolvable.
     * @param rssi_dBm         The RSSI value in dBm.
     * @param scan_response    true if the data payload is the result of a
     *                         scan/response. false if it is advertising data.
     * @param data             The advertising or scan/response data pointer.
     * @param data_length      The advertising or scan/response data length
     *                         in bytes.
     */
    void advertising_report(
        uint16_t                    conection_handle,
        ble::gap::address const&    peer_address,
        ble::gap::address const&    direct_address,
        int8_t                      rssi_dBm,
        bool                        scan_response,
        void const*                 data,
        uint8_t                     data_length
        ) override;

    /**
     * The peer is requesting a signal strength report.
     *
     * @param conection_handle The connection handle identifying the peer.
     * @param peer_address The peer's address.
     * @param rssi_dBm The peer's RSSI in dBm.
     */
    void scan_report_request(
        uint16_t                    conection_handle,
        ble::gap::address const&    peer_address,
        int8_t                      rssi_dBm
        ) override;

private:
    nordic::ble_gap_event_observer nordic_gap_event_observer_;
};
