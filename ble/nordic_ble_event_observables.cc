/**
 * @file nordic_ble_event_observables.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "nordic_ble_event_observable.h"
#include "nrf_sdh_ble.h"
#include "section_macros.h"
#include "project_assert.h"

// Instantiate the nordic::ble_obseverables class instance as a singleton.
static nordic::ble_observables ble_observables_instance;

// Allow access to the nordic::ble_obseverables singleton.
nordic::ble_observables& nordic::ble_observables::instance()
{
    return ble_observables_instance;
}

static void nordic_ble_event_handler(ble_evt_t const *ble_event, void *context)
{
    auto *ble_observables = reinterpret_cast<nordic::ble_observables*>(context);
    ASSERT(ble_observables == &ble_observables_instance);

    if ((ble_event->header.evt_id >= BLE_EVT_BASE) &&   // Common BLE events.
        (ble_event->header.evt_id <= BLE_EVT_LAST))
    {
        ble_observables->common_event_observable.notify(
            static_cast<enum BLE_COMMON_EVTS>(ble_event->header.evt_id),
            ble_event->evt.common_evt);

    }
    else if ((ble_event->header.evt_id >= BLE_GAP_EVT_BASE) &&
             (ble_event->header.evt_id <= BLE_GAP_EVT_LAST))
    {
        ble_observables->gap_event_observable.notify(
            static_cast<enum BLE_GAP_EVTS>(ble_event->header.evt_id),
            ble_event->evt.gap_evt);
    }
    else if ((ble_event->header.evt_id >= BLE_GATTC_EVT_BASE) &&
             (ble_event->header.evt_id <= BLE_GATTC_EVT_LAST))
    {
        enum BLE_GATTC_EVTS const gattc_event =
            static_cast<enum BLE_GATTC_EVTS>(ble_event->header.evt_id);

        // Note that GATTC events are broken up into 2 parts within this C++
        // framework: gattc responses and discovery responses.
        // They are demultiplexed here into separate observers.
        // The Nordic response for reading by UUID marks the beginning of
        // the non-discovery responses.
        if (gattc_event >= BLE_GATTC_EVT_CHAR_VAL_BY_UUID_READ_RSP)
        {
            ble_observables->gattc_event_observable.notify(
                gattc_event, ble_event->evt.gattc_evt);
        }
        else
        {
            ble_observables->gattc_discovery_observable.notify(
                gattc_event, ble_event->evt.gattc_evt);
        }
    }
    else if ((ble_event->header.evt_id >= BLE_GATTS_EVT_BASE) &&
             (ble_event->header.evt_id <= BLE_GATTS_EVT_LAST))
    {
        ble_observables->gatts_event_observable.notify(
            static_cast<enum BLE_GATTS_EVTS>(ble_event->header.evt_id),
            ble_event->evt.gatts_evt);
    }
}

static nrf_sdh_ble_evt_observer_t sdh_ble_event_observer
    IN_SECTION(".sdh_ble_observers") =
{
    .handler    = nordic_ble_event_handler,
    .p_context  = &ble_observables_instance
};

