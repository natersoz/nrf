/**
 * @file ble/gatt_service_discovery_iterator.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "gatt_service_container.h"
#include "logger.h"
#include "project_assert.h"

namespace ble
{
namespace gatt
{

void service_container::discovery_iterator::print(logger::level level) const
{
    logger &logger = logger::instance();
    iterator_node const node = **this;

    char uuid_char_buffer[ble::att::uuid::conversion_length];

    node.service.uuid.to_chars(std::begin(uuid_char_buffer),
                               std::end(uuid_char_buffer));
    logger.write(level, "service:        (t:0x%04x, h:0x%04x): %s",
                 node.service.decl.attribute_type,
                 node.service.decl.handle, uuid_char_buffer);

    node.characteristic.uuid.to_chars(std::begin(uuid_char_buffer),
                                      std::end(uuid_char_buffer));
    logger.write(level, "characteristic: (t:0x%04x, h:0x%04x): %s",
                 node.characteristic.decl.attribute_type,
                 node.characteristic.decl.handle, uuid_char_buffer);

    ble::att::handle_range const handle_range = this->handle_range();
    logger.write(level, "handle range:   [0x%04x, 0x%04x]",
                 handle_range.first, handle_range.second);
}

void service_container::discovery_iterator::increment()
{
    if (this->service_iterator == this->service_container->end())
    {
        ASSERT(0);      // Programming error.
    }

    ble::gatt::service& service = *this->service_iterator;
    ++this->characteristic_iterator;
    if (this->characteristic_iterator == service.characteristic_list.end())
    {
        ++this->service_iterator;
        if (this->service_iterator == this->service_container->end())
        {
            // The end of the service list; set the characteristic iterator
            // to point at the sentinel.
            this->characteristic_iterator =
                this->service_container->discovery_end().characteristic_iterator;
        }
        else
        {
            ble::gatt::service& service = *this->service_iterator;
            this->characteristic_iterator = service.characteristic_list.begin();

            if (this->characteristic_iterator == service.characteristic_list.end())
            {
                // This service contains no characteristics; skip to the next.
                // Note: This is a recursive call.
                this->increment();
            }
        }
    }
}

void service_container::discovery_iterator::decrement()
{
    if (this->service_iterator == this->service_container->end())
    {
        --this->service_iterator;
        ble::gatt::service &service = *this->service_iterator;
        this->characteristic_iterator = service.characteristic_list.end();
    }
    else
    {
        ble::gatt::service &service = *this->service_iterator;
        if (this->characteristic_iterator == service.characteristic_list.begin())
        {
            if (this->service_iterator == this->service_container->begin())
            {
                ASSERT(0);      // Programming error.
            }

            --this->service_iterator;
            ble::gatt::service& service = *this->service_iterator;
            this->characteristic_iterator = service.characteristic_list.end();
        }
    }
    --this->characteristic_iterator;
}

/**
 * Get the characteristic handle range from the iterator.
 * The handle range [first, second] is the characteristic declaration handle
 * (the lowest valued handle within the characteristic) to the next
 * characteristic in the container -1. This last handle in the characteristic
 * may be the characteristic value handle up if the characteristic has no
 * descriptors or the last descriptor handle.
 *
 * @return ble::att::handle_range The handle range associated with
 *                                the current characteristic.
 */
ble::att::handle_range service_container::discovery_iterator::handle_range() const
{
    if (this->service_iterator == this->service_container->end())
    {
        /// @todo check logic:
        return ble::att::handle_range(ble::att::handle_maximum,
                                      ble::att::handle_maximum);
    }
    else
    {
        discovery_iterator const& iter_this = *this;
        discovery_iterator        iter_next = iter_this;
        ++iter_next;

        ble::att::handle_range const handle_range(
            (*iter_this).characteristic.decl.handle,
            (*iter_next).characteristic.decl.handle - 1u);

        return handle_range;
    }
}

} // namespace gatt
} // namespace ble

