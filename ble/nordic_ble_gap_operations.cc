/**
 * @file ble/nordic_ble_gap_operations.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "ble/nordic_ble_gap_operations.h"
#include "ble_gap.h"
#include "project_assert.h"
#include "nrf_error.h"
#include "ble_err.h"

#include <algorithm>
#include <cstring>

namespace nordic
{

using status = ble::gap::operations::status;

static status status_code(uint32_t nordic_error_code)
{
    switch (nordic_error_code)
    {
    case NRF_SUCCESS:                       return status::success;
    case BLE_ERROR_INVALID_CONN_HANDLE:     return status::invalid_connection;
    case NRF_ERROR_INVALID_PARAM:           return status::invalid_parameter;
    case NRF_ERROR_INVALID_STATE:           return status::invalid_state;
    case NRF_ERROR_TIMEOUT:                 return status::smp_timeout;
    case NRF_ERROR_NO_MEM:                  return status::no_authentication_resource;
    case NRF_ERROR_BUSY:                    return status::busy;
    case NRF_ERROR_RESOURCES:               return status::no_ble_slots;
    default:    ASSERT(0);                  return status::unknown_error;
    }
}

int8_t ble_gap_operations::tx_power_level(int8_t tx_power_dBm)
{
    int8_t const nordic_tx_power[] = { -40, -20, -16, -12, -8, -4, 0, +3, 4 };

    // tx_power_ptr points to the first Nordic Tx power >= tx_power_dBm.
    int8_t const* tx_power_ptr = std::lower_bound(
        std::begin(nordic_tx_power), std::end(nordic_tx_power), tx_power_dBm);

    // Request > highest nordic_tx_power[] value? truncate to highest.
    tx_power_ptr = std::min(tx_power_ptr, std::end(nordic_tx_power) - 1);

    return *tx_power_ptr;
}

status ble_gap_operations::connect(
    ble::gap::address                       peer_address,
    ble::gap::connection_parameters const&  connection_parameters)
{
    return status::unimplemented;
}

status ble_gap_operations::connect_cancel()
{
    return status::unimplemented;
}

status ble_gap_operations::disconnect(
    uint16_t             connection_handle,
    ble::hci::error_code reason)
{
    uint32_t error_code = sd_ble_gap_disconnect(connection_handle,
                                                static_cast<uint8_t>(reason));
    return status_code(error_code);
}

status ble_gap_operations::connection_parameter_update_request(
    uint16_t                                connection_handle,
    ble::gap::connection_parameters const&  connection_parameters)
{
    ble_gap_conn_params_t const conn_params = {
        .min_conn_interval  = connection_parameters.interval_min,
        .max_conn_interval  = connection_parameters.interval_max,
        .slave_latency      = connection_parameters.slave_latency,
        .conn_sup_timeout   = connection_parameters.supervision_timeout
    };

    uint32_t error_code = sd_ble_gap_conn_param_update(connection_handle,
                                                       &conn_params);
    return status_code(error_code);
}

status ble_gap_operations::link_layer_length_update_request(
    uint16_t connection_handle,
    uint16_t rx_length_max,
    uint16_t rx_interval_usec_max,
    uint16_t tx_length_max,
    uint16_t tx_interval_usec_max)
{
    /// @todo This appears to be a Nordic specific means for negotiating MTU?
    /// See softdevice documentation file:///Users/natersoz/src/nRF/SDK/nRF5_SDK_14.2.0_documentation/s132/group___b_l_e___g_a_p___d_a_t_a___l_e_n_g_t_h___u_p_d_a_t_e___p_r_o_c_e_d_u_r_e___m_s_c.html
    /// Confusing as hell Nordic.

    /// BLUETOOTH SPECIFICATION Version 5.0 | Vol 6, Part B, Section 2.4.2.21
    /// LL_LENGTH_REQ and LL_LENGTH_RSP, the following rules apply:
    ble_gap_data_length_params_t const gap_data_length = {
        .max_tx_octets     = tx_length_max,
        .max_rx_octets     = rx_length_max,
        .max_tx_time_us    = tx_interval_usec_max,
        .max_rx_time_us    = rx_interval_usec_max
    };

    /// @todo what does this mean? figure it out.
    ble_gap_data_length_limitation_t gap_data_limit = {
        .tx_payload_limited_octets  = 0,
        .rx_payload_limited_octets  = 0,
        .tx_rx_time_limited_us      = 0
    };

    uint32_t error_code = sd_ble_gap_data_length_update(connection_handle,
                                                        &gap_data_length,
                                                        &gap_data_limit);
    return status_code(error_code);
}

status ble_gap_operations::phy_update_request(
    uint16_t                        connection_handle,
    ble::gap::phy_layer_parameters  phy_rx,
    ble::gap::phy_layer_parameters  phy_tx)
{
    ble_gap_phys_t gap_phy = {
        .tx_phys = static_cast<uint8_t>(phy_tx),
        .rx_phys = static_cast<uint8_t>(phy_rx)
    };

    uint32_t error_code = sd_ble_gap_phy_update(connection_handle, &gap_phy);
    return status_code(error_code);
}

status ble_gap_operations::pairing_request(
    uint16_t                                    connection_handle,
    bool                                        create_bond,
    ble::gap::security::pairing_request const&  pairing_request)
{
    ble_gap_sec_params_t const gap_sec = {
        .bond           = create_bond,
        .mitm           = pairing_request.auth_required.mitm,
        .lesc           = pairing_request.auth_required.lesc,
        .keypress       = pairing_request.auth_required.keypress,
        .io_caps        = static_cast<uint8_t>(pairing_request.io_caps),
        .oob            = static_cast<uint8_t>(pairing_request.oob),
        .min_key_size   = pairing_request.encryption_key_size_min,
        .max_key_size   = pairing_request.encryption_key_size_max,
        .kdist_own = {
            .enc        = pairing_request.initiator_key_distribution.enc_key,
            .id         = pairing_request.initiator_key_distribution.id_key,
            .sign       = pairing_request.initiator_key_distribution.sign_key,
            .link       = pairing_request.initiator_key_distribution.link_key
        },
        .kdist_peer = {
            .enc        = pairing_request.responder_key_distribution.enc_key,
            .id         = pairing_request.responder_key_distribution.id_key,
            .sign       = pairing_request.responder_key_distribution.sign_key,
            .link       = pairing_request.responder_key_distribution.link_key
        },
    };

    uint32_t error_code = sd_ble_gap_authenticate(connection_handle, &gap_sec);
    return status_code(error_code);
}

status ble_gap_operations::pairing_response(
    uint16_t                                    connection_handle,
    bool                                        create_bond,
    ble::gap::security::pairing_response const& pairing_response)
{
    ble_gap_sec_params_t const gap_sec = {
        .bond           = create_bond,
        .mitm           = pairing_response.auth_required.mitm,
        .lesc           = pairing_response.auth_required.lesc,
        .keypress       = pairing_response.auth_required.keypress,
        .io_caps        = static_cast<uint8_t>(pairing_response.io_caps),
        .oob            = static_cast<uint8_t>(pairing_response.oob),
        .min_key_size   = pairing_response.encryption_key_size_min,
        .max_key_size   = pairing_response.encryption_key_size_max,
        .kdist_own = {
            .enc        = pairing_response.initiator_key_distribution.enc_key,
            .id         = pairing_response.initiator_key_distribution.id_key,
            .sign       = pairing_response.initiator_key_distribution.sign_key,
            .link       = pairing_response.initiator_key_distribution.link_key
        },
        .kdist_peer = {
            .enc        = pairing_response.responder_key_distribution.enc_key,
            .id         = pairing_response.responder_key_distribution.id_key,
            .sign       = pairing_response.responder_key_distribution.sign_key,
            .link       = pairing_response.responder_key_distribution.link_key
        },
    };

    /// @todo understand SM
    ble_gap_sec_keyset_t  key_set = {
        .keys_own = {
            /// Encryption Key, or NULL.
            key_set.keys_own.p_enc_key  = nullptr,
            /// Identity Key, or NULL.
            key_set.keys_own.p_id_key   = nullptr,
            /// Signing Key, or NULL.
            key_set.keys_own.p_sign_key = nullptr,
            /// DH public key or NULL
            key_set.keys_own.p_pk       = nullptr,
        },
        .keys_peer = {
            /// Encryption Key, or NULL.
            key_set.keys_own.p_enc_key  = nullptr,
            /// Identity Key, or NULL.
            key_set.keys_own.p_id_key   = nullptr,
            /// Signing Key, or NULL.
            key_set.keys_own.p_sign_key = nullptr,
            /// DH public key or NULL
            key_set.keys_own.p_pk       = nullptr,
        },
    };

    uint32_t error_code = sd_ble_gap_sec_params_reply(connection_handle,
                                                      0u,
                                                      &gap_sec,
                                                      &key_set);
    return status_code(error_code);
}

status ble_gap_operations::security_authentication_key_response(
    uint16_t connection_handle,
    uint8_t  key_type,
    uint8_t* key_data)
{
    uint32_t error_code = sd_ble_gap_auth_key_reply(connection_handle,
                                                    key_type,
                                                    key_data);
    return status_code(error_code);
}

status ble_gap_operations::pairing_dhkey_response(
    uint16_t                         connection_handle,
    ble::gap::security::dhkey const& dhkey)
{
    ble_gap_lesc_dhkey_t lesc_dhkey;
    static_assert(sizeof(lesc_dhkey.key) == dhkey.size());

    memcpy(lesc_dhkey.key, dhkey.data(), dhkey.size());

    uint32_t error_code = sd_ble_gap_lesc_dhkey_reply(
        connection_handle,
        &lesc_dhkey);

    return status_code(error_code);
}

/** @todo Need to understand
status ble_gap_operations::sd_ble_gap_sec_info_reply(
    uint16_t                connection_handle,
    ble_gap_enc_info_t      gap_enc_info,
    ble_gap_irk_t           gap_irk,
    ble_gap_sign_info_t     sign_info)
{
    return status_code(error_code);
}
*/

} // namespace nordic

