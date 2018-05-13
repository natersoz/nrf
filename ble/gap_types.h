/**
 * @gap_types.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include <cstdint>

namespace ble
{

/**
 * @enum flags
 * @see CSS_v5 Supplement to Bluetooth Core Specification page 12
 *
 * Bit      Decription
 * 0        BLE Limited Discovery mode
 * 1        BLE General Discovery mode
 * 2        BR/EDR Not Supported                  (always 1)
 * 3        Simultaneous LE and BR/EDR controller (always 0)
 * 4        Simultaneous LE and BR/EDR host       (always 0)
 * [5:7]    Reserved
 */
enum flags : uint8_t
{
    /**
     * A device is limited by time, typically no more than 60 seconds,
     * in which discovery is possible.
     */
    limited_discovery       = (1u << 0u),

    /// The device can always be discovered.
    general_discovery       = (1u << 1u),
    br_edr_not_supported    = (1u << 2u),
    le_br_edr_controller    = (1u << 3u),
    le_br_edr_host          = (1u << 4u),
    le_limited_discovery    = limited_discovery | br_edr_not_supported,
    le_general_discovery    = general_discovery | br_edr_not_supported
};

/**
 * @enum gap_type
 * EIR Data Type, Advertising Data Type (AD Type) and OOB Data Type Definitions.
 *
 * Generic Access Profile assigned numbers are used in
 * - GAP for inquiry response
 * - EIR data type values
 * - Manufacturer-specific data
 * - Advertising data
 * low energy UUIDs, appearance characteristics, class of device.
 *
 * Each of these are TLV (Type, Length Value) where the type is enumerated
 * in gap_type_t, the length is the length of the value octects (not including
 * the type octect) and the value is an array of octects of length 'L'.
 *
 * Supplement to the Bluetooth Core Specification, Part A:
 * https://www.bluetooth.com/specifications/bluetooth-core-specification
 * This is the document which describes each of the types enumerated below.
 */
enum class gap_type : uint8_t
{
    /// @see enum flags
    flags = 0x01,

    /** @{
     * UUID services comprise an array (called 'list' in BLE terminology) of
     * UUID values. A complete list enumerates all of the service UUIDs
     * supported by the device. An incomplete list indicates that the list is
     * partial and that the device supports more than the UUIDs listed.
     */
    uuid_service_16_incomplete  = 0x02,
    uuid_service_16_complete    = 0x03,
    uuid_service_32_incomplete  = 0x04,
    uuid_service_32_complete    = 0x05,
    uuid_service_128_incomplete = 0x06,
    uuid_service_128_complete   = 0x07,
    /** @} */

    /**
     * The shortened local name of the device.
     * The complete name can be read using the device name characteristic
     * after the connection has been established using GATT.
     */
    local_name_short = 0x08,

    /// The complete device name.
    local_name_complete = 0x09,

    /// The Tx power level in dBm as a signed 8-bit integer: -127 to +127 dBm.
    tx_power_level = 0x0A,

    sevice_class = 0x0D,

    /** @{
     * Secure Simple Pairing Out Of Band (OOB)
     *
     * Enable an out of band mechanism to communicate discovery information
     * as well as other information related to the pairing process.
     *
     * @note The Secure Simple Pairing Out of Band data types shall not be used in
     * advertising packets and shall be only used over an out-of-band mechanism.
     */
    simple_pairing_hash_C           = 0x0E,
    simple_pairing_hash_C_192       = 0x0E,
    simple_pairing_randomizer_R     = 0x0F,
    simple_pairing_randomizer_R_192 = 0x0F,
    /** @} */

    /// Device ID Profile v1.3 or later
    device_id = 0x10,

    /**
     * The Security Manager TK Value data type allows an out of band mechanism
     * to be used by the Security Manager to communicate the TK value.
     *
     * @note The Security Manager TK Value data type shall not be used in
     * advertising packets and shall be only used over an out-of-band mechanism.
     */
    security_manager_tk_value  = 0x10,

    /**
     * OOB flags. @see enum oob_flags
     *
     * @note The Security Manager Out of Band data types shall not be used
     * advertising packets and shall be only used over an out-of-band mechanism.
     */
    security_manager_oob_flags = 0x11,

