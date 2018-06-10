/**
 * @file nordic_ble_event_observables.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "ble/address.h"
#include "ble/hci_error_codes.h"
#include "ble/gap_types.h"

#include <cstdint>

/// @todo For now, incuding Nordic stuff. Eventually eliminate.
#include "ble/gap.h"

namespace ble
{

enum class phy_layer_parameters
{
    _1_Mbps,
    _2_Mbps,
    coded
};

namespace gap
{

/**
 * BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part E page 1163
 */
enum class authentication_requirements: uint8_t
{
    mitm_protection_required        (1u << 0u),
    bonding_dedicated               (1u << 1u).
    bonding_general                 (1u << 2u)
};

/**
 * @todo Nordic Specific, Taken from Nordic BLE_GAP_TIMEOUT_SOURCES
 */
enum class timeout_reason: uint8_t
{
    advertising             = 0,
    scanning                = 1,
    connection              = 2,
    authenticated_payload   = 3
};

/**
 * @see BLUETOOTH SPECIFICATION Version 5.0 | Vol 3, Part H page 2349
 *      Table 3.8: Notification Type
 */
enum class passkey_event: uint8_t
{
    entry_started   = 0,
    digit_entered   = 1,
    digit_erased    = 2,
    cleared         = 3,
    entry_completed = 4
};

/**
 * BLUETOOTH SPECIFICATION Version 5.0 | Vol 3, Part H page 2310, 2340
 * Table 2.5: I/O Capabilities Mapping
 * Table 3.4: IO Capability Values
 * @enum io_capabilities
 * @note None of the pairing algorithms can use Yes/No input and no output,
 * therefore NoInputNoOutput is used as the resulting IO capability.
 */
enum class io_capabilities: uint8_t
{
    display_only            = 0,
    display_yesno           = 1,
    keyboard_only           = 2,
    no_input_no_output      = 3,
    keyboard_display        = 4,
};

/**
 * @see BLUETOOTH SPECIFICATION Version 5.0 | Vol 3, Part H page 2340
 * Table 3.5: OOB Data Present Values
 * @enum oob_flags.
 * @note Not in use since this degenerates to bool for Version 5.0.
 */
enum class oob_flags
{
    oob_authentication_not_present  = 0,
    oob_authentication_present      = 1
};

/**
 * @see BLUETOOTH SPECIFICATION Version 5.0 | Vol 3, Part H page 2341
 * Table 3.6: Bonding Flags
 * @enum bonding_flags.
 * @note Not in use since this degenerates to bool for Version 5.0.
 */
enum class bonding_flags
{
    no_bonding  = 0,
    bonding     = 1
};

struct event_observer
{
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
     * @param peer_address_id   Nordic Specific index into the device identities
     *                          list (as set with sd_ble_gap_device_identities_set)
     *                          when the peer is using privacy.
     */
    virtual void connect(
        uint16_t            conection_handle,
        gap::address const  &peer_address,
        uint8_t             peer_address_id)                {}      // BLE_GAP_EVT_CONNECTED

    /**
     * A conection has been dropped.
     *
     * @param conection_handle The connection handle that was disconnected.
     * @param error_code        The HCI error code indicating why the connection dropped.
     */
    virtual void disconnect(
        uint16_t            conection_handle,
        hci::error_codes    error_code)                     {}      // BLE_GAP_EVT_DISCONNECTED

    /**
     * A request from the peer for updating the conection parameters.
     *
     * @param connection_parameters The newly requested connection setting.
     */
    virtual void connection_parameter_update(
        uint16_t                     conection_handle,
        connection_parameters const& connection_parameters) {}      // BLE_GAP_EVT_CONN_PARAM_UPDATE

    // For now got with what nordic has.
    /// @todo Check this.
    virtual void security_parameters_request(
        uint16_t                conection_handle,
        bool                    bonding,
        bool                    mitm_protection,
        bool                    le_secure_connection_pairing,
        bool                    keypress_notifications,
        bool                    oob_data_present,
        io_capabilities         io_caps,
        uint8_t                 key_size_min,
        uint8_t                 key_size_max,
        ble_gap_sec_kdist_t     key_distrib_own,
        ble_gap_sec_kdist_t     key_distrib_peer)           {}      // BLE_GAP_EVT_SEC_PARAMS_REQUEST

