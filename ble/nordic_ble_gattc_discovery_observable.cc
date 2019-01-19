/**
 * @file nordic_ble_gattc_discovery_observable.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Provide attachment and notifications for the ble::gattc::discovery_observer
 * class into the Nordic BLE observables.
 */

#include "ble/att.h"
#include "ble/nordic_ble_att.h"
#include "ble/nordic_ble_event_observable.h"
#include "ble/nordic_ble_event_observer.h"

#include "ble_gatt.h"
#include "nordic_error.h"
#include "logger.h"
#include "cmsis_gcc.h"

namespace nordic
{

template<>
void ble_event_observable<ble_gattc_discovery_observer>::notify(
    ble_gattc_discovery_observer::event_enum_t event_type,
    ble_gattc_discovery_observer::event_data_t const&  event_data)
{
    logger &logger = logger::instance();

    for (auto observer_iter  = this->observer_list_.begin();
              observer_iter != this->observer_list_.end(); )
    {
        // Increment the iterator prior to using it.
        // If the client removes itself during the completion callback
        // then the iterator will be valid and can continue.
        ble_gattc_discovery_observer &observer = *observer_iter;
        ++observer_iter;

        switch (event_type)
        {
        case BLE_GATTC_EVT_PRIM_SRVC_DISC_RSP:
            logger.debug("BLE_GATTC_EVT_PRIM_SRVC_DISC_RSP: count: %u",
                         event_data.params.prim_srvc_disc_rsp.count);

            for (uint16_t iter = 0u;
                 iter < event_data.params.prim_srvc_disc_rsp.count; ++iter)
            {
                ble_gattc_service_t const* service =
                    &event_data.params.prim_srvc_disc_rsp.services[iter];

                /* The Nordic Unknown UUID type means that a 128-bit UUID was
                 * received which was no pre-regisetered with the softdevice.
                 * Issue a raw read request on the GATT start handle.
                 * When the read response comes in, it will need to be parsed
                 * as a 128-bit UUID.
                 */
                if (service->uuid.type == BLE_UUID_TYPE_UNKNOWN)
                {
                    // If this call should fail the do not return.
                    // Keep discovering services as best can be done.
                    uint32_t const error_code = gattc_uuid128_acquire(
                        event_data.conn_handle,
                        service->handle_range.start_handle);

                    if (error_code == NRF_SUCCESS)
                    {
                        // The call to gattc_uuid128_acquire() set up a read
                        // response from which service discovery will continue.
                        return;
                    }
                }

                ble::att::uuid const uuid = nordic::to_att_uuid(service->uuid);

                char uuid_char_buffer[ble::att::uuid::conversion_length];
                uuid.to_chars(std::begin(uuid_char_buffer), std::end(uuid_char_buffer));

                logger.debug("BLE_GATTC_EVT_PRIM_SRVC_DISC_RSP[0x%04x:0x%04x]: %s",
                             service->handle_range.start_handle,
                             service->handle_range.end_handle,
                             uuid_char_buffer);

                observer.interface_reference.service_discovered(
                    event_data.conn_handle,
                    nordic::to_att_error_code(event_data.gatt_status),
                    event_data.error_handle,
                    service->handle_range.start_handle,
                    service->handle_range.end_handle,
                    uuid,
                    iter + 1u >= event_data.params.prim_srvc_disc_rsp.count);
            }
            break;

        case BLE_GATTC_EVT_REL_DISC_RSP:
            // Relationship Discovery Response event.
            // See ble_gattc_evt_rel_disc_rsp_t.
            for (uint16_t iter = 0u;
                 iter < event_data.params.rel_disc_rsp.count; ++iter)
            {
                ble_gattc_include_t const* include_disc_rsp =
                    &event_data.params.rel_disc_rsp.includes[iter];

                ble_gattc_service_t const* service = &include_disc_rsp->included_srvc;

                ble::att::uuid const uuid = nordic::to_att_uuid(service->uuid);

                char uuid_char_buffer[ble::att::uuid::conversion_length];
                uuid.to_chars(std::begin(uuid_char_buffer), std::end(uuid_char_buffer));

                logger.debug("BLE_GATTC_EVT_REL_DISC_RSP[0x%04:0x%04x]: incl: 0x%04x, %s",
                             service->handle_range.start_handle,
                             service->handle_range.end_handle,
                             include_disc_rsp->handle,
                             uuid_char_buffer);

                observer.interface_reference.relationship_discovered(
                    event_data.conn_handle,
                    nordic::to_att_error_code(event_data.gatt_status),
                    event_data.error_handle,
                    service->handle_range.start_handle,
                    service->handle_range.end_handle,
                    include_disc_rsp->handle,
                    uuid,
                    iter + 1u >= event_data.params.rel_disc_rsp.count);
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

                logger.debug("BLE_GATTC_EVT_CHAR_DISC_RSP: "
                             "decl: 0x%04x, value: 0x%04x, props: 0x%04x, %s",
                             char_disc_rsp->handle_decl,
                             char_disc_rsp->handle_value,
                             properties.get(),
                             uuid_char_buffer);

                observer.interface_reference.characteristic_discovered(
                    event_data.conn_handle,
                    nordic::to_att_error_code(event_data.gatt_status),
                    event_data.error_handle,
                    char_disc_rsp->handle_decl,
                    char_disc_rsp->handle_value,
                    uuid,
                    properties,
                    iter + 1u >= event_data.params.char_disc_rsp.count);
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

                logger.debug("BLE_GATTC_EVT_CHAR_DISC_RSP[0x%04]: %s",
                             desc_disc_rsp->handle, uuid_char_buffer);

                observer.interface_reference.descriptor_discovered(
                    event_data.conn_handle,
                    nordic::to_att_error_code(event_data.gatt_status),
                    event_data.error_handle,
                    desc_disc_rsp->handle,
                    uuid,
                    iter + 1u >= event_data.params.desc_disc_rsp.count);
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

                    logger.debug("BLE_GATTC_EVT_ATTR_INFO_DISC_RSP [0x%04]: 0x%04x",
                                 gattc_attr->handle, gattc_attr->uuid);

                    observer.interface_reference.attribute_discovered(
                        event_data.conn_handle,
                        nordic::to_att_error_code(event_data.gatt_status),
                        event_data.error_handle,
                        gattc_attr->handle,
                        uuid,
                        iter + 1u >= event_data.params.attr_info_disc_rsp.count);
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

                    logger.debug("BLE_GATTC_EVT_ATTR_INFO_DISC_RSP [0x%04]: %s",
                                 gattc_attr->handle, uuid);

                    observer.interface_reference.attribute_discovered(
                        event_data.conn_handle,
                        nordic::to_att_error_code(event_data.gatt_status),
                        event_data.error_handle,
                        gattc_attr->handle,
                        uuid,
                        iter + 1u >= event_data.params.attr_info_disc_rsp.count);
                }
                break;

            default:
                logger.error("unknown Nordic attribute uuid discovery format: %u",
                             event_data.params.attr_info_disc_rsp.format);
                ASSERT(0);
                break;
            }
            break;

        default:
            break;
        }
    }
}

} // namespace nordic
