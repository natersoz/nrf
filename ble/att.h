/**
 * @file ble/att.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Declare the namespace ble::att, the Bluetooth Attribute protocol.
 */

#pragma once

#include <type_traits>
#include <cstdint>
#include <utility>

namespace ble
{

/**
 * @namespace att The Bluetooth attribute protocol (ATT).
 * See Bluetooth Core Specification Part F: Attribute Protocol.
 */
namespace att
{

/**
 * The invalid attribute handle value.
 * @note This is different than the ble::gap::handle_invalid value.
 *       A conneciton handle value zero is expected and is normal.
 * @see BLUETOOTH SPECIFICATION Version 5.0 | Vol 3, Part F page 2174
 *      3.2.2 Attribute Handle
 */
static constexpr uint16_t const handle_invalid = 0u;

/** @{
 * The minimum and maximum attribute handle value.
 * Characteristics will always maintain handle values in the range
 * [ble::att::handle_minimum ... ble::att::handle_maximum].
 *
 * @see BLUETOOTH SPECIFICATION Version 5.0 | Vol 3, Part F page 2174
 *      3.2.2 Attribute Handle
 */
static constexpr uint16_t const handle_minimum = 0x0001u;
static constexpr uint16_t const handle_maximum = UINT16_MAX;
/** @} */

/**
 * An BLE ATT handle range is inclusive of the first and second members within
 * the pair.
 */
using handle_range = std::pair<uint16_t, uint16_t>;

/**
 * @typedef ble::att::length_t
 * Define an attribute lenth type similar to std::size_t but for use with BLE.
 * @todo - make this size_t?
 */
using length_t          = uint16_t;
using signed_length_t   = std::make_signed<length_t>::type;

/**
 * @see Bluetooth Core Specification 5.0, Volume 3, Part F
 * Section 5.2.1 ATT_MTU, Table 5.1: LE L2CAP ATT_MTU
 */
static constexpr length_t const mtu_length_minimum =  23u;
static constexpr length_t const mtu_length_maximum = 251u;

enum class permissions
{
    readable                = 0x01,
    writeable               = 0x02,
    read_write              = 0x03,
    excryption_required     = 0x04,
    authentication_required = 0x08,
    authorization_required  = 0x10,
};

/**
 * @enum error_code
 * @see Bluetooth Core Specification 5.0, Volume 3, Part F
 * Section 3.4.1.1 Error Response, Table 3.3: Error Codes
 */
enum class error_code : uint16_t
{
    /// @note success is not found in the Core specification
    /// table cited, but we need something that indicates success
    /// in the cases where status is reported and there is no failure.
    success                             = 0x00,

    /// The attribute handle given was not valid on this server.
    invalid_handle                      = 0x01,

    /// The attribute cannot be read.
    read_not_permitted                  = 0x02,

    /// The attribute cannot be written.
    write_not_permitted                 = 0x03,

    /// The attribute PDU is invalid.
    invalid_pdu                         = 0x04,

    /// The attribute requires authentication before it can be read or written.
    insufficient_authentication         = 0x05,

    /// Attribute server does not support the request received from the client.
    request_not_supported               = 0x06,

    /// Offset specified was past the end of the attribute.
    invalid_offset                      = 0x07,

    /// The attribute requires authorization before it can be read or written.
    insufficient_authorization          = 0x08,

    /// Too many prepare writes have been queued.
    prepare_queue_full                  = 0x09,

    /// No attribute found within the given attribute handle range.
    attribute_not_found                 = 0x0a,

    /// The attribute cannot be read using the Read Blob Request.
    attribute_not_long                  = 0x0b,

    /// The Encryption Key Size used for encrypting this link is insufficient.
    insufficient_encryption_key_size    = 0x0c,

    /// The attribute value length is invalid for the operation.
    invalid_attribute_value_length      = 0x0d,

    /// Something unlikely (WTF) happened.
    unlikely_error                      = 0x0e,

    /// The attribute requires encryption before it can be read or written.
    insufficient_encryption             = 0x0f,

    /// The attribute type is not a supported grouping attribute as defined by a higher layer specification.
    unsupported_group_type              = 0x10,

    /// Insufficient Resources to complete the request.
    insufficient_resources              = 0x11,

    /// Vendor extensions are not part of the Core specification.
    /// Vendors extend things and their extended values go here
    vendor_extension_begin              = 0x200,

