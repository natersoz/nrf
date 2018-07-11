/**
 * @file ble/peripheral.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "ble/advertising.h"
#include "ble/gap_types.h"
#include "ble/gap_event_observer.h"
#include "ble/gatts_event_observer.h"
#include "ble/gattc_event_observer.h"
#include "ble/gatt_service.h"
#include "ble/uuid.h"

namespace ble
{

/**
 * @class peripheral
 * Aggregate the specific classes which compose a BLE peripheral
 */
class peripheral
{
public:
    virtual ~peripheral()                     = default;

    peripheral()                              = delete;
    peripheral(peripheral const&)             = delete;
    peripheral(peripheral &&)                 = delete;
    peripheral& operator=(peripheral const&)  = delete;
    peripheral& operator=(peripheral&&)       = delete;

    peripheral(ble::advertising            &advertising,
               ble::gap::event_observer    &gap_event_observer,
               ble::gatts::event_observer  *gatts_event_observer,
               ble::gattc::event_observer  *gattc_event_observer)
    : advertising_(advertising),
      gap_event_observer_(gap_event_observer),
      gatts_event_observer_(gatts_event_observer),
      gattc_event_observer_(gattc_event_observer)
    {
    }

    ble::advertising& advertising_get() {
        return this->advertising_;
    }

    ble::advertising const& advertising_get() const {
        return this->advertising_;
    }

    ble::gap::connection_parameters const& connection_parameters_get() const;
    void connection_parameters_set(ble::gap::connection_parameters const& connection_parameters);

    void service_add(ble::gatt::service const& service);
    ble::gatt::service*       service_get(ble::att::uuid uuid);
    ble::gatt::service const* service_get(ble::att::uuid uuid) const;
    ble::gatt::service*       service_get(ble::gatt::services uuid);
    ble::gatt::service const* service_get(ble::gatt::services uuid) const;

private:
    ble::advertising            &advertising_;
    ble::gap::event_observer    &gap_event_observer_;
    ble::gatts::event_observer  *gatts_event_observer_;
    ble::gattc::event_observer  *gattc_event_observer_;

    ble::gatt::service_container service_container_;
};

} // namespace ble
