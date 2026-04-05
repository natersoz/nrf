/**
 * @file ble/common_event_observer.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "ble/common_event_observer.h"
#include "logger.h"
#include "project_assert.h"

#include <nrf_error.h>
#include <ble.h>

namespace ble
{
namespace common
{

void event_observer::memory_request(uint16_t            connection_handle,
                                    memory_type         memory_type,
                                    size_t              length,
                                    uint16_t            alignment)
{
    (void)length;
    (void)alignment;
    logger::instance().debug("memory_request(0x%04x, %u)", connection_handle, memory_type);

    uint32_t error_code = sd_ble_user_mem_reply(connection_handle, nullptr);
    ASSERT(error_code == NRF_SUCCESS);
    (void)error_code;
}

void event_observer::memory_release(uint16_t            connection_handle,
                                    memory_type         memory_type,
                                    void*               memory_address,
                                    std::size_t         memory_length)
{
    (void)connection_handle;
    (void)memory_type;
    (void)memory_address;
    (void)memory_length;

    logger::instance().error("memory_release(): unexpected call");
}

} // namespace common
} // namespace ble
