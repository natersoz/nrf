/**
 * @file nordic_gatts.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "nordic_gatts.h"
#include "project_assert.h"
#include "logger.h"

#include "ble.h"                    // Softdevice headers
#include "ble_gatts.h"
#include "nrf_error.h"

#include <cstring>

namespace nordic
{

/** Convert the generic service attribute type to the Nordic symbol
 *
 * @param attr_type The generice attribute type, one of:
 *        ble::gatt::attribute_type::primary_service
 *        ble::gatt::attribute_type::secondary_service
 *
 * @return uint8_t The corresponding Nordic type.
 */
static uint8_t nordic_service_type(ble::gatt::attribute_type attr_type)
{
    switch (attr_type)
    {
    case ble::gatt::attribute_type::primary_service:
        return BLE_GATTS_SRVC_TYPE_PRIMARY;
    case ble::gatt::attribute_type::secondary_service:
        return BLE_GATTS_SRVC_TYPE_SECONDARY;
    default:
        ASSERT(0);
        return BLE_GATTS_SRVC_TYPE_INVALID;
    }
}

/**
 * Convert a generic uuid to a Nordic uuid.
 *
 * @param uuid The generic ble::att::uuid to convert to a Nordic type.
 * @return ble_uuid_t The Nordic UUID, ready for use.
 */
static ble_uuid_t nordic_uuid_type(ble::att::uuid const &uuid)
{
    if (uuid.is_ble())
    {
        ble_uuid_t const nordic_uuid = {
            .uuid = uuid.get_u16(),
            .type = BLE_UUID_TYPE_BLE
        };

        return nordic_uuid;
    }
    else
    {
        // Convert from uuid big-endian to little-endian.
        // For the base: zero out bytes [12:15], the least significant 32-bits.
        // Note: Nordic appears to ignore [12:15] within sd_ble_uuid_vs_add()
        // and internal table, but be safe anyway.
        ble::att::uuid uuid_base = uuid.reverse();
        uuid_base.data[12u] = 0u;
        uuid_base.data[13u] = 0u;
        uuid_base.data[14u] = 0u;
        uuid_base.data[15u] = 0u;

        ble_uuid128_t nordic_uuid_128;
        memcpy(nordic_uuid_128.uuid128, uuid_base.data, sizeof(nordic_uuid_128.uuid128));
        static_assert(sizeof(nordic_uuid_128.uuid128) == sizeof(uuid_base.data));

        /*
         * Regarding Nordic handling of 128-bit uuids:
         *
         * Each time sd_ble_uuid_vs_add() is called the 128-bit uuid is added to
         * an array within the softdevice. The index into that array is passed
         * back through the uint8_t *p_uuid_type parameter.
         * The same 128-bit value can be passed multiple times and each repeated
         * time for the same 128-bit value will return the same index.
         * We can just keep adding the base uuid value over and over again
         * and Nordic will return the same index for repeated uuid values.
         * The zero value of this index is BLE_UUID_TYPE_VENDOR_BEGIN.
         */
        uint8_t nordic_index = 0u;
        uint32_t const error = sd_ble_uuid_vs_add(&nordic_uuid_128, &nordic_index);
        if (error == NRF_SUCCESS)
        {
            logger &logger = logger::instance();
            logger.debug("sd_ble_uuid_vs_add() OK: index: %u", nordic_index);
            logger.write_data(logger::level::debug,
                              nordic_uuid_128.uuid128,
                              sizeof(nordic_uuid_128.uuid128));

            ble_uuid_t const nordic_uuid = {
                .uuid = uuid.get_u16(),
                .type = nordic_index
            };

            return nordic_uuid;
        }
        else
        {
            logger &logger = logger::instance();
            logger.error("error: sd_ble_uuid_vs_add() failed: %u", error);
            logger.error("error: nordic_index: %u, nordic uuid:", nordic_index);
            logger.write_data(logger::level::error,
                              nordic_uuid_128.uuid128,
                              sizeof(nordic_uuid_128.uuid128));
        }
    }

    // Indicate to the caller that this was a fail.
    ble_uuid_t const nordic_uuid = {
        .uuid = 0x0000,
        .type = BLE_UUID_TYPE_UNKNOWN,
    };

    return nordic_uuid;
}

/**
 * Convert the class ble::gatt::properties uint16_t bit-field into the
 * equivalent Nordic bit-field.
 *
 * @param property_bits The bit-field extracted from the ble::gatt::properties
 * class using the function ble::gatt::properties::get().
 *
 * @return ble_gatt_char_props_t The Nordic properties bit-field.
 */
