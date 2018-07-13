/**
 * @file ble/peripheral_trivial.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "ble/peripheral.h"

namespace ble
{
    ble::gattc::event_observer peripheral::ble_gattc_event_observer_trivial;
    ble::gatts::event_observer peripheral::ble_gatts_event_observer_trivial;
}
