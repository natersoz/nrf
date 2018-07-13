/**
 * @file nordic_ble_gattc_event_observable.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Provide attachment and notifications for the ble::gap::event_observer class
 * into the Nordic BLE observables.
 */

#include "ble/nordic_ble_event_observable.h"
#include "ble/nordic_ble_event_observer.h"
#include "ble/gattc_event_observer.h"

#include "ble_gatt.h"                       // Nordic softdevice header
#include "logger.h"

namespace nordic
{

template<>
void ble_event_observable<ble_gattc_event_observer>::notify(
    ble_gattc_event_observer::event_enum_t event_type,
    ble_gattc_event_observer::event_data_t const&  event_data)
{
    for (auto observer_iter  = this->observer_list_.begin();
              observer_iter != this->observer_list_.end(); )
    {
        // Increment the iterator prior to using it.
        // If the client removes itself during the completion callback
        // then the iterator will be valid and can continue.
        auto &observer = *observer_iter;
        ++observer_iter;

        (void) observer;        /// @todo Remove once implementation is done.

        switch (event_type)
        {
        case BLE_GATTC_EVT_PRIM_SRVC_DISC_RSP:
            // Primary Service Discovery Response event.
            // See @ref ble_gattc_evt_prim_srvc_disc_rsp_t.
            break;
        case BLE_GATTC_EVT_REL_DISC_RSP:
            // Relationship Discovery Response event.
            // See @ref ble_gattc_evt_rel_disc_rsp_t.
            break;
        case BLE_GATTC_EVT_CHAR_DISC_RSP:
            // Characteristic Discovery Response event.
            // See @ref ble_gattc_evt_char_disc_rsp_t.
            break;
        case BLE_GATTC_EVT_DESC_DISC_RSP:
            // Descriptor Discovery Response event.
            // See @ref ble_gattc_evt_desc_disc_rsp_t.
            break;
        case BLE_GATTC_EVT_ATTR_INFO_DISC_RSP:
            // Attribute Information Response event.
            // See @ref ble_gattc_evt_attr_info_disc_rsp_t.
            break;
        case BLE_GATTC_EVT_CHAR_VAL_BY_UUID_READ_RSP:
            // Read By UUID Response event.
            // See @ref ble_gattc_evt_char_val_by_uuid_read_rsp_t.
            break;
        case BLE_GATTC_EVT_READ_RSP:
            // Read Response event.
            // See @ref ble_gattc_evt_read_rsp_t.
            break;
        case BLE_GATTC_EVT_CHAR_VALS_READ_RSP:
            // Read multiple Response event.
            // See @ref ble_gattc_evt_char_vals_read_rsp_t.
            break;
        case BLE_GATTC_EVT_WRITE_RSP:
            // Write Response event.
            // See @ref ble_gattc_evt_write_rsp_t.
            break;
        case BLE_GATTC_EVT_HVX:
            // Handle Value Notification or Indication event.
            // Confirm indication with @ref sd_ble_gattc_hv_confirm.
            // See @ref ble_gattc_evt_hvx_t.
            break;
        case BLE_GATTC_EVT_EXCHANGE_MTU_RSP:
            // Exchange MTU Response event.
            // See @ref ble_gattc_evt_exchange_mtu_rsp_t.
            break;
        case BLE_GATTC_EVT_TIMEOUT:
            // Timeout event.
            // See @ref ble_gattc_evt_timeout_t.
            break;
        case BLE_GATTC_EVT_WRITE_CMD_TX_COMPLETE:
            // Write without Response transmission complete.
            // See @ref ble_gattc_evt_write_cmd_tx_complete_t.
            break;
        default:
            break;
        }

    }
}

} // namespace nordic