    /// Not part of the core extension.
    unknown                             = 0xffff
};

/**
 * @enum op_code
 * @see Bluetooth Core Specification 5.0, Volume 3, Part F
 * 3.4.8 Attribute Opcode Summary, Table 3.37: Attribute Protocol Summary
 */
enum class op_code: uint8_t
{
    /// @note 'invalid' is not part of Core specification.
    /// It is added here to indicate the op_code type is invalid and does
    /// not map to a known op_code.
    invalid                     = 0x00,

    error_response              = 0x01,

    mtu_exchange_request        = 0x02,
    mtu_exchange_response       = 0x03,

    find_information_request    = 0x04,
    find_information_response   = 0x05,
    find_by_type_value_request  = 0x06,
    find_by_type_value_response = 0x07,

    read_by_type_request        = 0x08,
    read_by_type_response       = 0x09,
    read_request                = 0x0a,
    read_response               = 0x0b,
    read_blob_request           = 0x0c,
    read_blob_response          = 0x0d,
    read_multiple_request       = 0x0e,
    read_multiple_response      = 0x0f,
    read_by_group_type_request  = 0x10,
    read_by_group_type_response = 0x11,

    write_request               = 0x12,
    write_response              = 0x13,
    write_command               = 0x52,
    write_signed_command        = 0xd2,
    write_prepare_request       = 0x16,
    write_prepare_response      = 0x17,
    write_execute_request       = 0x18,
    write_execute_response      = 0x19,

    handle_value_notification   = 0x1b,
    handle_value_indication     = 0x1d,
    handle_value_confirmation   = 0x1e,
};

/**
 * @see Bluetooth Core Specification 5.0, Volume 3, Part F
 * 3.4.1.1 Error Response, Table 3.2: Format of Error Response
 */
struct error_response
{
    enum op_code    op_code;            ///< Always error_response (0x01).
    enum op_code    op_code_requested;
    uint16_t        attribute_handle;
    error_code      error;
};

/**
 * @see Bluetooth Core Specification 5.0, Volume 3, Part F
 * 3.4.2.1 Exchange MTU Request, Table 3.4: Format of Exchange MTU Request
 */
struct exchange_mtu_request
{
    enum op_code    op_code;        ///< Always mtu_exchange_request (0x02).
    uint16_t        client_rx_mtu;  ///< Client receive MTU size.
};

/**
 * @see Bluetooth Core Specification 5.0, Volume 3, Part F
 * 3.4.2.2 Exchange MTU Response, Table 3.5: Format of Exchange MTU Response
 */
struct exchange_mtu_response
{
    enum op_code    op_code;        ///< Always mtu_exchange_response (0x03).
    uint16_t        server_rx_mtu;  ///< Attribute server receive MTU size.
};

/**
 * @see Bluetooth Core Specification 5.0, Volume 3, Part F
 * 3.4.3.1 Find Information Request, Table 3.6: Format of Find Information Request
 */
struct find_information_request
{
    enum op_code    op_code;        ///< Always find_information_request (0x04).
    uint16_t        handle_start;   ///< First requested handle.
    uint16_t        handle_end;     ///< Last requested handle (inclusive).
};

/**
 * @see Bluetooth Core Specification 5.0, Volume 3, Part F
 * 3.4.3.2 Find Information Response, Table 3.7: Format of Find Information Response
 */
struct find_information_response
{
    enum class format : uint8_t
    {
        uuid_16     = 0x01,         ///< A list of 1 or more handles with their  16-bit UUIDs
        uuid_128    = 0x02,         ///< A list of 1 or more handles with their 128-bit UUIDs
    };

