/**
 * @file nordic_ble_gatts_event_observer.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "ble/gatts_event_observer.h"
#include "nrf_error.h"
#include "logger.h"
#include "project_assert.h"

#include "ble_gatts.h"

namespace ble {
namespace gatts {

/**
 * Provide default handling for the Nordic specific BLE_GATTS_EVT_SYS_ATTR_MISSING.
 * There is very little documentation related to what this message means or
 * how its response function sd_ble_gatts_sys_attr_set() is to be used.
 * The default usage of (null, 0, 0) appears to clear the persistent storage
 * for the connection.
 *
 * Hopefully this never gets called.
 */
void event_observer::system_attribute_missing(uint16_t conection_handle,
                                              uint8_t  hint)
{
    uint8_t const error_code = sd_ble_gatts_sys_attr_set(conection_handle, nullptr, 0, 0);

    if (error_code != NRF_SUCCESS)
    {
        logger::instance().error(
            "GATTS system_attribute_missing: sd_ble_gatts_sys_attr_set() failed: %u", error_code);
        ASSERT(0);
    }
}

} // namespace gatts
} // namespace ble

