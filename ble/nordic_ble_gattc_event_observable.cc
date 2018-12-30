/**
 * @file nordic_ble_gattc_event_observable.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Provide attachment and notifications for the ble::gap::event_observer class
 * into the Nordic BLE observables.
 */

#include "ble/att.h"
#include "ble/nordic_ble_att.h"
#include "ble/nordic_ble_event_observable.h"
#include "ble/nordic_ble_event_observer.h"
#include "ble/gattc_event_observer.h"

#include "ble_gatt.h"                       // Nordic softdevice
#include "logger.h"
#include "cmsis_gcc.h"

/**
 * @todo Check list:
 * + Check on uuid 16 bit and 128 bit reverse() requirement and usage when
 *   converting from Nordic uuid types ble_uuid_t and ble_uuid128_t.
 */
namespace nordic
{

template<>
void ble_event_observable<ble_gattc_event_observer>::notify(
    ble_gattc_event_observer::event_enum_t event_type,
    ble_gattc_event_observer::event_data_t const&  event_data)
{
    logger &logger = logger::instance();

    ble::att::error_code const error_code = nordic::to_att_error_code(event_data.gatt_status);

    for (auto observer_iter  = this->observer_list_.begin();
              observer_iter != this->observer_list_.end(); )
    {
        // Increment the iterator prior to using it.
        // If the client removes itself during the completion callback
        // then the iterator will be valid and can continue.
        ble_gattc_event_observer &observer = *observer_iter;
        ++observer_iter;

        switch (event_type)
        {
        case BLE_GATTC_EVT_PRIM_SRVC_DISC_RSP:
            // Primary Service Discovery Response event.
            // See ble_gattc_evt_prim_srvc_disc_rsp_t.
            for (uint16_t iter = 0u;
                 iter < event_data.params.prim_srvc_disc_rsp.count; ++iter)
            {
                ble_gattc_service_t const* service =
                    &event_data.params.prim_srvc_disc_rsp.services[iter];

                ble::att::uuid const uuid = nordic::to_att_uuid(service->uuid);

                char uuid_char_buffer[ble::att::uuid::conversion_length];
                uuid.to_chars(std::begin(uuid_char_buffer), std::end(uuid_char_buffer));

                logger.info("BLE_GATTC_EVT_PRIM_SRVC_DISC_RSP[0x%04, 0x%04x]: %s",
                            service->handle_range.start_handle,
                            service->handle_range.end_handle,
                            uuid_char_buffer);

                observer.interface_reference.service_discovery_response(
                    event_data.conn_handle,
                    error_code,
                    event_data.error_handle,
                    service->handle_range.start_handle,
                    service->handle_range.end_handle,
                    uuid);
            }
            break;

        case BLE_GATTC_EVT_REL_DISC_RSP:
            // Relationship Discovery Response event.
            // See ble_gattc_evt_rel_disc_rsp_t.
            for (uint16_t iter = 0u;
                 iter < event_data.params.rel_disc_rsp.count;++iter)
            {
                ble_gattc_include_t const* include_disc_rsp =
                    &event_data.params.rel_disc_rsp.includes[iter];

                ble_gattc_service_t const* service = &include_disc_rsp->included_srvc;

                ble::att::uuid const uuid = nordic::to_att_uuid(service->uuid);

                char uuid_char_buffer[ble::att::uuid::conversion_length];
                uuid.to_chars(std::begin(uuid_char_buffer), std::end(uuid_char_buffer));

                logger.info("BLE_GATTC_EVT_REL_DISC_RSP[0x%04, 0x%04x]: incl: 0x%04x, %s",
                            service->handle_range.start_handle,
                            service->handle_range.end_handle,
                            include_disc_rsp->handle,
                            uuid_char_buffer);

                observer.interface_reference.relationship_discovery_response(
                    event_data.conn_handle,
                    error_code,
                    event_data.error_handle,
                    service->handle_range.start_handle,
                    service->handle_range.end_handle,
                    include_disc_rsp->handle,
                    uuid);
            }
            break;

        case BLE_GATTC_EVT_CHAR_DISC_RSP:
            // Characteristic Discovery Response event.
            // See ble_gattc_evt_char_disc_rsp_t.
            for (uint16_t iter = 0u;
                 iter < event_data.params.char_disc_rsp.count; ++iter)
            {
                ble_gattc_char_t const* char_disc_rsp =
                    &event_data.params.char_disc_rsp.chars[iter];

                ble::att::uuid const uuid = nordic::to_att_uuid(char_disc_rsp->uuid);

                char uuid_char_buffer[ble::att::uuid::conversion_length];
                uuid.to_chars(std::begin(uuid_char_buffer), std::end(uuid_char_buffer));

                ble::gatt::properties const properties = nordic::to_att_properties(
                    char_disc_rsp->char_props);

                logger.info("BLE_GATTC_EVT_CHAR_DISC_RSP: "
                            "decl: 0x%04x, value: 0x%04x, props: 0x%04x, %s",
                            char_disc_rsp->handle_decl,
                            char_disc_rsp->handle_value,
                            properties.get(),
                            uuid_char_buffer);

                observer.interface_reference.characteristic_discovery_response(
                    event_data.conn_handle,
                    error_code,
                    event_data.error_handle,
                    char_disc_rsp->handle_decl,
                    char_disc_rsp->handle_value,
                    uuid,
                    properties);
            }
            break;

        case BLE_GATTC_EVT_DESC_DISC_RSP:
            // Descriptor Discovery Response event.
            // See ble_gattc_evt_desc_disc_rsp_t.
            for (uint16_t iter = 0u;
                 iter < event_data.params.desc_disc_rsp.count; ++iter)
            {
                ble_gattc_desc_t const* desc_disc_rsp =
                    &event_data.params.desc_disc_rsp.descs[iter];

                ble::att::uuid const uuid = nordic::to_att_uuid(desc_disc_rsp->uuid);

                char uuid_char_buffer[ble::att::uuid::conversion_length];
                uuid.to_chars(std::begin(uuid_char_buffer), std::end(uuid_char_buffer));

                logger.info("BLE_GATTC_EVT_CHAR_DISC_RSP[0x%04]: %s",
                            desc_disc_rsp->handle, uuid_char_buffer);

                observer.interface_reference.descriptor_discovery_response(
                    event_data.conn_handle,
                    error_code,
                    event_data.error_handle,
                    desc_disc_rsp->handle,
                    uuid);
            }
            break;

        case BLE_GATTC_EVT_ATTR_INFO_DISC_RSP:
            // Attribute Information Response event.
            // See ble_gattc_evt_attr_info_disc_rsp_t.
            switch (event_data.params.attr_info_disc_rsp.format)
            {
            case BLE_GATTC_ATTR_INFO_FORMAT_16BIT:
                for (uint16_t iter = 0u;
                     iter < event_data.params.attr_info_disc_rsp.count; ++iter)
                {
                    ble_gattc_attr_info16_t const* gattc_attr =
                        &event_data.params.attr_info_disc_rsp.info.attr_info16[iter];

                    ble::att::uuid uuid(gattc_attr->uuid.uuid);

                    // char uuid_char_buffer[ble::att::uuid::conversion_length];
                    // uuid.to_chars(std::begin(uuid_char_buffer), std::end(uuid_char_buffer));

                    logger.info("BLE_GATTC_EVT_ATTR_INFO_DISC_RSP [0x%04]: 0x%04x",
                                gattc_attr->handle, gattc_attr->uuid);

                    observer.interface_reference.attribute_uuid_discovery_response(
                        event_data.conn_handle,
                        error_code,
                        event_data.error_handle,
                        gattc_attr->handle,
                        uuid);
                }
                break;

            case BLE_GATTC_ATTR_INFO_FORMAT_128BIT:
                for (uint16_t iter = 0u;
                     iter < event_data.params.attr_info_disc_rsp.count; ++iter)
                {
                    ble_gattc_attr_info128_t const* gattc_attr =
                        &event_data.params.attr_info_disc_rsp.info.attr_info128[iter];

                    ble::att::uuid uuid(gattc_attr->uuid.uuid128);

                    char uuid_char_buffer[ble::att::uuid::conversion_length];
                    uuid.to_chars(std::begin(uuid_char_buffer), std::end(uuid_char_buffer));

                    logger.info("BLE_GATTC_EVT_ATTR_INFO_DISC_RSP [0x%04]: %s",
                                gattc_attr->handle, uuid);

                    observer.interface_reference.attribute_uuid_discovery_response(
                        event_data.conn_handle,
                        error_code,
                        event_data.error_handle,
                        gattc_attr->handle,
                        uuid);
                }
                break;

            default:
                logger.error("unknown Nordic attribute uuid discovery format: %u",
                             event_data.params.attr_info_disc_rsp.format);
                ASSERT(0);
                break;
            }
            break;

        case BLE_GATTC_EVT_CHAR_VAL_BY_UUID_READ_RSP:
            // Read By UUID Response event.
            // See ble_gattc_evt_char_val_by_uuid_read_rsp_t.
            {
                ble_gattc_evt_char_val_by_uuid_read_rsp_t const &read_rsp =
                    event_data.params.char_val_by_uuid_read_rsp;

                // Data is held in (handle, value) pairs. The length in bytes
                // of the value is value_length.
                uint32_t const  value_length = read_rsp.value_len;
                uint8_t  const* hv_pair_ptr  = read_rsp.handle_value;

                for (uint16_t iter = 0u;
                     iter < event_data.params.char_val_by_uuid_read_rsp.count; ++iter)
                {
                    uint16_t handle = *hv_pair_ptr++;
                    handle <<= 8u;
                    handle |= *hv_pair_ptr++;
                    handle = __REV16(handle);   // 16-bit endian swap.

                    observer.interface_reference.read_characteristic_by_uuid_response(
                        event_data.conn_handle,
                        error_code,
                        event_data.error_handle,
                        handle,
                        &hv_pair_ptr,
                        value_length);

                    hv_pair_ptr += value_length;
                }
            }
            break;

        case BLE_GATTC_EVT_READ_RSP:
            // Read Response event.
            // See ble_gattc_evt_read_rsp_t.
            observer.interface_reference.read_response(
                event_data.conn_handle,
                error_code,
                event_data.error_handle,
                event_data.params.read_rsp.handle,
                event_data.params.read_rsp.data,
                event_data.params.read_rsp.offset,
                event_data.params.read_rsp.len);
            break;

        case BLE_GATTC_EVT_CHAR_VALS_READ_RSP:
            // Read multiple Response event.
            // See ble_gattc_evt_char_vals_read_rsp_t.
            observer.interface_reference.read_multi_response(
                event_data.conn_handle,
                error_code,
                event_data.error_handle,
                event_data.params.char_vals_read_rsp.values,
                event_data.params.char_vals_read_rsp.len);
            break;

        case BLE_GATTC_EVT_WRITE_RSP:
            // Write Response event.
            // See ble_gattc_evt_write_rsp_t.
            observer.interface_reference.write_response(
                event_data.conn_handle,
                error_code,
                event_data.error_handle,
                nordic::to_att_write_op_code(event_data.params.write_rsp.write_op),
                event_data.params.write_rsp.handle,
                event_data.params.write_rsp.data,
                event_data.params.write_rsp.offset,
                event_data.params.write_rsp.len);
            break;

        case BLE_GATTC_EVT_HVX:
            // Handle Value Notification or Indication event.
            // Confirm indication with sd_ble_gattc_hv_confirm.
            // See ble_gattc_evt_hvx_t.
            switch (event_data.params.hvx.type)
            {
            case BLE_GATT_HVX_NOTIFICATION:
                observer.interface_reference.handle_notification(
                    event_data.conn_handle,
                    error_code,
                    event_data.error_handle,
                    event_data.params.hvx.handle,
                    event_data.params.hvx.data,
                    event_data.params.hvx.len);
                break;

            case BLE_GATT_HVX_INDICATION:
                observer.interface_reference.handle_indication(
                    event_data.conn_handle,
                    error_code,
                    event_data.error_handle,
                    event_data.params.hvx.handle,
                    event_data.params.hvx.data,
                    event_data.params.hvx.len);
                break;

            default:
                logger.error("BLE_GATTC_EVT_HVX: unknown type: %u",
                             event_data.params.hvx.type);
                break;
            }
            break;

        case BLE_GATTC_EVT_EXCHANGE_MTU_RSP:
            // Exchange MTU Response event.
            // See ble_gattc_evt_exchange_mtu_rsp_t.
            observer.interface_reference.exchange_mtu_response(
                event_data.conn_handle,
                error_code,
                event_data.error_handle,
                event_data.params.exchange_mtu_rsp.server_rx_mtu);
            break;

        case BLE_GATTC_EVT_TIMEOUT:
            // Timeout event.
            // See ble_gattc_evt_timeout_t.
            observer.interface_reference.timeout(
                event_data.conn_handle,
                error_code,
                event_data.error_handle);
            break;

        case BLE_GATTC_EVT_WRITE_CMD_TX_COMPLETE:
            // Write without Response transmission complete.
            // See ble_gattc_evt_write_cmd_tx_complete_t.
            observer.interface_reference.write_command_tx_completed(
                event_data.conn_handle,
                error_code,
                event_data.error_handle,
                event_data.params.write_cmd_tx_complete.count);
            break;

        default:
            break;
        }

    }
}

} // namespace nordic