    enum op_code     op_code;       ///< Always find_information_response (0x05).
    enum format      format;        ///< @see enum find_information_response::format
    void const      *data;          ///< @todo Represent variable length data
    length_t        data_length;    ///< 4 to (ATT_MTU - 2)
};

/**
 * @see Bluetooth Core Specification 5.0, Volume 3, Part F
 * 3.4.3.3 Find By Type Value Request, Table 3.11: Format of Find By Type Value Request
 */
struct find_by_type_value_request
{
    enum op_code    op_code;        ///< Always find_by_type_value_request (0x06).
    uint16_t        handle_start;   ///< First requested handle.
    uint16_t        handle_end;     ///< Last requested handle (inclusive).
    uint16_t        attribute_type; ///< 2-octect UUID to find.
    void const      *data;          ///< Attribute value to find.
    length_t        data_length;    ///< 0 to (ATT_MTU - 7)
};

/**
 * @see Bluetooth Core Specification 5.0, Volume 3, Part F
 * 3.4.3.4 Find By Type Value Response, Table 3.12: Format of Find By Type Value Response
 */
struct find_by_type_value_response
{
    enum op_code    op_code;        ///< Always find_by_type_value_response (0x07).
    void const      *data;          ///< A list of handle informations.
    length_t        data_length;    ///< 4 to (ATT_MTU-1),
};

/**
 * @see Bluetooth Core Specification 5.0, Volume 3, Part F
 * 3.4.4.1 Read By Type Request, Table 3.14: Format of Read By Type Request
 */
struct read_by_type_request
{
    enum op_code    op_code;        ///< Always read_by_type_request (0x08).
    uint16_t        handle_start;   ///< First requested handle.
    uint16_t        handle_end;     ///< Last requested handle (inclusive).
    uint16_t        attribute_type; ///< 2 or 16 octect UUID to find.
                                    ///< @todo Should we represent this with
                                    ///< The type ble::uuid?
};

/**
 * @see Bluetooth Core Specification 5.0, Volume 3, Part F
 * 3.4.4.3 Read Request, Table 3.17: Format of Read Request
 */
struct read_request
{
    enum op_code    op_code;        ///< Always read_request (0x0a).
    uint16_t        handle;         ///< The attribute handle.
};

/**
 * @see Bluetooth Core Specification 5.0, Volume 3, Part F
 * 3.4.4.4 Read Response, Table 3.18: Format of Read Response
 */
struct read_response
{
    enum op_code    op_code;        ///< Always read_response (0x0b).
    void const      *data;          ///< The attribute value.
    length_t        data_length;    ///< 0 to (ATT_MTU-1)
};

/**
 * @see Bluetooth Core Specification 5.0, Volume 3, Part F
 * 3.4.4.5 Read Blob Request, Table 3.19: Format of Read Blob Request
 */
struct read_blob_request
{
    enum op_code    op_code;        ///< Always read_blob_request (0x0c).
    uint16_t        handle;         ///< The attribute handle.
    uint16_t        offset;         ///< Offset into the attribute value to read.
};

/**
 * @see Bluetooth Core Specification 5.0, Volume 3, Part F
 * 3.4.4.6 Read Blob Response, Table 3.20: Format of Read Blob Response
 */
struct read_blob_response
{
    enum op_code     op_code;        ///< Always read_blob_response (0x0d).
    void const      *data;          ///< The attribute value.
    length_t        data_length;    ///< 0 to (ATT_MTU-1)
};

/**
 * @see Bluetooth Core Specification 5.0, Volume 3, Part F
 * 3.4.4.7 Read Multiple Request, Table 3.21: Format of Read Multiple Request
 */
struct read_multiple_request
{
    enum op_code    op_code;        ///< Always read_multiple_request (0x0e).
    void const      *data;          ///< A set of 2 or more attribute handles.
                                    ///< i.e. uint16_t handles[].
    length_t        data_length;    ///< 4 to (ATT_MTU-1)
};

/**
 * @see Bluetooth Core Specification 5.0, Volume 3, Part F
 * 3.4.4.6 Read Blob Response, Table 3.20: Format of Read Blob Response
 */
struct read_multiple_response
{
    enum op_code    op_code;        ///< Always read_multiple_response (0x0f).
    void const      *data;          ///< A concatenation of attribute values.
    length_t        data_length;    ///< 0 to (ATT_MTU-1)
};

/**
 * @see Bluetooth Core Specification 5.0, Volume 3, Part F
 * 3.4.4.9 Read by Group Type Request, Table 3.23: Format of Read By Group Type Request
 */
struct read_by_group_type_request
{
    enum op_code    op_code;        ///< Always read_by_group_type_request (0x10).
    uint16_t        handle_start;   ///< First requested handle.
    uint16_t        handle_end;     ///< Last requested handle (inclusive).
    uint16_t        attribute_type; ///< 2 or 16 octect UUID to find.
                                    ///< @todo Should we represent this with
                                    ///< The type ble::uuid?
};

/**
 * @see Bluetooth Core Specification 5.0, Volume 3, Part F
 * 3.4.4.10 Read by Group Type Response, Table 3.24: Format of Read By Group Type Response
 */
struct read_by_group_type_response
{
    enum op_code    op_code;        ///< Always read_by_group_type_response (0x11).
    uint8_t         length;         ///< The size of each attribute data type.
    void const      *data;          ///< A list of atribute data.
    length_t        data_length;    ///< 4 to (ATT_MTU-2),
};

/**
 * @see Bluetooth Core Specification 5.0, Volume 3, Part F
 * 3.4.5.1 Write Request, Table 3.26: Format of Write Request
 */
struct write_request
{
    enum op_code    op_code;        ///< Always write_request (0x12).
    uint16_t        handle;         ///< The handle of the attribute to write.
    void const      *data;          ///< The attribute data to write.
    length_t        data_length;    ///< 0 to (ATT_MTU-3),
};

/**
 * @see Bluetooth Core Specification 5.0, Volume 3, Part F
 * 3.4.5.2 Write Response, Table 3.27: Format of Write Response
 */
struct write_response
{
    enum op_code    op_code;        ///< Always write_response (0x13).
};

/**
 * @see Bluetooth Core Specification 5.0, Volume 3, Part F
 * 3.4.5.3 Write Command, Table 3.28: Format of Write Command
 */
struct write_command
{
    enum op_code    op_code;        ///< Always write_command (0x52).
    uint16_t        handle;         ///< The handle of the attribute to write.
    void const      *data;          ///< The attribute data to write.
    length_t        data_length;    ///< 0 to (ATT_MTU-3),
};

/**
 * @see Bluetooth Core Specification 5.0, Volume 3, Part F
 * 3.4.5.4 Signed Write Command, Table 3.29: Format of Signed Write Command
 */
struct write_signed_command
{
    enum op_code    op_code;            ///< Always write_signed_command (0xd2).
    uint16_t        handle;             ///< The handle of the attribute to write.
    void const      *data;              ///< The attribute data to write.
    length_t        data_length;        ///< 0 to (ATT_MTU-15),
    void const      *signature;         ///< The authentication signature.
    length_t        signature_length;   ///< Always 12.
};

/**
 * @see Bluetooth Core Specification 5.0, Volume 3, Part F
 * 3.4.6.1 Prepare Write Request, Table 3.30: Format of Prepare Write Request
 */
struct write_prepare_request
{
    enum op_code    op_code;        ///< Always write_prepare_request (0x16).
    uint16_t    handle;             ///< The handle of the attribute to write.
    uint16_t    offset;             ///< The offset into the attribute data.
    void const  *data;              ///< The attribute data to write.
    length_t    data_length;        ///< 0 to (ATT_MTU-5),
};

/**
 * @see Bluetooth Core Specification 5.0, Volume 3, Part F
 * 3.4.6.2 Prepare Write Response, Table 3.31: Format of Prepare Write Response
 */
struct write_prepare_response
{
    enum op_code    op_code;        ///< Always write_prepare_response (0x17).
    uint16_t        handle;         ///< The handle of the attribute to write.
    uint16_t        offset;         ///< The offset into the attribute data.
    void const      *data;          ///< The attribute data to write.
    length_t        data_length;    ///< 0 to (ATT_MTU-5),
};

/**
 * @see Bluetooth Core Specification 5.0, Volume 3, Part F
 * 3.4.6.3 Execute Write Request, Table 3.32: Format of Execute Write Request
 */
struct write_execute_request
{
    enum class flags: uint8_t
    {
        cancel = 0x00,              ///< 0x00: Cancel prepared write operation.
        commit = 0x01,              ///< 0x01: Commit prepared write operation.
    };