    /**
     * The peer has requested a secure connection.
     * @see BLUETOOTH SPECIFICATION Version 5.0 | Vol 3, Part H page 2350
     *      Figure 3.11: LE Key Distribution Format
     *
     * @param conection_handle The peer's connection handle.
     * @param address          The peer's address.
     * @param peer_address_id  @todo Nordic specific ?
     *                         An index into the Nordic security database.
     * @param encryption_key_required The peer is requiring encryption
     * @param identity_key_required   The peer is requiring identificaton (authentication?).
     * @param signing_key_required    The peer is requiring CSRK signatures.
     *                                (Connection Signature Resolving Key).
     */
    virtual void security_information_request(
        uint16_t                conection_handle,
        gap::address const      &address,
        uint8_t                 peer_address_id,
        bool                    encryption_key_required,
        bool                    identity_key_required,
        bool                    signing_key_required) {}    // BLE_GAP_EVT_SEC_INFO_REQUEST

    /**
     * @param passkey A six digit ASCII passkey value ['0':'9'].
     * @param match_request Requires a response to determine if the passkey matched.
     */
    virtual void security_passkey_display(
        uint16_t                    conection_handle,
        std::array<char, 6u> const& passkey,
        bool                        match_request
        )                                                   {}    // BLE_GAP_EVT_PASSKEY_DISPLAY

    /**
     * @see BLUETOOTH SPECIFICATION Version 5.0 | Vol 3, Part H page 2349
     *      Table 3.8: Notification Type
     * @param key_press_event key_press
     * This seems to serve little purpose.
     */
    virtual void security_key_pressed(
        uint16_t                conection_handle,
        passkey_event           key_press_event)              {}    // BLE_GAP_EVT_KEY_PRESSED

    /**
     * @param key_type BLE_GAP_AUTH_KEY_TYPES @todo TBD, nordic specific
     * 0: None
     * 1: Pass Key
     * 2: OOB
     */
    virtual void security_authentication_key_request(
        uint16_t                conection_handle,
        uint8_t                 key_type)                   {}    // BLE_GAP_EVT_AUTH_KEY_REQUEST

    virtual void security_DH_key_calculation_request(
        uint16_t                        conection_handle,
        std::array<uint8_t, 32u> const& dh_key,
        bool                            oob_required)       {}    // BLE_GAP_EVT_LESC_DHKEY_REQUEST

    virtual void security_authentication_complete(
        uint16_t                        conection_handle,
        ble_gap_evt_auth_status_t const &status)            {}    // BLE_GAP_EVT_AUTH_STATUS

    /**
     * @param security_mode
     * @param security_level
     * Security Mode 0 Level 0: No access permissions at all (this level is not defined by the Bluetooth Core specification).
     * Security Mode 1 Level 1: No security is needed (aka open link).
     * Security Mode 1 Level 2: Encrypted link required, MITM protection not necessary.
     * Security Mode 1 Level 3: MITM protected encrypted link required.
     * Security Mode 1 Level 4: LESC MITM protected encrypted link using a 128-bit strength encryption key required.
     *
     * Security Mode 2 Level 1: Signing or encryption required, MITM protection not necessary.
     * Security Mode 2 Level 2: MITM protected signing required, unless link is MITM protected encrypted.
     *
     * @param key_size
     * Length of currently active encryption key, 7 to 16 octets (only applicable for bonding procedures).
     */
    virtual void connection_security_update(
        uint16_t        conection_handle,
        uint8_t         security_mode,
        uint8_t         security_level,
        uint8_t         key_size)                           {}    // BLE_GAP_EVT_CONN_SEC_UPDATE

    /**
     * The connection has timed out.
     *
     * @param conection_handle The peer's connection handle.
     * @param reason See @enum timeout_reason.
     */
    virtual void timeout_expiration(
        uint16_t        conection_handle,
        timeout_reason  reason
        )                                                   {}    // BLE_GAP_EVT_TIMEOUT

    /**
     * The RSSI value from the peer has been updated.
     *
     * @param conection_handle The peer's connection handle.
     * @param rssi The RSSI value in dBm.
     */
    virtual void rssi_update(
        uint16_t                        conection_handle,
        int8_t                          rssi)               {}    // BLE_GAP_EVT_RSSI_CHANGED

    virtual void security_request()                     {}    // BLE_GAP_EVT_SEC_REQUEST
    virtual void scan_report_request()                  {}    // BLE_GAP_EVT_SCAN_REQ_REPORT

    /**
     * PHY requested by peer
     */
    virtual void phy_update_request(
        phy_layer_parameters tx,
        phy_layer_parameters rx)                        {}    // BLE_GAP_EVT_PHY_UPDATE_REQUEST

    virtual void phy_update_complete(hci::error_codes     error_code,
                                     phy_layer_parameters tx,
                                     phy_layer_parameters rx) {}    // BLE_GAP_EVT_PHY_UPDATE



};

} // namespace gap
} // namespace ble
