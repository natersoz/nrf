/**
 * @file nordic_ble_stack_observer.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "ble.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"

#include "ble/nordic_ble_event_strings.h"
#include "section_macros.h"
#include "logger.h"
#include "project_assert.h"

#include <cstdint>

/// @todo reconcile Nordic GATT MTU max size with nordic::ble::stack
/// and ble::att::mtu_length_maximum
/// Here setting it to 252 for alignment (should not be necessary).
/// @see github issue #22 GATT MTU size needs reconciled.
constexpr size_t const gatt_mtu_size_max = 252u;

// Use an native type as a backing store for the event type.
// This will guarantee portable proper alignment.
using event_buffer_type_t = uint32_t;

constexpr size_t const event_buffer_byte_size =
    BLE_EVT_LEN_MAX(gatt_mtu_size_max);

constexpr size_t const event_buffer_size =
    (event_buffer_byte_size / sizeof(event_buffer_type_t)) + 1u;

/**
 * Notify BLE observers registered in linker section .sdh_ble_observers
 * of the BLE events received from the Nordic softdevice.
 *
 * @param ble_event_ptr A pointer to the BLE event.
 */
static void event_notify(ble_evt_t const* ble_event_ptr)
{
    /// Locate the start and stop of the linker section .sdh_ble_observers.
    /// The observers registered in this section are notified of BLE events.
    /// @note __stop_sdh_ble_observers is inclusive of the last node in the
    /// section data and therefore needs to be included in the comparison.
    /// i.e. <=, not < .
    extern uint32_t __start_sdh_ble_observers;
    extern uint32_t __stop_sdh_ble_observers;

    nrf_sdh_ble_evt_observer_t *observers_begin = reinterpret_cast<nrf_sdh_ble_evt_observer_t *>(&__start_sdh_ble_observers);
    nrf_sdh_ble_evt_observer_t *observers_end   = reinterpret_cast<nrf_sdh_ble_evt_observer_t *>(&__stop_sdh_ble_observers);

    for (nrf_sdh_ble_evt_observer_t *observer = observers_begin;
         observer <= observers_end; ++observer)
    {
        // No need to check for all-FF's as uninitialized FLASH.
        // The section is only as long as the number of nodes placed in it.
        if (observer && observer->handler)
        {
            observer->handler(ble_event_ptr, observer->p_context);
        }
    }
}

/**
 * Receive all Nordic softdevice BLE stack events and dispatch these same
 * events to the linker seccton .sdh_ble_observers for these observers to
 * receive them.
 *
 * @note While this is an extra layer of processing (we could call
 * nordic_ble_event_handler() driectly with this data) encapuslating this
 * stack event in one place makes sense.
 */
static void nordic_stack_event_handler(void* context)
{
    (void) context;
    logger& logger = logger::instance();

    /// @todo Consider making this static, global to this module.
    event_buffer_type_t ble_event_buffer[event_buffer_size];

    ble_evt_t const* ble_event_ptr = reinterpret_cast<ble_evt_t const*>(ble_event_buffer);
    uint16_t         ble_event_len = static_cast<uint16_t>(sizeof(ble_event_buffer));

    for (;;)
    {
        uint32_t const event_result = sd_ble_evt_get(
            reinterpret_cast<uint8_t *>(ble_event_buffer), &ble_event_len);

        switch (event_result)
        {
        case NRF_SUCCESS:
            // The event was recieved from the Noridc BLE stack. Process it.
            // Don't log anything here; debug logging will be handled in
            // nordic_ble_event_handler() which is dispatched in the section
            // ".sdh_ble_observers".
            // Unless you're paranoid. Then uncomment the following line:
            logger.debug("Nordic BLE event: 0x%02x %s",
                         ble_event_ptr->header.evt_id,
                         nordic::event_string(ble_event_ptr->header.evt_id));
            event_notify(ble_event_ptr);
            break;

        case NRF_ERROR_NOT_FOUND:
            // The BLE event queue is empty. Done processing events for now.
            return;

        case NRF_ERROR_DATA_SIZE:
            logger.error(
                "nordic_stack_event_handler(): buffer too small: 0x%p, %u / %u",
                ble_event_buffer, sizeof(ble_event_buffer), ble_event_len);
            ASSERT(0);
            break;

        case NRF_ERROR_INVALID_ADDR:
            logger.error(
                "nordic_stack_event_handler(): alignment error: 0x%p, %u",
                ble_event_buffer, sizeof(ble_event_buffer));
            ASSERT(0);
            break;

        default:
            // Uknown event_result code.
            logger.error(
                "nordic_stack_event_handler(): event result code: %u 0x%x",
                event_result, event_result);
            ASSERT(0);
            break;
        }
    }
}

static nrf_sdh_stack_observer_t sdh_stack_event_observer
    IN_SECTION(".sdh_stack_observers") =
{
    .handler    = nordic_stack_event_handler,
    .p_context  = nullptr
};