    enum op_code    op_code;        ///< Always write_execute_request (0x18).
    flags           flags;
};

static_assert(sizeof (write_execute_request) == 2u);

/**
 * @see Bluetooth Core Specification 5.0, Volume 3, Part F
 * 3.4.6.4 Execute Write Response, Table 3.33: Format of Execute Write Response
 */
struct write_execute_response
{
    enum op_code    op_code;        ///< Always write_execute_response (0x19).
};

/**
 * @see Bluetooth Core Specification 5.0, Volume 3, Part F
 * 3.4.7.1 Handle Value Notification, Table 3.34: Format of Handle Value Notification
 */
struct handle_value_notification
{
    enum op_code    op_code;        ///< Always handle_value_notification (0x1b).
    uint16_t        handle;         ///< The attribute of the handle.
    void const      *data;          ///< The attribute data value.
    length_t        data_length;    ///< 0 to (ATT_MTU-3),
};

/**
 * @see Bluetooth Core Specification 5.0, Volume 3, Part F
 * 3.4.7.2 Handle Value Indication, Table 3.35: Format of Handle Value Indication
 */
struct handle_value_indication
{
    enum op_code    op_code;        ///< Always handle_value_indication (0x1d).
    uint16_t        handle;         ///< The attribute of the handle.
    void const      *data;          ///< The attribute data value.
    length_t        data_length;    ///< 0 to (ATT_MTU-3),
};

/**
 * @see Bluetooth Core Specification 5.0, Volume 3, Part F
 * 3.4.7.3 Handle Value Confirmation, Table 3.36: Format of Handle Value Confirmation
 */
struct handle_value_confirmation
{
    enum op_code    op_code;        ///< Always handle_value_indication (0x1e).
};

} // namespace att
} // namespace ble

