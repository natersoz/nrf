/**
 * @file ble/gatts_event_observer.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Generic GATTS BLE event observer default implementation.
 */

#include "ble/profile_connectable.h"
#include "ble/gatts_event_observer.h"
#include "ble/gatts_operations.h"
#include "logger.h"
#include "project_assert.h"

namespace ble
{
namespace gatts
{

/**
 * BLE_GATTS_EVT_WRITE
 *
 * @todo @ref sd_ble_evt_get for more information on how to
 * use event structures with variable length array members.
 *
 * @todo authorization_required parameter needs to be properly handled.
 */
void event_observer::write(uint16_t            conection_handle,
                           uint16_t            attribute_handle,
                           att::op_code        write_operation_type,
                           bool                authorization_required,
                           att::length_t       offset,
                           att::length_t       length,
                           void const*         data)
{
    logger& logger = logger::instance();

    ble::profile::connectable* connectable = this->get_connecteable();
    if (not connectable)
    {
        ASSERT(0);
        return;
    }

    if (connectable->gap.get_connection_handle() != conection_handle)
    {
        /// This GATTS request is from a different connection.
        /// @todo Is it normal for this connection to get notified of other
        /// connection events? Not sure at this point; warn for now:
        logger.warn("GATTS write(0x%04x, 0x%04x): wrong connection handle",
                    conection_handle, attribute_handle);
        return;
    }

    ble::gatt::characteristic *characteristic =
        connectable->service_container.find_characteristic(attribute_handle);
    if (not characteristic)
    {
        logger.warn("GATTS write(0x%04x, 0x%04x): invalid handle",
                    conection_handle, attribute_handle);
        return;
    }

    if (not characteristic->decl.properties.is_writable())
    {
        logger.warn("GATTS write(0x%04x, 0x%04x): not writeable",
                    conection_handle, attribute_handle);
    }

    ble::att::length_t const written = characteristic->write(
        write_operation_type, offset, length, data);

    if (written != length)
    {
        logger.warn("GATTS write(0x%04x, 0x%04x): write length: %u / %u",
                    conection_handle, attribute_handle, written, length);
    }
}

// BLE_GATTS_EVT_WRITE
void event_observer::write_cancel(uint16_t          conection_handle,
                                  uint16_t          attribute_handle,
                                  att::op_code      write_operation_type,
                                  bool              authorization_required,
                                  att::length_t     offset,
                                  att::length_t     length,
                                  void const*       data)
{
    ble::profile::connectable* connectable = this->get_connecteable();
    if (not connectable)
    {
        ASSERT(0);
        return;
    }

    if (connectable->gap.get_connection_handle() != conection_handle)
    {
        // This GATTS request is from a different connection.
        return;
    }

    /// @todo implementation
}

// BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST
void event_observer::read_authorization_request(uint16_t      conection_handle,
                                                uint16_t      attribute_handle,
                                                att::length_t offset)
{
    ble::profile::connectable* connectable = this->get_connecteable();
    if (not connectable)
    {
        ASSERT(0);
        return;
    }

    if (connectable->gap.get_connection_handle() != conection_handle)
    {
        // This GATTS request is from a different connection.
        return;
    }

    /// @todo implementation
}

// BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST
void event_observer::write_authorization_request(
    uint16_t      conection_handle,
    uint16_t      attribute_handle,
    att::op_code  write_operation_type,
    bool          authorization_required,
    att::length_t offset,
    att::length_t length,
    void const*   data)
{
    ble::profile::connectable* connectable = this->get_connecteable();
    if (not connectable)
    {
        ASSERT(0);
        return;
    }

    if (connectable->gap.get_connection_handle() != conection_handle)
    {
        // This GATTS request is from a different connection.
        return;
    }

    /// @todo implementation
}

// BLE_GATTS_EVT_SC_CONFIRM
void event_observer::service_change_confirmation(uint16_t conection_handle)
{
}

// BLE_GATTS_EVT_HVC
void event_observer::handle_value_confirmation(uint16_t conection_handle,
                                               uint16_t attribute_handle)
{
}

/**
 * BLUETOOTH SPECIFICATION Version 5.0 | Vol 3, Part F
 * Section 3.4.2.1 Exchange MTU Request
 * Table 3.4: Format of Exchange MTU Request
 *
 * By default, accept the central's MTU request.
 */
void event_observer::exchange_mtu_request(uint16_t      conection_handle,
                                          att::length_t client_rx_mtu_size)
{
    ble::profile::connectable* connectable = this->get_connecteable();
    if (not connectable)
    {
        ASSERT(0);
        return;
    }

    if (connectable->gap.get_connection_handle() != conection_handle)
    {
        // This GATTS request is from a different connection.
        return;
    }

    ble::att::length_t const att_mtu_length_maximum =
        connectable->stack.get_constraints().att_mtu_maximum_length;

    client_rx_mtu_size = std::min(client_rx_mtu_size, att_mtu_length_maximum);
    client_rx_mtu_size = std::max(client_rx_mtu_size, att::mtu_length_minimum);

    connectable->gatts.operations->exchange_mtu_reply(conection_handle,
                                                      client_rx_mtu_size);
}

// BLE_GATTS_EVT_TIMEOUT, // always BLE_GATT_TIMEOUT_SRC_PROTOCOL (0)
void event_observer::timeout(uint16_t conection_handle, uint8_t timeout_source)
{
    ble::profile::connectable* connectable = this->get_connecteable();
    if (not connectable)
    {
        ASSERT(0);
        return;
    }

    if (connectable->gap.get_connection_handle() != conection_handle)
    {
        // This GATTS request is from a different connection.
        return;
    }

    /// @todo implementation

}

// BLE_GATTS_EVT_HVN_TX_COMPLETE
void event_observer::handle_value_notifications_tx_completed(
    uint16_t conection_handle, uint8_t  count)
{
    ble::profile::connectable* connectable = this->get_connecteable();
    if (not connectable)
    {
        ASSERT(0);
        return;
    }

    if (connectable->gap.get_connection_handle() != conection_handle)
    {
        // This GATTS request is from a different connection.
        return;
    }

    /// @todo implementation

}

} // namespace gatts
} // namespace ble