    /**
     * The slave (peripheral) connection range; 2 16-bit unsigned integers:
     * minimum connection interval, maximum connection interval
     * in 1.25 msec increments, range: [0x0006:0x0C80],
     * 0xffff indicates an unspecified interval.
     */
    slave_connection_interval_range = 0x12,

    /** @{
     * Peripherals advertise the Service Solicitation data type to
     * invite Central devices that expose one or more of the services specified
     * in the Service Solicitation data to connect.
     */
    service_solicitation_uuid_list_16   = 0x14,
    service_solicitation_uuid_list_128  = 0x15,
    /** @} */

    /**
     * The Service Data data type consists of a service UUID
     * with the data associated with that service.
     */
    service_data_uuid_16 = 0x16,

    /**
     * The Public Target Address data type defines the address of one or more
     * intended recipients of an advertisement when one or more devices were
     * bonded using a public address. This data type is intended to be used
     * to avoid a situation where a bonded device unnecessarily responds
     * to an advertisement intended for another bonded device.
     *
     * The size of this data type is a multiple of 6 octets.
     */
    public_target_address = 0x17,

    /**
     * The Random Target Address data type defines the address of one or more
     * intended recipients of an advertisement when one or more devices were
     * bonded using a random address.
     *
     * The size of this data type is a multiple of 6 octets.
     */
    random_target_address = 0x18,

    /**
     * The Appearance data type shall exist only once. It may be sent either
     * in the Advertising or Scan Response data, but not both.
     *
     * @todo See BLE assigned numbers TBD
     */
    appearance = 0x19,

    /// The advertising interval in 0.625 msec units.
    /// Data payload: unsigned 16-bit integer.
    advertising_interval = 0x1A,

    /**
     * 7 octets of BLE device address.
     * The 6 least significant Octets is the Device Address.
     * The most sigificant octet, bit 0 indicates whether the address is
     *   0: public
     *   1: random
     * The other bits in the MSByte octet are reserved.
     */
    device_dddress = 0x1B,

    /// @see enum le_role
    role = 0x1C,

    /** @{ @see simple_pairing_hash_C */
    simple_pairing_hash_C_256       = 0x1D,
    simple_pairing_randomizer_R_256 = 0x1E,
    /** @} */

    /// @see service_solicitation_uuid_list_16
    service_solicitation_uuid_list_32 = 0x1F,

    service_data_uuid_32 = 0x20,        ///< @see service_data_uuid_16.
    service_data_uuid_128 = 0x21,       ///< @see service_data_uuid_16.

    secure_connections_confirmation_value = 0x22,
    secure_connections_random_value = 0x23,

    /// TBD: org.bluetooth.characteristic.uri
    uri = 0x24,

    /// Indoor Posiioning Service v1.0 or later
    indoor_positioning  = 0x25,

    /// Transport Discovery Service v1.0 or later
    transport_discovery_data = 0x26,

    supported_features = 0x27,

    channel_map_update_indication   = 0x28,

    /// Mesh Profile Specification Section 5.2.1
    PB_ADV  = 0x29,

    /// Mesh Profile Specification Section 3.3.1
    mesh_message = 0x2A,

    /// Mesh Profile Specification Section 3.9
    mesh_beacon = 0x2B,

    /// 3D Synchronization Profile, v1.0 or later
    sync_3D_information_data = 0x3D,

    /**
     * The first 2 bytes are company specific identifiers:
     * https://www.bluetooth.com/specifications/assigned-numbers/company-identifiers
     * The remaining octets carry additional manufacturer specific data.
     *
     * @todo This appears to be sent LSByte first as Apple sent as [4C 00]
     * and the Apple value is 0x004C.
     */
    manufacturer_specific_data = 0xFF,
};

namespace secutiry_manager
{

/**
 * @enum oob_flags
 *
 * @note Deliberately not using enum class here so that oob_flags can
 * of bit-wise or-d together
 *
 */
enum oob_flags : uint8_t
{
    oob_data_present        = 1u << 0u,
    le_supported            = 1u << 1u,
    simultaneous_le_bdr     = 1u << 2u,
    address_type_random     = 1u << 3u,
};

enum le_role : uint8_t
{
    peripheral_only      = 0x00,
    central_only         = 0x01,
    peripheral_preferred = 0x02, ///< Dual role supported; Peripheral preferred.
    central_preferred    = 0x03, ///< Dual role supported; Central preferred.
};

} // namespcae security_manager

} // namespace ble

