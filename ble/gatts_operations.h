/**
 * @file ble/gatts_operations.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Generic GATTS BLE commands, response, indication, notification methods.
 */

#pragma once

#include <cstdint>
#include <system_error>

#include "ble/att.h"
#include "ble/gatt_service.h"

#include "ble/profile_connectable_accessor.h"

namespace ble
{
namespace gatts
{

class operations: public ble::profile::connectable_accessor
{
public:
    virtual ~operations()                       = default;

    operations()                                = default;
    operations(operations const&)               = delete;
    operations(operations &&)                   = delete;
    operations& operator=(operations const&)    = delete;
    operations& operator=(operations&&)         = delete;

    /**
     * Send GATT characteristic notifications for a specific attribute.
     *
     * @param connection_handle
     * @param attribute_handle
     * @param offset
     * @param length
     * @param data
     *
     * @return att::length_t The number of bytes sent.
     * @todo Need to return an error code as well.
     *
     * Failures modes:
     *      No such attribute.
     *      Invalid attribute offset
     *      Invalid connecion handle
     *      System atribute missing - see sd_ble_gatts_sys_attr_set()
     *      Resources in use, wait for BLE_GATTS_EVT_HVN_TX_COMPLETE to continue
     *      Busy, wait for BLE_GATTS_EVT_HVC to continue
     */
    virtual att::length_t notify(
        uint16_t            connection_handle,
        uint16_t            attribute_handle,
        att::length_t       offset,
        att::length_t       length,
        void const*         data) = 0;

    virtual att::length_t indicate(
        uint16_t            connection_handle,
        uint16_t            attribute_handle,
        att::length_t       offset,
        att::length_t       length,
        void const*         data) = 0;

    /**
     * Respond to a read authorization request.
     * This call should only be made in response to a GATTS event
     * read_authorization_request().
     *
     * @param connection_handle
     * @param attribute_handle
     * @param error_code
     * @param atttribute_update If true, then the offset, data, length
     *                          parameters are used to update the attribute value.
     * @todo Why is this needed? We can just update the characteristic outselves.
     */
    virtual std::errc read_authorize_reply(
        uint16_t            connection_handle,
        uint16_t            attribute_handle,
        att::error_code     error_code,
        bool                atttribute_update,
        att::length_t       offset,
        att::length_t       length,
        void const*         data) = 0;

    virtual std::errc write_authorize_reply(
        uint16_t            connection_handle,
        uint16_t            attribute_handle,
        att::error_code     error_code,
        bool                atttribute_update,
        att::length_t       offset,
        att::length_t       length,
        void const*         data) = 0;

    /**
     * Reply to an MTU request from the GATTC (client) by the GATTS (server).
     *
     * @param connection_handle
     * @param att_mtu_length
     */
    virtual std::errc exchange_mtu_reply(
        uint16_t            connection_handle,
        att::length_t       att_mtu_length) = 0;

    /**
     * Add a GATT service.
     *
     * @param service The ble::gatt:: service to add.
     * The service must be fully contructed with the correct uuid, etc.
     *
     * @return ble::gatts::error_code
     */
    virtual std::errc service_add(
        ble::gatt::service& service) = 0;
};

} // namespace gatts
} // namespace ble

