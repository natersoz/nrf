/**
 * @file ble/gattc_event_observer.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "ble/att.h"
#include "ble/uuid.h"
#include "ble/gatt_declaration.h"

namespace ble
{
namespace gattc
{

struct service_info
{
    ble::att::uuid                      uuid;
    ble::att::find_information_request  info;
};

struct characteristic_info
{
    ble::att::uuid                      uuid;
    ble::gatt::properties               properties;
    uint16_t                            handle_declaration;
    uint16_t                            handle_value;
};

struct attribute_info
{
    ble::att::uuid                      uuid;
    uint16_t                            handle;
};

class event_observer
{
public:
    virtual ~event_observer()                        = default;

    event_observer()                                 = default;
    event_observer(event_observer const&)            = delete;
    event_observer(event_observer &&)                = delete;
    event_observer& operator=(event_observer const&) = delete;
    event_observer& operator=(event_observer&&)      = delete;

    virtual void service_discovery_response(
        uint16_t                    conection_handle,
        service_info const*         services,
        ble::att::length_t          services_count
        ) {} // BLE_GATTC_EVT_PRIM_SRVC_DISC_RSP

    virtual void relationship_discovery_response(
        uint16_t                    conection_handle,
        service_info const*         services,
        ble::att::length_t          services_count
        ) {} // BLE_GATTC_EVT_REL_DISC_RSP

    virtual void characteristic_discovery_response(
        uint16_t                    conection_handle,
        characteristic_info const*  characteristics,
        ble::att::length_t          characteristics_count
        ) {} // BLE_GATTC_EVT_CHAR_DISC_RSP

    virtual void attribute_uuid_discovery_response(
        uint16_t                    conection_handle,
        attribute_info const*       attributes,
        ble::att::length_t          attributes_count
        ) {} // BLE_GATTC_EVT_ATTR_INFO_DISC_RSP

    virtual void read_characteristic_by_uuid_response(
        uint16_t                    conection_handle,
        void const*                 handle_value_pairs,
        ble::att::length_t          handle_value_count
        ) {} // BLE_GATTC_EVT_CHAR_VAL_BY_UUID_READ_RSP

    virtual void read_response(
        uint16_t                    conection_handle,
        uint16_t                    attribute_handle,
        void const*                 data,
        ble::att::length_t          offset,
        ble::att::length_t          length
        ) {} // BLE_GATTC_EVT_READ_RSP

    virtual void read_multi_response(
        uint16_t                    conection_handle,
        void const*                 data,
        ble::att::length_t          length
        ) {} // BLE_GATTC_EVT_CHAR_VALS_READ_RSP

    virtual void write_response(
        uint16_t                    conection_handle,
        uint16_t                    attribute_handle,
        void const*                 data,
        ble::att::length_t          offset,
        ble::att::length_t          length
        ) {} // BLE_GATTC_EVT_WRITE_RSP

    virtual void handle_notification(
        uint16_t                    conection_handle,
        uint16_t                    attribute_handle,
        void const*                 data,
        ble::att::length_t          length
        ) {} // BLE_GATTC_EVT_HVX

    virtual void handle_indication(
        uint16_t                    conection_handle,
        uint16_t                    attribute_handle,
        void const*                 data,
        ble::att::length_t          length
        ) {} // BLE_GATTC_EVT_HVX

    virtual void mtu_rx_size(
        uint16_t                    conection_handle,
        uint16_t                    server_rx_mtu_size
        ) {} // BLE_GATTC_EVT_EXCHANGE_MTU_RSP

    virtual void timeout(
        uint16_t                    conection_handle,
        uint8_t                     timeout_source
        ) {} // BLE_GATTC_EVT_TIMEOUT

    virtual void write_command_tx_completed(
        uint16_t                    conection_handle,
        uint8_t                     count
        ) {} // BLE_GATTC_EVT_WRITE_CMD_TX_COMPLETE
};

} // namespace gattc
} // namespace ble

