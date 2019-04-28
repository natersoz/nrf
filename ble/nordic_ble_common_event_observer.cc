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

void event_observer::memory_request(uint16_t            conection_handle,
                                    memory_type         memory_type,
                                    size_t              length,
                                    uint16_t            alignment)
{
    logger::instance().debug("memory_request(0x%04x, %u)", conection_handle, memory_type);

    uint32_t error_code = sd_ble_user_mem_reply(conection_handle, nullptr);
    ASSERT(error_code == NRF_SUCCESS);
}

void event_observer::memory_release(uint16_t            conection_handle,
                                    memory_type         memory_type,
                                    void*               memory_address,
                                    std::size_t         memory_length)
{
    logger::instance().error("memory_release(): unexpected call");
}

} // namespace common
} // namespace ble


