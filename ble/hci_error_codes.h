/**
 * @file nordic_ble_event_observables.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

namespace ble
{
namespace hci
{

/**
 * @enum error_codes
 * BLUETOOTH SPECIFICATION Version 5.0 | Vol 2, Part D page 643
 */
enum class error_codes: uint8_t
{
    success                                                 = 0x00,
    unknown_hci_command                                     = 0x01,
    unknown_connection_identifier                           = 0x02,
    hardware_failure                                        = 0x03,
    page_timeout                                            = 0x04,
    authentication_failure                                  = 0x05,
    pin_or_key_missing                                      = 0x06,
    memory_capacity_exceeded                                = 0x07,
    connection_timeout                                      = 0x08,
    connection_limit_exceeded                               = 0x09,
    synchronous_connection_limit_exceeded                   = 0x0A,
    connection_already_exists                               = 0x0B,
    command_disallowed                                      = 0x0C,
    connection_rejected_limited_resources                   = 0x0D,
    connection_rejected_security_reasons                    = 0x0E,
    connection_rejected_unacceptable_bd_addr                = 0x0F,
    connection_accept_timeout_exceeded                      = 0x10,
    unsupported_feature_or_parameter_value                  = 0x11,
    invalid_hci_command_parameters                          = 0x12,
    remote_user_terminated_connection                       = 0x13,
    remote_connection_terminated_low_resources              = 0x14,
    remote_connection_terminated_power_off                  = 0x15,
    connection_terminated_by_local_host                     = 0x16,
    repeated_attempts                                       = 0x17,
    pairing_not_allowed                                     = 0x18,
    unknown_ll_pdu                                          = 0x19,
    unsupported_remote_feature                              = 0x1A,
    sco_offset_rejected                                     = 0x1B,
    sco_interval_rejected                                   = 0x1C,
    sco_air_mode_rejected                                   = 0x1D,
    invalid_ll_parameters                                   = 0x1E,
    unspecified_error                                       = 0x1F,
    unsupported_ll_parameter_value                          = 0x20,
    role_change_not_allowed                                 = 0x21,
    ll_response_timeout                                     = 0x22,
    ll_procedure_collision                                  = 0x23,
    ll_pdu_not_allowed                                      = 0x24,
    encryption_mode_not_acceptable                          = 0x25,
    link_key_cannot_be_changed                              = 0x26,
    requested_qos_not_supported                             = 0x27,
    instant_passed                                          = 0x28,
    pairing_with_unit_key_not_supported                     = 0x29,
    different_transaction_collision                         = 0x2A,
    reserved_for_future_use_0x2B                            = 0x2B,
    qos_unacceptable_parameter                              = 0x2C,
    qos_rejected                                            = 0x2D,
    channel_classification_not_supported                    = 0x2E,
    insufficient_security                                   = 0x2F,
    parameter_out_of_mandatory_range                        = 0x30,
    reserved_for_future_use_0x31                            = 0x31,
    role_switch_pending                                     = 0x32,
    reserved_for_future_use_0x33                            = 0x33,
    reserved_slot_violation                                 = 0x34,
    role_switch_failed                                      = 0x35,
    extended_inquiry_response_too_large                     = 0x36,
    secure_simple_pairing_not_supported_by_host             = 0x37,
    host_busy_pairing                                       = 0x38,
    connection_rejected_no_suitable_channel_found           = 0x39,
    controller_busy                                         = 0x3A,
    unacceptable_connection_parameters                      = 0x3B,
    advertising_timeout                                     = 0x3C,
    connection_terminated_mic_failure                       = 0x3D,
    connection_failed_to_be_established                     = 0x3E,
    mac_connection_failed                                   = 0x3F,
    coarse_clock_adjustment_rejected                        = 0x40,     // retry_to_adjust_using_clock_dragging
    type0_submap_not_defined                                = 0x41,
    unknown_advertising_identifier                          = 0x42,
    limit_reached                                           = 0x43,
    Operation_Cancelled_by_Host                             = 0x44,
};

} // hci
} // ble