static ble_gatt_char_props_t nordic_properties(uint16_t property_bits)
{
    ble_gatt_char_props_t const nordic_props = {
        .broadcast      = bool(property_bits & ble::gatt::properties::broadcast),
        .read           = bool(property_bits & ble::gatt::properties::read),
        .write_wo_resp  = bool(property_bits & ble::gatt::properties::write_without_response),
        .write          = bool(property_bits & ble::gatt::properties::write),
        .notify         = bool(property_bits & ble::gatt::properties::notify),
        .indicate       = bool(property_bits & ble::gatt::properties::indicate),
        .auth_signed_wr = bool(property_bits & ble::gatt::properties::write_with_signature),
    };

    return nordic_props;
}

/**
 * Convert the class ble::gatt::properties uint16_t extended bit-field into the
 * equivalent Nordic extended bit-field.
 *
 * @param property_bits The bit-field extracted from the ble::gatt::properties
 * class using the function ble::gatt::properties::get().
 *
 * @return ble_gatt_char_ext_props_t The Nordic extended properties bit-field.
 */
static ble_gatt_char_ext_props_t nordic_properties_ext(uint16_t property_bits)
{
    ble_gatt_char_ext_props_t const nordic_props_ext = {
        .reliable_wr    = bool(property_bits & ble::gatt::properties::write_reliable),
        .wr_aux         = bool(property_bits & ble::gatt::properties::write_aux),
    };

    return nordic_props_ext;
}

static ble_gatts_char_pf_t noridc_presentation_descriptor(
    ble::gatt::characteristic_presentation_format_descriptor const *presd)
{
    if (presd)
    {
        ble_gatts_char_pf_t const pf = {
            .format     = static_cast<uint8_t>(presd->format),
            .exponent   = presd->exponent,
            .unit       = static_cast<uint16_t>(presd->units),
            .name_space = presd->name_space,
            .desc       = presd->description,
        };
        return pf;
    }
    else
    {
        ble_gatts_char_pf_t pf;
        memset(&pf, 0, sizeof(pf));
        return pf;
    }
}

/**
 * Add the service characteristic to the GATT sever using the Nordic softdevice.
 *
 * @param service_handle The service handle.
 * @param [inout] characteristic The characteristic.
 * The following elements are ports of the characteristic class are modified:
 * - The handle for the characteristic attribute value.
 * - The handles for the characteristic descriptors.
 * - The data pointer from the characteristic attribute is given to the
 *   softdevice as writeable.
 *
 * @return uint32_t The Nordic error code.
 * @retval NRF_SUCCESS if successful.
 */
