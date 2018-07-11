/**
 * @file ble/gatts_event_observer.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Generic GATTS BLE event observer interface.
 */

#pragma once

namespace ble
{
namespace gatts
{

class event_observer
{
public:
    virtual ~event_observer()                        = default;
    event_observer()                                 = default;

    event_observer(event_observer const&)            = delete;
    event_observer(event_observer &&)                = delete;
    event_observer& operator=(event_observer const&) = delete;
    event_observer& operator=(event_observer&&)      = delete;

    virtual size_t read(uint16_t attr_handle) = 0;
    virtual size_t write(uint16_t attr_handle) = 0;
    virtual bool rw_authorize_request(uint16_t attr_handle) = 0;
};

} // namespace gatts
} // namespace ble

