/**
 * @file ble_connection.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 */

#pragma once
#include <cstdint>
#include <cstddef>

struct connection_params
{
    /// Minimum connection interval, 1.25 msec units
    uint16_t interval_min;

    /// Maximum connection interval, 1.25 msec units
    uint16_t interval_max;

    /**
     * Slave Latency: The maximum number of conection intervals
     * (i.e. connection events) for which the slave (peripheral)
     * must respond to the master (central).
     */
    uint16_t slave_latency;

    /**
     * The conenction supervisor timeout in milliseconds.
     *
     * @note If both conn_sup_timeout and max_conn_interval are specified,
     * then the following constraint applies:
     *  conn_sup_timeout * 4 > (1 + slave_latency) * max_conn_interval
     *  that corresponds to the following Bluetooth Spec requirement:
     *  The Supervision_Timeout in milliseconds shall be larger than
     *  (1 + Conn_Latency) * Conn_Interval_Max * 2,
     *  where Conn_Interval_Max is given in milliseconds.
     */
    uint16_t supervision_timeout;

};

struct connection_limits
{
    uint16_t const interval_min         = 0x0006;       ///< 7.5 msec
    uint16_t const interval_max         = 0x0c80;       ///< 4 seconds
    uint16_t const slave_latency_max    = 0x01f3;       ///< In connection events.
    uint16_t const supvisor_timeout_min = 0x000a;       ///< 100 msec
    uint16_t const supvisor_timeout_min = 0x0c80;       ///< 32 seconds
};

/**
 * @tparam alloc_count The number of connections that can be made simultaneously.
 *
 * @todo
 * ATT_MTU - part of connection?
 *
 */
template<size_t alloc_count>
class ble_connection
{
    static constexpr size_t const length = 6u;

    ~ble_connection()                                   = default;

    ble_connection(ble_connection const&)               = delete;
    ble_connection(ble_connection &&)                   = delete;
    ble_connection& operator=(ble_connection const&)    = delete;
    ble_connection& operator=(ble_connection&&)         = delete;

    ble_connection():
    {
    }

    uint16_t handle;        ///< The connection handle.
};

