/**
 * @file ble/common_event_observer.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include <cstdint>
#include <cstddef>

namespace ble
{
namespace common
{

enum class memory_type: uint8_t
{
    invalid             = 0,
    gatt_queued_writes  = 1,
};

class event_observer
{
public:
    virtual ~event_observer()                         = default;

    event_observer()                                  = default;
    event_observer(event_observer const&)             = delete;
    event_observer(event_observer &&)                 = delete;
    event_observer& operator=(event_observer const&)  = delete;
    event_observer& operator=(event_observer&&)       = delete;

    /**
     * The BLE stack has request a block of memory.
     * @param connection_handle The associated connection handle.
     * @param memory_type       @see ble::common::memory_type
     * @param memory_length     The number of bytes to allocate.
     * @param memory_alignment  The alignment required by the allocation.
     */
    virtual void memory_request(uint16_t        connection_handle,
                                memory_type     memory_type,
                                size_t          memory_length,
                                uint16_t        memory_alignment);

    /**
     * The BLE stack has released a block of memory.
     * @param connection_handle The associated connection handle.
     * @param memory_type      @see ble::common::memory_type
     * @param memory_address   The base memory address that is to be freed.
     * @param memory_length    The size in bytes of the allocation being freed.
     */
    virtual void memory_release(uint16_t        connection_handle,
                                memory_type     memory_type,
                                void*           memory_address,
                                std::size_t     memory_length);
};

} // namespace common
} // namespace ble


