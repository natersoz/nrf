/**
 * @file nordic_ble_common_event_observable.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "ble/nordic_ble_event_observable.h"
#include "ble/nordic_ble_event_observer.h"

#include "logger.h"

namespace nordic
{

template<>
void ble_event_observable<ble_common_event_observer>::notify(
    ble_common_event_observer::event_enum_t event_type,
    ble_common_event_observer::event_data_t const&  event_data)
{
    for (auto observer_iter  = this->observer_list_.begin();
              observer_iter != this->observer_list_.end(); )
    {
        logger &logger = logger::instance();

        // Increment the iterator prior to using it.
        // If the client removes itself during the completion callback
        // then the iterator will be valid and can continue.
        auto &observer = *observer_iter;
        ++observer_iter;

        switch (event_type)
        {
        case BLE_EVT_USER_MEM_REQUEST:
            // User Memory request. @ref ble_evt_user_mem_request_t
            {
                observer.interface_reference.memory_request(
                    event_data.conn_handle,
                    static_cast<ble::common::memory_type>(event_data.params.user_mem_request.type),
                    0u,
                    1u);
            }
            break;
        case  BLE_EVT_USER_MEM_RELEASE:
            // User Memory release. @ref ble_evt_user_mem_release_t
            {
                observer.interface_reference.memory_release(
                    event_data.conn_handle,
                    static_cast<ble::common::memory_type>(event_data.params.user_mem_release.type),
                    event_data.params.user_mem_release.mem_block.p_mem,
                    event_data.params.user_mem_release.mem_block.len);
            }
            break;
        default:
            logger.warn("unhandled Nordic common event: %u", event_type);
            break;
        }
    }
}

} // namespace nordic
