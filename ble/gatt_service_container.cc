/**
 * @file ble/gatt_service_container.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "gatt_service_container.h"
#include "logger.h"
#include "project_assert.h"

#include <algorithm>
#include <utility>

namespace ble
{
namespace gatt
{

ble::gatt::service const*
    service_container::find_service(ble::att::uuid const &uuid) const
{
    service_list_type::const_iterator iter =
        std::find_if(this->begin(), this->end(),
                     [&uuid] (ble::gatt::service const& service_in_list) {
                         return uuid == service_in_list.uuid; });

    return (iter == this->end())? nullptr : &*iter;
}

ble::gatt::service*
    service_container::find_service(ble::att::uuid const &uuid)
{
    return const_cast<ble::gatt::service*>(
        std::as_const(*this).find_service(uuid)
        );
}

ble::gatt::service const*
    service_container::find_service(ble::gatt::service_type uuid_16) const
{
    ble::att::uuid const uuid(static_cast<uint16_t>(uuid_16));
    return this->find_service(uuid);
}

ble::gatt::service* service_container::find_service(ble::gatt::service_type uuid_16)
{
    return const_cast<ble::gatt::service*>(std::as_const(*this).find_service(uuid_16));
}

ble::gatt::service const*
    service_container::find_service_by_handle(uint16_t handle) const
{
    ble::gatt::service const* service_assoc = nullptr;
    for (ble::gatt::service const& service : *this)
    {
        if (service.decl.handle <= handle)
        {
            if (service_assoc)
            {
                if (service_assoc->decl.handle < service.decl.handle)
                {
                    service_assoc = &service;
                }
            }
            else
            {
                service_assoc = &service;
            }
        }
    }

    return service_assoc;
}

ble::gatt::service* service_container::find_service_by_handle(uint16_t handle)
{
    return const_cast<ble::gatt::service*>(
        std::as_const(*this).find_service_by_handle(handle));
}

ble::att::handle_range service_container::service_handle_range(
    ble::gatt::service const& service) const
{
    auto iter = this->iterator_to(service);
    ++iter;
    uint16_t const handle_last = (iter == this->end())
                                 ? ble::att::handle_maximum
                                 : ((*iter).decl.handle - 1u);
    ble::att::handle_range handle_range(service.decl.handle, handle_last);
    return handle_range;
}

ble::gatt::characteristic const*
    service_container::find_characteristic(uint16_t handle) const
{
    for (ble::gatt::service const& service : *this)
    {
        ble::gatt::attribute const* attribute = service.find_attribute(handle);
        if (attribute)
        {
            return static_cast<characteristic const*>(attribute);
        }
    }

    return nullptr;
}

ble::gatt::characteristic*
    service_container::find_characteristic(uint16_t handle)
{
    return const_cast<ble::gatt::characteristic*>(
        std::as_const(*this).find_characteristic(handle)
        );
}

ble::gatt::characteristic const*
    service_container::find_characteristic(ble::att::uuid const &uuid) const
{
    for (ble::gatt::service const& service : *this)
    {
        ble::gatt::characteristic const* charateristic =
            service.find_characteristic(uuid);
        if (charateristic)
        {
            return charateristic;
        }
    }

    return nullptr;
}

ble::gatt::characteristic*
    service_container::find_characteristic(ble::att::uuid const &uuid)
{
    return const_cast<ble::gatt::characteristic*>(
        std::as_const(*this).find_characteristic(uuid)
        );
}

ble::gatt::characteristic const* service_container::find_characteristic(
    ble::gatt::characteristic_type const uuid_16) const
{
    ble::att::uuid const uuid(static_cast<uint16_t>(uuid_16));
    return this->find_characteristic(uuid);
}

ble::gatt::characteristic* service_container::find_characteristic(
    ble::gatt::characteristic_type const uuid_16)
{
    return const_cast<ble::gatt::characteristic*>(
        std::as_const(*this).find_characteristic(uuid_16)
        );
}

/**
 * Point the iterator to the first characteristic of the first service in the
 * list. If there are no serivces in the container then the characteristic
 * iterator points to the charateristic_sentinel.end().
 *
 * @return ble::gatt::service_container::discovery_iterator
 */
ble::gatt::service_container::discovery_iterator
    service_container::discovery_begin()
{
    ble::gatt::service_container::iterator serv_iterator = this->begin();
    attribute::list_type::iterator
        char_iterator(this->charateristic_sentinel.end());
    if (serv_iterator != this->end())
    {
        ble::gatt::service &service = *serv_iterator;
        char_iterator = service.characteristic_list.begin();
    }

    return discovery_iterator(this, serv_iterator, char_iterator);
}

// Point the iterator to the end characteristic of the last service in the
// list. This works even if there is no characteristic in the last service.
ble::gatt::service_container::discovery_iterator
    service_container::discovery_end()
{
    ble::gatt::service_container::iterator serv_iterator = this->end();
    attribute::list_type::iterator
        char_iterator(this->charateristic_sentinel.end());
    return discovery_iterator(this, serv_iterator, char_iterator);
}

ble::gatt::service_container::discovery_iterator
    service_container::next_open_characteristic(discovery_iterator disco_iter)
{
    logger &logger = logger::instance();
    logger.debug("next_open_characteristic: ");

    ble::gatt::service_container::discovery_iterator iter_end =
        this->discovery_end();
    for (; disco_iter != iter_end; ++disco_iter)
    {
        ble::att::handle_range const handle_range = disco_iter.handle_range();

        // Each characteristic requires 2 handles: a declaration handle
        // and its value handle. If there is room for other attributes prior
        // to the next characteristic handle then it is considered 'open'.
        if (handle_range.second >= handle_range.first + 2u)
        {
            logger.debug("------ open_characteristic found:");
            disco_iter.print(logger::level::debug);
            break;
        }
    }

    return disco_iter;
}

} // namespace gatt
} // namespace ble


