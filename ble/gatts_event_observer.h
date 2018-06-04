/**
 * @file ble/gatts_event_observer.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Generic GATTS BLE event observer interface.
 */

#pragma once



namespace ble
{

/**
 * @todo Things needed:
 * - conection handle or connection association
 *
 */
class gatts_event_observer
{
    virtual ~gatts_event_observer()                              = default;
    gatts_event_observer();

    gatts_event_observer(gatts_event_observer const&)            = delete;
    gatts_event_observer(gatts_event_observer &&)                = delete;
    gatts_event_observer& operator=(gatts_event_observer const&) = delete;
    gatts_event_observer& operator=(gatts_event_observer&&)      = delete;


    virtual size_t read(uint16_t attr_handle) = 0;
    virtual size_t write(uint16_t attr_handle) = 0;
    virtual bool rw_authorize_request(uint16_t attr_handle) = 0;

};


}

