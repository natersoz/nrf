/**
 * @file ble/gap_reqpy_request.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "ble/gap_types.h"

namespace ble
{
namespace gap
{

/**
 * @class ble::gap::request_reply
 *
 * A generic BLE set of GAP commands for sending requests and replies to GAP
 * events. This is a pure virtual interface intended to be override by a
 * specific silicon implementation.
 *
 * The class pairs with the ble::gap::event_observer within the context of a
 * ble::gap::connection to create a command/response structure for handling
 * the BLE GAP.
 */
class request_reply
{
public:
    virtual ~request_reply()                        = default;

    request_reply()                                 = default;
    request_reply(request_reply const&)             = delete;
    request_reply(request_reply &&)                 = delete;
    request_reply& operator=(request_reply const&)  = delete;
    request_reply& operator=(request_reply&&)       = delete;

    virtual sd_ble_gap_sec_params_reply() = 0;
    virtual sd_ble_gap_sec_info_reply() = 0;
    virtual sd_ble_gap_auth_key_reply() = 0;


    /**
     * @see Bluetooth Core Specification 5.0, Volume 3, Part H
     * 3.5.2 Pairing Response
     *
     * Nordic: sd_ble_gap_sec_params_reply
     */
    virtual void pairing_response(
        uint16_t connection_handle,
        security::pairing_response const &response) = 0;

    /**
     * BLUETOOTH SPECIFICATION Version 5.0 | Vol 3, Part H page 2348
     * 3.5.7 Pairing DHKey Check
     */
    virtual void pairing_dhkey_check(
        uint16_t connection_handle,
        security::dhkey_check const &dhkey_check) = 0;

    /**
     * BLUETOOTH SPECIFICATION Version 5.0 | Vol 3, Part A
     * Figure 4.22: Connection Parameters Update Request Packet
     */
    virtual connection_parameter_update_request(uint16_t connection_handle,
                                                connection_parameters const &connection_parameters) = 0;

    virtual phy_update_request(uint16_t connection_handle,
                               phy_layer_parameters phy_rx,
                               phy_layer_parameters phy_tx) = 0;

    /**
     * BLUETOOTH SPECIFICATION Version 5.0 | Vol 6, Part B
     * Section 2.4.2.21 LL_LENGTH_REQ and LL_LENGTH_RSP
     * Table 4.3: Valid ranges for data PDU length management parameters
     */
    virtual void link_layer_length_update_request(uint16_t connection_handle,
                                                  uint16_t rx_length_max,
                                                  uint16_t rx_interval_usec_max
                                                  uint16_t tx_length_max,
                                                  uint16_t tx_interval_usec_max) = 0;

    virtual sd_ble_gap_addr_set() = 0;
    virtual sd_ble_gap_addr_get() = 0;
    virtual sd_ble_gap_whitelist_set() = 0;     // address, count

    /// @todo TBD
    virtual sd_ble_gap_device_identities_set() = 0;
    virtual sd_ble_gap_privacy_set(ble_gap_privacy_params_t) = 0;
    virtual sd_ble_gap_privacy_get(ble_gap_privacy_params_t) = 0;
//    virtual sd_ble_gap_adv_data_set() = 0;
//    virtual sd_ble_gap_adv_start() = 0;
//    virtual sd_ble_gap_adv_stop() = 0;


    virtual void negotiate_ppcp() override;
    virtual void negotiate_mtu() override;
    virtual void disconnect(ble::hci::error_code reason) override;

    virtual void sd_ble_gap_tx_power_set(int8_t) = 0;
    virtual void sd_ble_gap_appearance_set(uint16_t) = 0;
    virtual uint16_t sd_ble_gap_appearance_get() = 0;
    virtual void sd_ble_gap_ppcp_set(ble_gap_conn_params_t) = 0;
    virtual ble_gap_conn_params_t sd_ble_gap_ppcp_get() = 0;
    virtual void sd_ble_gap_device_name_set(char const*, att:length_t length) = 0;
    virtual char const* sd_ble_gap_device_name_get() = 0;


    virtual sd_ble_gap_authenticate(uint16_t conn_handle,) = 0;
    virtual sd_ble_gap_sec_params_reply(uint16_t conn_handle, ble_gap_sec_params_t) = 0;
    virtual sd_ble_gap_auth_key_reply() = 0;
    virtual sd_ble_gap_lesc_dhkey_reply() = 0;
    virtual sd_ble_gap_keypress_notify() = 0;
    virtual sd_ble_gap_lesc_oob_data_get() = 0;
    virtual sd_ble_gap_lesc_oob_data_set() = 0;
    virtual sd_ble_gap_encrypt() = 0;
    virtual sd_ble_gap_sec_info_reply() = 0;
    virtual sd_ble_gap_conn_sec_get() = 0;


    virtual sd_ble_gap_rssi_start() = 0;
    virtual sd_ble_gap_rssi_stop() = 0;
    virtual sd_ble_gap_rssi_get() = 0;
    virtual sd_ble_gap_scan_start() = 0;
    virtual sd_ble_gap_scan_stop() = 0;

    virtual void () = 0;
    virtual sd_ble_gap_connect() = 0;
    virtual sd_ble_gap_connect_cancel() = 0;
    virtual sd_ble_gap_phy_update() = 0;
    virtual sd_ble_gap_data_length_update() = 0;
    virtual () = 0;
    virtual () = 0;



};

} // namespace gap
} // namespace ble

