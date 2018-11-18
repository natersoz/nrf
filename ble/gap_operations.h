/**
 * @file ble/gap_operations.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "ble/gap_types.h"
#include "ble/hci_error_codes.h"

namespace ble
{
namespace gap
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
class operations
{
public:
    enum class status
    {
        success = 0,

        /// The connection handle supplied is invalid.
        invalid_connection,

        /// A parameter value supplied is invalid.
        invalid_parameter,

        /// The operation was attempted within an invalid state.
        invalid_state,

        /// Security Manager protocol timeout.
        smp_timeout,

        /// The number of authentication resources running simultaneously
        /// has been depleted.
        no_authentication_resource,

        busy,
        no_ble_slots,
        unimplemented,
        unknown_error
    };

    virtual ~operations()                     = default;

    operations()                              = default;
    operations(operations const&)             = delete;
    operations(operations &&)                 = delete;
    operations& operator=(operations const&)  = delete;
    operations& operator=(operations&&)       = delete;

    virtual status connect(
        ble::gap::address                       peer_address,
        ble::gap::connection_parameters const&  connection_parameters) = 0;

    virtual status connect_cancel() = 0;

    virtual status disconnect(uint16_t              connection_handle,
                              ble::hci::error_code  reason) = 0;

    /**
     * BLUETOOTH SPECIFICATION Version 5.0 | Vol 3, Part A
     * Figure 4.22: Connection Parameters Update Request Packet
     */
    virtual status connection_parameter_update_request(
        uint16_t                        connection_handle,
        connection_parameters const&    connection_parameters) = 0;

    /**
     * BLUETOOTH SPECIFICATION Version 5.0 | Vol 6, Part B
     * 2.4.2.22 LL_PHY_REQ and LL_PHY_RSP
     * Figure 2.40: CtrData field of the LL_LENGTH_REQ and LL_LENGTH_RSP PDUs
     * Table 4.3: Valid ranges for data PDU length management parameters
     *
     * 5.1.9 Data Length Update Procedure
     * Central or Peripheral can initiate the Data Length update procedure
     * once connected.
     *
     * @todo How is this different than an MTU length request ?
     */
    virtual status link_layer_length_update_request(
        uint16_t connection_handle,
        uint16_t rx_length_max,
        uint16_t rx_interval_usec_max,
        uint16_t tx_length_max,
        uint16_t tx_interval_usec_max) = 0;

    /**
     * BLUETOOTH SPECIFICATION Version 5.0 | Vol 6, Part B
     * Section 2.4.2.21 LL_LENGTH_REQ and LL_LENGTH_RSP
     * Figure 2.41: CtrData field of the LL_PHY_REQ and LL_PHY_RSP PDUs
     */
    virtual status phy_update_request(
        uint16_t                connection_handle,
        phy_layer_parameters    phy_rx,
        phy_layer_parameters    phy_tx) = 0;

    /**
     * Send a pairing response to the peer.
     */
    virtual status pairing_request(
        uint16_t                         connection_handle,
        bool                             create_bond,
        security::pairing_request const& pairing_request) = 0;

    /**
     * @see Bluetooth Core Specification 5.0, Volume 3, Part H
     * 3.5.2 Pairing Response
     *
     * Nordic: sd_ble_gap_sec_params_reply
     */
    virtual status pairing_response(
        uint16_t                          connection_handle,
        bool                              create_bond,
        security::pairing_response const& pairing_response) = 0;

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
        uint8_t* key_data) = 0;

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
        uint16_t               connection_handle,
        security::dhkey const& dhkey) = 0;

/*
    virtual status sd_ble_gap_sec_info_reply(
        uint16_t                connection_handle,
        ble_gap_enc_info_t      gap_enc_info,
        ble_gap_irk_t           gap_irk,
        ble_gap_sign_info_t     sign_info) = 0;
*/

};

} // namespace gap
} // namespace ble

