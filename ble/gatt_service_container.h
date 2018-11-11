/**
 * @file ble/gatt_service_container.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "ble/uuid.h"
#include "ble/att.h"
#include "ble/gatt_service.h"

#include <algorithm>
#include <cstdint>
#include <boost/intrusive/list.hpp>

namespace ble
{
namespace gatt
{

/**
 * The list of descriptors associated with this characteristic.
 * @note constant_time_size<false> is used so that the nodes can be
 * efficiently removed through hook.unlink().
 */
using service_list_type =
    boost::intrusive::list<
        service,
        boost::intrusive::constant_time_size<false>,
        boost::intrusive::member_hook<service,
                                      service::list_hook_type,
                                      &service::hook_>
    >;

class service_container: public service_list_type
{
public:
    ~service_container()                                    = default;

    service_container()                                     = default;
    service_container(service_container const&)             = delete;
    service_container(service_container &&)                 = delete;
    service_container& operator=(service_container const&)  = delete;
    service_container& operator=(service_container&&)       = delete;

    service const* find_service(ble::att::uuid const &uuid) const;
    service*       find_service(ble::att::uuid const &uuid);

    service const* find_service(ble::gatt::services uuid_16) const;
    service*       find_service(ble::gatt::services uuid_16);

    characteristic const* find_characteristic(uint16_t handle) const;
    characteristic*       find_characteristic(uint16_t handle);

    characteristic const* find_characteristic(ble::att::uuid const &uuid) const;
    characteristic*       find_characteristic(ble::att::uuid const &uuid);

    characteristic const* find_characteristic(ble::gatt::characteristics const uuid_16) const;
    characteristic*       find_characteristic(ble::gatt::characteristics const uuid_16);
};

} // namespace gatt
} // namespace ble
