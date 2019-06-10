/**
 * @file nordic_ble_common_event_observable.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "ble/nordic_ble_event_observer.h"
#include "logger.h"

namespace nordic
{

void ble_common_event_notify(ble::common::event_observer&   observer,
                             enum BLE_COMMON_EVTS           event_type,
                             ble_common_evt_t const&        event_data)
{
    auto const memory_type = static_cast<ble::common::memory_type>(
        event_data.params.user_mem_request.type);

    switch (event_type)
    {
    case BLE_EVT_USER_MEM_REQUEST:
        observer.memory_request(event_data.conn_handle, memory_type, 0u, 1u);
        break;

    case BLE_EVT_USER_MEM_RELEASE:
        observer.memory_release(event_data.conn_handle,
                                memory_type,
                                event_data.params.user_mem_release.mem_block.p_mem,
                                event_data.params.user_mem_release.mem_block.len);
        break;

    default:
        logger::instance().warn("unhandled Nordic common event: %u", event_type);
        break;
    }
}

} // namespace nordic