static uint32_t gatts_characteristic_add(uint16_t service_handle,
                                         ble::gatt::characteristic &characteristic)
{
    logger &logger = logger::instance();

    // The characteristic descriptors supported by Nordic.
    ble::gatt::characteristic_user_descriptor                 *userd = nullptr;
    ble::gatt::client_characteristic_configuration_descriptor *cccd  = nullptr;
    ble::gatt::server_characteristic_configuration_descriptor *sccd  = nullptr;
    ble::gatt::characteristic_presentation_format_descriptor  *presd = nullptr;

    // Loop through the characteritic descriptors and find the ones supported by Nordic.
    for (ble::gatt::characteristic_base_descriptor &node : characteristic.descriptor_list)
    {
        // Note: with --rtti turned off in the compiler we have to use
        // reinterpret_cast<> instead of dynamic_cast<>.
        switch (node.decl.attribute_type)
        {
        case ble::gatt::attribute_type::characteristic_user_description:
            userd = reinterpret_cast<ble::gatt::characteristic_user_descriptor*>(&node);
            break;
        case ble::gatt::attribute_type::client_characteristic_configuration:
            cccd = reinterpret_cast<ble::gatt::client_characteristic_configuration_descriptor*>(&node);
            break;
        case ble::gatt::attribute_type::server_characteristic_configuration:
            sccd = reinterpret_cast<ble::gatt::server_characteristic_configuration_descriptor*>(&node);
            break;
        case ble::gatt::attribute_type::characteristic_presentation_format:
            presd = reinterpret_cast<ble::gatt::characteristic_presentation_format_descriptor*>(&node);
            break;
        default:
            break;
        }
    }

    uint16_t const property_bits    = characteristic.decl.properties.get();
    ble_gatts_char_pf_t const pf    = noridc_presentation_descriptor(presd);

    constexpr uint16_t const u16_zero   = 0u;
    constexpr uint8_t  const vloc_stack = BLE_GATTS_VLOC_STACK;
    constexpr uint8_t  const vloc_user  = BLE_GATTS_VLOC_USER;

    ble_gatts_char_md_t const characteristic_metadata = {
        .char_props                 = nordic_properties(property_bits),
        .char_ext_props             = nordic_properties_ext(property_bits),
        .p_char_user_desc           = userd ? reinterpret_cast<uint8_t const*>(userd->user_string_ptr) : nullptr,
        .char_user_desc_max_size    = userd ? userd->user_string_length : u16_zero,
        .char_user_desc_size        = userd ? userd->user_string_length : u16_zero,
        .p_char_pf                  = presd ? &pf : nullptr,

        /// @todo These get changed once SM is better understood.
        /// For now the defaults will work properly.
        .p_user_desc_md             = nullptr,  ///< metadata for 0x2901 user_descr
        .p_cccd_md                  = nullptr,  ///< metadata for 0x2902 cccd
        .p_sccd_md                  = nullptr,  ///< metadata for 0x2903 sccd
    };

    ble_gatts_attr_md_t const attribute_metadata = {
        /// @todo These get changed once SM is better understood.
        .read_perm                  = { .sm = 0u, .lv = 0u },   ///< @todo SM
        .write_perm                 = { .sm = 0u, .lv = 0u },   ///< @todo SM
        .vlen                       = characteristic.data_length_is_variable(),
        .vloc                       = (characteristic.data_pointer() == nullptr) ?
                                      vloc_stack : vloc_user,

        /// @todo These get changed once SM is better understood.
        .rd_auth                    = false,    /// @todo SM authorization and value will be requested from the application on every read operation.
        .wr_auth                    = false,    /// @todo SM Write authorization will be requested from the application on every Write Request operation (but not Write Command).
    };

    ble_uuid_t const nordic_uuid = nordic_uuid_type(characteristic.uuid);

    ble_gatts_attr_t const characteristic_attribute_value = {
        .p_uuid                     = &nordic_uuid,
        .p_attr_md                  = &attribute_metadata,
        .init_len                   = characteristic.data_length(),
        .init_offs                  = u16_zero,
        .max_len                    = characteristic.data_length_max(),
        .p_value                    = reinterpret_cast<uint8_t*>(characteristic.data_pointer()),
    };

    ble_gatts_char_handles_t gatt_handles;
    uint32_t const error = sd_ble_gatts_characteristic_add(service_handle,
                                                           &characteristic_metadata,
                                                           &characteristic_attribute_value,
                                                           &gatt_handles);


    char uuid_char_buffer[ble::att::uuid::conversion_length];
    characteristic.uuid.to_chars(uuid_char_buffer, uuid_char_buffer + ble::att::uuid::conversion_length);
    if (error == NRF_SUCCESS)
    {
        logger.debug("sd_ble_gatts_characteristic_add(%s): OK", uuid_char_buffer);
    }
    else
    {
        logger.error("error: sd_ble_gatts_characteristic_add(%s) failed: %u",
                     uuid_char_buffer, error);
    }

    characteristic.decl.handle = gatt_handles.value_handle;
    logger.debug("value handle: 0x%04x", gatt_handles.value_handle);

    if (userd)
    {
        logger.debug("userd handle: 0x%04x", gatt_handles.user_desc_handle);
        userd->decl.handle = gatt_handles.user_desc_handle;
    }

    if (cccd)
    {
        logger.debug("cccd  handle: 0x%04x", gatt_handles.cccd_handle);
        cccd->decl.handle = gatt_handles.cccd_handle;
    }

    if (sccd)
    {
        logger.debug("sccd  handle: 0x%04x", gatt_handles.sccd_handle);
        sccd->decl.handle = gatt_handles.sccd_handle;
    }

    return error;
}

uint32_t gatts_service_add(ble::gatt::service& service)
{
    logger           &logger     = logger::instance();
    uint8_t    const nordic_type = nordic_service_type(service.decl.attribute_type);
    ble_uuid_t const nordic_uuid = nordic_uuid_type(service.uuid);

    ASSERT(nordic_uuid.type != BLE_UUID_TYPE_UNKNOWN);

    char uuid_char_buffer[ble::att::uuid::conversion_length];
    service.uuid.to_chars(uuid_char_buffer, uuid_char_buffer + ble::att::uuid::conversion_length);

    uint32_t error = sd_ble_gatts_service_add(nordic_type,
                                              &nordic_uuid,
                                              &service.decl.handle);

    if (error == NRF_SUCCESS)
    {
        logger.debug("sd_ble_gatts_service_add(%s): OK", uuid_char_buffer);

        for (ble::gatt::characteristic &node : service.characteristic_list)
        {
            error = gatts_characteristic_add(service.decl.handle, node);
            if (error != NRF_SUCCESS) { break; }
        }
    }
    else
    {
        logger.error("error: sd_ble_gatts_service_add(%s) failed: %u",
                     uuid_char_buffer, error);
    }

    return error;
}

#if 0
uint32_t gatts_serivce_include_add(uint16_t service_handle, )
{

}
#endif
} // namespace nordic

