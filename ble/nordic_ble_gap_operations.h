/**
 * @file ble/nordic_ble_gap_operations.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "ble/gap_operations.h"

namespace nordic
{

/**
 * @interface ble::gap::operations
 *
 * A generic BLE set of GAP commands for sending GAP requests and responses.
 * This is a pure virtual interface intended to be override by a
 * specific silicon vendor implementation; like Nordic's softdevice.
 *
 * The class pairs with the ble::gap::event_observer within the context of a
 * ble::gap::connection to create a command/response structure for handling
 * the BLE GAP.
 */
class ble_gap_operations: public ble::gap::operations
{
public:
    using status = ble::gap::operations::status;

    /**
     * Get the nearest Nordic valid Transmit power level which is greater than
     * or equal to the target power level in dBm. When calling the function
     * sd_ble_gap_tx_power_set() the parameter tx_power must be a valid value.
     *
     * @param target_power_dBm The desired transmit power level in dBm.
     * @return int8_t          The valid Nordic power level in dBm.
     */
    static int8_t tx_power_level(int8_t target_power_dBm);

    virtual ~ble_gap_operations() override                    = default;

    ble_gap_operations()                                      = default;
    ble_gap_operations(ble_gap_operations const&)             = delete;
    ble_gap_operations(ble_gap_operations &&)                 = delete;
    ble_gap_operations& operator=(ble_gap_operations const&)  = delete;
    ble_gap_operations& operator=(ble_gap_operations&&)       = delete;

    virtual status connect(
        ble::gap::address                       peer_address,
        ble::gap::connection_parameters const&  connection_parameters) override;

    virtual status connect_cancel() override;

    virtual status disconnect(
        uint16_t             connection_handle,
        ble::hci::error_code reason) override;

    /**
     * BLUETOOTH SPECIFICATION Version 5.0 | Vol 3, Part A
     * Figure 4.22: Connection Parameters Update Request Packet
     */
    virtual status connection_parameter_update_request(
        uint16_t                               connection_handle,
        ble::gap::connection_parameters const& connection_parameters) override;

    /**
     * BLUETOOTH SPECIFICATION Version 5.0 | Vol 6, Part B
     * Section 2.4.2.21 LL_LENGTH_REQ and LL_LENGTH_RSP
     * Table 4.3: Valid ranges for data PDU length management parameters
     */
    virtual status link_layer_length_update_request(
        uint16_t connection_handle,
        uint16_t rx_length_max,
        uint16_t rx_interval_usec_max,
        uint16_t tx_length_max,
        uint16_t tx_interval_usec_max) override;

    virtual status phy_update_request(
        uint16_t                        connection_handle,
        ble::gap::phy_layer_parameters  phy_rx,
        ble::gap::phy_layer_parameters  phy_tx) override;

    /**
     * Send a pairing response to the peer.
     *
     * Nordic: sd_ble_gap_authenticate()
     */
    virtual status pairing_request(
        uint16_t                                   connection_handle,
        bool                                       create_bond,
        ble::gap::security::pairing_request const& pairing_request) override;

    /**
     * @see Bluetooth Core Specification 5.0, Volume 3, Part H
     * 3.5.2 Pairing Response
     *
     * Nordic: sd_ble_gap_sec_params_reply
     */
    virtual status pairing_response(
        uint16_t                                    connection_handle,
        bool                                        create_bond,
        ble::gap::security::pairing_response const& pairing_response) override;

    /**
     * Nordic: send reply to a @ref BLE_GAP_EVT_AUTH_KEY_REQUEST or a
     * @ref BLE_GAP_EVT_PASSKEY_DISPLAY
     *
     * keytype:                         key_data
     * BLE_GAP_AUTH_KEY_TYPE_PASSKEY:   then a 6-byte ASCII digits ['0;:'9']
     * BLE_GAP_AUTH_KEY_TYPE_OOB:       16-byte OOB, little endian.
     *
     * @param connection_handle
     * @param key_data
     */
    virtual status security_authentication_key_response(
        uint16_t connection_handle,
        uint8_t  key_type,
        uint8_t* key_data) override;

    /**
     * BLUETOOTH SPECIFICATION Version 5.0 | Vol 3, Part H page 2348
     * 3.5.7 Pairing DHKey Check
     *
     * This message is used to transmit the 128-bit DHKey Check values (Ea/Eb)
     * generated using f6. This message is used by both initiator and responder.
     * This PDU is only used for LE Secure Connections.
     *
     * Nordic sd_ble_gap_lesc_dhkey_reply
     */
    virtual status pairing_dhkey_response(
        uint16_t                         connection_handle,
        ble::gap::security::dhkey const& dhkey) override;
/*
    virtual status sd_ble_gap_sec_info_reply(
        uint16_t                connection_handle,
        ble_gap_enc_info_t      gap_enc_info,
        ble_gap_irk_t           gap_irk,
        ble_gap_sign_info_t     sign_info) override;
*/
};

} // namespace nordic

