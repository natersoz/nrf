/**
 * @file nordic_ble_event_observables.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "ble/profile_connectable.h"

#include "nordic_ble_event_observer.h"
#include "section_macros.h"
#include "project_assert.h"
#include "logger.h"

#include <nrf_sdh_ble.h>

namespace nordic
{

static ble::profile::connectable::container ble_connectable_container;
static ble::common::event_observer          nordic_common_event_observer;

void register_ble_connectable(ble::profile::connectable& ble_connectable)
{
    if (not ble_connectable.is_linked())
    {
        ble_connectable_container.push_back(ble_connectable);
    }
    else
    {
        logger::instance().warn(
            "register_ble_connectable(0x%p) connectable already connected",
            &ble_connectable);
    }

    ble_connectable_container.push_back(ble_connectable);
}

void deregister_ble_connectable(ble::profile::connectable& ble_connectable)
{
    if (ble_connectable.is_linked())
    {
        ble_connectable.unlink();
    }
    else
    {
        logger::instance().warn(
            "deregister_ble_connectable(0x%p) connectable not connected",
            &ble_connectable);
    }
}

static void nordic_ble_event_handler(ble_evt_t const *ble_event, void *context)
{
    ble::profile::connectable::container* container =
        reinterpret_cast<ble::profile::connectable::container*>(context);
    ASSERT(container == &ble_connectable_container);

    if ((ble_event->header.evt_id >= BLE_EVT_BASE) &&   // Common BLE events.
        (ble_event->header.evt_id <= BLE_EVT_LAST))
    {
        nordic::ble_common_event_notify(
            nordic_common_event_observer,
            static_cast<enum BLE_COMMON_EVTS>(ble_event->header.evt_id),
            ble_event->evt.common_evt);
    }
    else if ((ble_event->header.evt_id >= BLE_GAP_EVT_BASE) &&
             (ble_event->header.evt_id <= BLE_GAP_EVT_LAST))
    {
        for (auto iter  = ble_connectable_container.begin();
                  iter != ble_connectable_container.end(); )
        {
            // Increment the iterator prior to using it.
            // If the client removes itself during the notification callback
            // then the iterator will remain valid and can continue.
            ble::profile::connectable& connectable = *iter;
            ++iter;

            /// @todo Check connection handle and only notify the connectable
            /// object for which the connection handle matches. (?)
            /// If the connection handle of the connectable is invalid then
            /// notify since it is not yet connected and this might be its
            /// connection notification.
            nordic::ble_gap_event_notify(
                connectable.gap,
                static_cast<enum BLE_GAP_EVTS>(ble_event->header.evt_id),
                ble_event->evt.gap_evt);
        }
    }
    else if ((ble_event->header.evt_id >= BLE_GATTC_EVT_BASE) &&
             (ble_event->header.evt_id <= BLE_GATTC_EVT_LAST))
    {
        enum BLE_GATTC_EVTS const gattc_event =
            static_cast<enum BLE_GATTC_EVTS>(ble_event->header.evt_id);

        for (auto iter  = ble_connectable_container.begin();
                  iter != ble_connectable_container.end(); )
        {
            // Increment the iterator prior to using it.
            // If the client removes itself during the notification callback
            // then the iterator will remain valid and can continue.
            ble::profile::connectable& connectable = *iter;
            ++iter;

            // Note that GATTC events are broken up into 2 parts:
            // gattc responses and discovery responses.
            // They are demultiplexed here into separate observers.
            // The Nordic response for reading by UUID marks the beginning
            // of the non-discovery responses.
            if (gattc_event >= BLE_GATTC_EVT_CHAR_VAL_BY_UUID_READ_RSP)
            {
                if (connectable.gattc.event_observer)
                {
                    ble_gattc_event_notify(*connectable.gattc.event_observer,
                                           gattc_event,
                                           ble_event->evt.gattc_evt);
                }
            }
            else
            {
                if (connectable.gattc.service_builder)
                {
                    ble_discovery_response(*connectable.gattc.service_builder,
                                           gattc_event,
                                           ble_event->evt.gattc_evt);
                }
            }
        }
    }
    else if ((ble_event->header.evt_id >= BLE_GATTS_EVT_BASE) &&
             (ble_event->header.evt_id <= BLE_GATTS_EVT_LAST))
    {
        enum BLE_GATTS_EVTS const gatts_event =
            static_cast<enum BLE_GATTS_EVTS>(ble_event->header.evt_id);

        for (auto iter  = ble_connectable_container.begin();
                  iter != ble_connectable_container.end(); )
        {
            // Increment the iterator prior to using it.
            // If the client removes itself during the notification callback
            // then the iterator will remain valid and can continue.
            ble::profile::connectable& connectable = *iter;
            ++iter;

            // Note that GATTC events are broken up into 2 parts:
            // gattc responses and discovery responses.
            // They are demultiplexed here into separate observers.
            // The Nordic response for reading by UUID marks the beginning
            // of the non-discovery responses.
            if (connectable.gatts.event_observer)
            {
                ble_gatts_event_notify(*connectable.gatts.event_observer,
                                       gatts_event,
                                       ble_event->evt.gatts_evt);
            }
        }
    }
    else
    {
        logger::instance().warn("Unknown nordic BLE event: 0x%02x",
                              ble_event->header.evt_id);
    }
}

// For whatever reason clang-tidy is not accepting the reason in parens.
// NOLINTNEXTLINE clang-diagnostic-unused-const-variable
static nrf_sdh_ble_evt_observer_t sdh_ble_event_observer
    IN_SECTION(".sdh_ble_observers") =
{
    .handler    = nordic_ble_event_handler,
    .p_context  = &ble_connectable_container
};

} // namespace nordic
