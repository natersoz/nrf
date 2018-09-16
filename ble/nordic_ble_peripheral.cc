/**
 * @file ble/nordic_ble_peripheral.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Implementation of the ble/peripheral class for the Nordic nrf5x device.
 */

#include "ble/peripheral.h"
#include "project_assert.h"

#include "ble_gap.h"                // Nordic softdevice headers

/**
 * GAP functionality:
 * - Tx power
 * - Report received RSSI?
 * - Connection parameters
 * - Connection maintenance
 *      - Connection PPCP
 *      - Advertising
 *      - Disconnect
 * - MTU negotiation
 */


namespace nordic
{
    void ble_peripheral::connection_parameters(
        gap::connection_parameters const& connection_parameters)
    {
        // Call the base class to set the member data.
        ble::peripheral::connection_parameters(connection_parameters);


        uint32_t error_code = NRF_SUCCESS;


    }

    void ble_peripheral::disconnect(ble::hci::error_code reason)
    {
    }



    set_gap_connection_parameters





} // namespace nordic
