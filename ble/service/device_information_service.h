/**
 * @file device_information_service.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * @see https://www.bluetooth.com/specifications/gatt
 */

#pragma once

#include "ble/gatt_service.h"
#include "ble/gatt_descriptors.h"
#include "ble/gatt_uuids.h"

namespace ble
{
namespace service
{

/**
 * @class device_information_service
 * https://www.bluetooth.org/docman/handlers/downloaddoc.ashx?doc_id=244369
 *
 * @note The characteristics are all of type and must be added by the code
 * author.
 */
class device_information_service: public gatt::service
{
public:
    virtual ~device_information_service() override                            = default;

    device_information_service(device_information_service const&)             = delete;
    device_information_service(device_information_service &&)                 = delete;
    device_information_service& operator=(device_information_service const&)  = delete;
    device_information_service& operator=(device_information_service&&)       = delete;

    /**
     * Construct a Current Time Service as a primary service.
     *
     * @param attr_type Specify whether the battery service
     * is a primary or secondary service.
     */
    device_information_service():
        service(gatt::services::device_information,
                gatt::attribute_type::primary_service)
    {
    }
};

} // namespace service
} // namespace ble

