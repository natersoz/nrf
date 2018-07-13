/**
 * @file nordic_ble_event_observer.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Observer interface for receiving Noridc BLE softdevice events.
 */

#pragma once

#include "ble/common_event_observer.h"      // Abstract BLE obsevers.
#include "ble/gap_event_observer.h"
#include "ble/gattc_event_observer.h"
#include "ble/gatts_event_observer.h"

#include "ble.h"                            // Nordic softdevice headers
#include "ble_gap.h"
#include "ble_gattc.h"
#include "ble_gatts.h"

#include <boost/intrusive/list.hpp>

namespace nordic
{

template <typename observer_type>
class ble_event_observable;

template <typename interface_type,
          typename event_enum_type,
          typename event_data_type>
class ble_event_observer
{
public:
    using event_enum_t = event_enum_type;
    using event_data_t = event_data_type;

    virtual ~ble_event_observer()                               = default;

    ble_event_observer()                                        = delete;
    ble_event_observer(ble_event_observer const&)               = delete;
    ble_event_observer(ble_event_observer&&)                    = delete;
    ble_event_observer& operator=(ble_event_observer const &)   = delete;
    ble_event_observer& operator=(ble_event_observer&&)         = delete;

    ble_event_observer(interface_type &interface):
        interface_reference(interface),
        observable_(nullptr) {}

    bool is_attached() const { return bool(this->observable_); }

    /**
     * Since ble_event_observer is not copyable in any form, the means for
     * testing equality is whether they are the same instance in memory.
     */
    bool operator==(ble_event_observer const& other) const { return (this == &other); }

private:
    interface_type &interface_reference;

    /// @todo needs volatile
    boost::intrusive::list_member_hook<> hook_;

    using observable_type =
        ble_event_observable<ble_event_observer<interface_type,
                                                event_enum_type,
                                                event_data_type>>;

    observable_type volatile *observable_;

    friend observable_type;
};

using ble_common_event_observer = ble_event_observer<ble::common::event_observer, enum BLE_COMMON_EVTS, ble_common_evt_t>;
using ble_gap_event_observer    = ble_event_observer<ble::gap::event_observer,    enum BLE_GAP_EVTS,    ble_gap_evt_t>;
using ble_gattc_event_observer  = ble_event_observer<ble::gattc::event_observer,  enum BLE_GATTC_EVTS,  ble_gattc_evt_t>;
using ble_gatts_event_observer  = ble_event_observer<ble::gatts::event_observer,  enum BLE_GATTS_EVTS,  ble_gatts_evt_t>;
// TBD using ble_l2cap_event_observer    = ble_event_observer<enum BLE_L2CAP_EVTS, >;

} // namespace nordic
