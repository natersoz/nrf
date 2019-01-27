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
                                      &service::hook>
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

    ble::gatt::service const* find_service(ble::att::uuid const &uuid) const;
    ble::gatt::service*       find_service(ble::att::uuid const &uuid);

    ble::gatt::service const* find_service(ble::gatt::service_type uuid_16) const;
    ble::gatt::service*       find_service(ble::gatt::service_type uuid_16);

    ble::gatt::service const* find_service_handle_assoc(uint16_t gatt_handle) const;
    ble::gatt::service*       find_service_handle_assoc(uint16_t gatt_handle);

    ble::att::handle_range service_handle_range(ble::gatt::service const& service) const;

    ble::gatt::characteristic const* find_characteristic(uint16_t handle) const;
    ble::gatt::characteristic*       find_characteristic(uint16_t handle);

    ble::gatt::characteristic const* find_characteristic(ble::att::uuid const &uuid) const;
    ble::gatt::characteristic*       find_characteristic(ble::att::uuid const &uuid);

    ble::gatt::characteristic const* find_characteristic(ble::gatt::characteristic_type const uuid_16) const;
    ble::gatt::characteristic*       find_characteristic(ble::gatt::characteristic_type const uuid_16);

    class discovery_iterator
    {
    public:
        struct iterator_node
        {
            ~iterator_node()                                = default;

            iterator_node()                                 = delete;
            iterator_node(iterator_node const&)             = default;
            iterator_node(iterator_node &&)                 = default;
            iterator_node& operator=(iterator_node const&)  = default;
            iterator_node& operator=(iterator_node&&)       = default;

            iterator_node(ble::gatt::service& svc, ble::gatt::attribute& chr)
            : service(svc),
              characteristic(reinterpret_cast<ble::gatt::characteristic&>(chr))
            {
            }

            ble::gatt::service&             service;
            ble::gatt::characteristic&      characteristic;
        };

        using iterator_category = std::bidirectional_iterator_tag;
        using value_type        = iterator_node;
        using difference_type   = iterator_node;
        using pointer           = iterator_node*;
        using reference         = iterator_node&;

        ~discovery_iterator()                                     = default;

        discovery_iterator(discovery_iterator const&)             = default;
        discovery_iterator(discovery_iterator &&)                 = default;
        discovery_iterator& operator=(discovery_iterator const&)  = default;
        discovery_iterator& operator=(discovery_iterator&&)       = default;

        discovery_iterator(
            ble::gatt::service_container*             srv_container,
            ble::gatt::service_container::iterator    srv_iterator,
            ble::gatt::attribute::list_type::iterator chr_iterator)
        : service_container(srv_container),
          service_iterator(srv_iterator),
          characteristic_iterator(chr_iterator)
        {
        }

        /**
         * The default ctor is an iterator which points at nothing.
         * Make certain it is set to something valid before usage.
         */
        discovery_iterator()
        : service_container(nullptr),
          service_iterator(),
          characteristic_iterator()
        {
        }

        discovery_iterator& operator++()
        {
            increment();
            return *this;
        }

        discovery_iterator operator++(int)
        {
            discovery_iterator previous = *this;
            increment();
            return previous;
        }

        discovery_iterator& operator--()
        {
            decrement();
            return *this;
        }

        discovery_iterator operator--(int)
        {
            discovery_iterator previous = *this;
            decrement();
            return previous;
        }

        bool operator==(discovery_iterator const& other) const
        {
            return
                (this->service_iterator == other.service_iterator) &&
                (this->characteristic_iterator == other.characteristic_iterator);
        }

        bool operator!=(discovery_iterator const& other) const
        {
            return not (*this == other);
        }

        /// Dereference the iterator to obtain the service, characteristic pair.
        value_type operator *() const
        {
            return iterator_node(*this->service_iterator,
                                 *this->characteristic_iterator);
        }

        void print() const;

        /// Move forward to the next characteristic in the service container.
        void increment();
        /// Move reverse to the prev characteristic in the service container.
        void decrement();

        /**
         * Get the characteristic handle range from the iterator.
         *
         * @return ble::att::handle_range The handle range associated with
         * the current characteristic.
         */
        ble::att::handle_range handle_range() const;

        ble::gatt::service_container*               service_container;
        ble::gatt::service_container::iterator      service_iterator;
        ble::gatt::attribute::list_type::iterator   characteristic_iterator;
    };

    discovery_iterator discovery_begin();
    discovery_iterator discovery_end();

    discovery_iterator next_open_characteristic(discovery_iterator disco_iter);

private:
    /**
     * When the service_container discovery_iterator points to the end() of
     * the list the the characteristic iterator needs somthing valid to point
     * to. It will point to charateristic_sentinel::end();
     */
    ble::gatt::attribute::list_type charateristic_sentinel;
};

} // namespace gatt
} // namespace ble
