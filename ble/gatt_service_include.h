/**
 * @file ble/gatt_service_include.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "ble/uuid.h"
#include "ble/att.h"
#include "ble/gatt_declaration.h"
#include "ble/gatt_descriptors.h"
#include "ble/gatt_characteristic.h"
#include "ble/gatt_uuids.h"

#include <algorithm>
#include <cstdint>
#include <boost/intrusive/list.hpp>

namespace ble
{
namespace gatt
{

/**
 * @class service_include
 * Include characteristics from one service within another.
 */
class service_include
{
public:
    virtual ~service_include()                          = default;

    service_include(service_include const&)             = delete;
    service_include(service_include &&)                 = delete;
    service_include& operator=(service_include const&)  = delete;
    service_include& operator=(service_include&&)       = delete;

    service_include():
        decl(attribute_type::include, properties::read),
        service_ptr(nullptr),
        characteristic_begin_ptr(nullptr),
        characteristic_last_ptr(nullptr)
    {
    }

    /**
     * Instantiate a BLE GATT Service Inclusion.
     *
     * @param service A reference to the service whose characteristics
     *                will be included.
     * @param characteristic_begin The first characteristic to include.
     * @param characteristic_last  The last characteristic to include.
     */
    service_include(service             &service,
                    characteristic      &characteristic_begin,
                    characteristic      &characteristic_last) :

        decl(attribute_type::include,   properties::read),
        service_ptr(&service),
        characteristic_begin_ptr(&characteristic_begin),
        characteristic_last_ptr(&characteristic_last)
    {
    }

    declaration     decl;
    service         *service_ptr;
    characteristic  *characteristic_begin_ptr;
    characteristic  *characteristic_last_ptr;
};

} // namespace gatt
} // namespace ble

