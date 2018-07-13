/**
 * @file nordic_ble_gatts_event_observable.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Provide attachment and notifications for the ble::gatts::event_observer class
 * into the Nordic BLE observables.
 */

#include "ble/nordic_ble_event_observable.h"
#include "ble/nordic_ble_event_observer.h"
#include "ble/gatts_event_observer.h"
#include "ble/att.h"

#include "ble_gatt.h"                       // Nordic softdevice header
#include "logger.h"

namespace nordic
{

ble::att::op_code nordic_write_type_opcode(uint8_t write_type)
{
    switch (write_type)
    {
    case BLE_GATTS_OP_WRITE_REQ:                return ble::att::op_code::write_request;
    case BLE_GATTS_OP_WRITE_CMD:                return ble::att::op_code::write_command;
    case BLE_GATTS_OP_SIGN_WRITE_CMD:           return ble::att::op_code::write_signed_command;
    case BLE_GATTS_OP_PREP_WRITE_REQ:           return ble::att::op_code::write_prepare_request;
    case BLE_GATTS_OP_EXEC_WRITE_REQ_CANCEL:    return ble::att::op_code::write_execute_request;
    case BLE_GATTS_OP_EXEC_WRITE_REQ_NOW:       return ble::att::op_code::write_execute_request;
    default:
        break;
    }

    logger::instance().error("unhandled nordic write type: %u", write_type);
    return static_cast<ble::att::op_code>(0u);
}

template<>
void ble_event_observable<ble_gatts_event_observer>::notify(
    ble_gatts_event_observer::event_enum_t event_type,
    ble_gatts_event_observer::event_data_t const&  event_data)
{
    logger &logger = logger::instance();

    for (auto observer_iter  = this->observer_list_.begin();
              observer_iter != this->observer_list_.end(); )
    {
        // Increment the iterator prior to using it.
        // If the client removes itself during the completion callback
        // then the iterator will be valid and can continue.
        ble_gatts_event_observer &observer = *observer_iter;
        ++observer_iter;

        switch (event_type)
        {
        case BLE_GATTS_EVT_WRITE:
            // Write operation performed.
            // See @ref ble_gatts_evt_write_t.
            logger::instance().debug(
                "GATTS write: c: 0x%04x, h: 0x%04x, u: 0x%04x, o: %u, ar: %u, off: %u, len: %u",
                 event_data.conn_handle,
                 event_data.params.write.handle,
                 event_data.params.write.uuid.uuid,
                 nordic_write_type_opcode(event_data.params.write.op),
                 bool(event_data.params.write.auth_required),
                 event_data.params.write.offset,
                 event_data.params.write.len);

            if (event_data.params.write.op == BLE_GATTS_OP_EXEC_WRITE_REQ_CANCEL)
            {
                observer.interface_reference.write_cancel(
                    event_data.conn_handle,
                    event_data.params.write.handle,
                    nordic_write_type_opcode(event_data.params.write.op),
                    bool(event_data.params.write.auth_required),
                    event_data.params.write.offset,
                    event_data.params.write.len,
                    event_data.params.write.data);
            }
            else
            {
                observer.interface_reference.write(
                    event_data.conn_handle,
                    event_data.params.write.handle,
                    nordic_write_type_opcode(event_data.params.write.op),
                    bool(event_data.params.write.auth_required),
                    event_data.params.write.offset,
                    event_data.params.write.len,
                    event_data.params.write.data);
            }
            break;

        case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
            // Reply with @ref sd_ble_gatts_rw_authorize_reply.
            if (event_data.params.authorize_request.type == BLE_GATTS_AUTHORIZE_TYPE_READ)
            {
                logger::instance().debug(
                    "GATTS rd_ar: c: 0x%04x, h: 0x%04x, u: 0x%04x, o: %u",
                    event_data.conn_handle,
                    event_data.params.write.handle,
                    event_data.params.write.uuid.uuid,
                    event_data.params.write.offset);

                observer.interface_reference.read_authorization_request(
                    event_data.conn_handle,
                    event_data.params.authorize_request.request.read.handle,
                    event_data.params.authorize_request.request.read.offset);
            }
            else if (event_data.params.authorize_request.type == BLE_GATTS_AUTHORIZE_TYPE_READ)
            {
                logger::instance().debug(
                    "GATTS wr_ar: c: 0x%04x, h: 0x%04x, u: 0x%04x, o: %u, ar: %u, off: %u, len: %u",
                    event_data.conn_handle,
                    event_data.params.write.handle,
                    event_data.params.write.uuid.uuid,
                    nordic_write_type_opcode(event_data.params.write.op),
                    bool(event_data.params.write.auth_required),
                    event_data.params.write.offset,
                    event_data.params.write.len);
                observer.interface_reference.write_authorization_request(
                    event_data.conn_handle,
                    event_data.params.authorize_request.request.write.handle,
                    nordic_write_type_opcode(event_data.params.authorize_request.request.write.op),
                    bool(event_data.params.authorize_request.request.write.auth_required),
                    event_data.params.authorize_request.request.write.offset,
                    event_data.params.authorize_request.request.write.len,
                    event_data.params.authorize_request.request.write.data);
            }
            else
            {
                logger.error("invalid authorization request type: %u",
                             event_data.params.authorize_request.type);
            }
            break;

        case BLE_GATTS_EVT_SYS_ATTR_MISSING:
            // A persistent system attribute access is pending or missing
            // in the persistent storage.
            // See @ref ble_gatts_evt_sys_attr_missing_t.
            // Respond with @ref sd_ble_gatts_sys_attr_set.

            // This call is only allowed for active connections, and is usually
            // made immediately after a connection is established
            // with an known bonded device.

            logger::instance().debug(
                "BLE_GATTS_EVT_SYS_ATTR_MISSING: c: 0x%04x, hint: 0x%02x",
                event_data.conn_handle,
                event_data.params.sys_attr_missing.hint);

            observer.interface_reference.system_attribute_missing(
                event_data.conn_handle,
                event_data.params.sys_attr_missing.hint);
            break;

        case BLE_GATTS_EVT_HVC:
            // Handle Value Confirmation.
            logger::instance().debug(
                "GATTS handle value confirmation: c: 0x%04x, h: 0x%04x",
                event_data.conn_handle,
                event_data.params.hvc.handle);

            observer.interface_reference.handle_value_confirmation(
                event_data.conn_handle,
                event_data.params.hvc.handle);
            break;

        case BLE_GATTS_EVT_SC_CONFIRM:
            // Service Changed Confirmation.
            // No additional event structure applies.
            logger::instance().debug(
                "GATTS service change confirmation: c: 0x%04x",
                event_data.conn_handle);

            observer.interface_reference.service_change_confirmation(
                event_data.conn_handle);
            break;

        case BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST:
            // Exchange MTU Request.
            // Reply with @ref sd_ble_gatts_exchange_mtu_reply.
            logger::instance().debug(
                "GATTS service change confirmation: c: 0x%04x, rx_mtx: %u",
                event_data.conn_handle,
                event_data.params.exchange_mtu_request.client_rx_mtu);

            observer.interface_reference.mtu_rx_size(
                event_data.conn_handle,
                event_data.params.exchange_mtu_request.client_rx_mtu);
            break;

        case BLE_GATTS_EVT_TIMEOUT:
            // Peer failed to respond to an ATT request in time.
            logger::instance().debug(
                "GATTS timeout: c: 0x%04x, source: %u",
                event_data.conn_handle,
                event_data.params.timeout.src);

            observer.interface_reference.mtu_rx_size(
                event_data.conn_handle,
                event_data.params.timeout.src);
            break;

        case BLE_GATTS_EVT_HVN_TX_COMPLETE:
            // Handle Value Notification transmission complete.
            logger::instance().debug(
                "GATTS hvn tx completed: c: 0x%04x, n: %u",
                event_data.conn_handle,
                event_data.params.hvn_tx_complete.count);

            observer.interface_reference.handle_value_notifications_tx_completed(
                event_data.conn_handle,
                event_data.params.hvn_tx_complete.count);
            break;

        default:
            logger.warn("unhandled GATTS event: %u", event_type);
            break;
        }
    }
}

} // namespace nordic
