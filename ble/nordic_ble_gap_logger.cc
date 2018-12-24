/**
 * @file nordic_ble_gap_logger.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Debug logging functions for nordic::ble_gap related stuff
 */

#include "ble_gap.h"                    // Nordic softdevice header
#include "logger.h"

namespace nordic
{

char const* ble_gap_addr_type_string(uint8_t addr_type)
{
    switch (addr_type)
    {
    case BLE_GAP_ADDR_TYPE_PUBLIC:
        return "public";
    case BLE_GAP_ADDR_TYPE_RANDOM_STATIC:
        return "random static";
    case BLE_GAP_ADDR_TYPE_RANDOM_PRIVATE_RESOLVABLE:
        return "random resolvable";
    case BLE_GAP_ADDR_TYPE_RANDOM_PRIVATE_NON_RESOLVABLE:
        return "random non-resolvable";
    case BLE_GAP_ADDR_TYPE_ANONYMOUS:
        return "anonymous";
    default:
        return "invalid";
    }
}

void log_address(logger::level log_level, ble_gap_addr_t const &addr)
{
    logger::instance().write(
        log_level, "%02x:%02x:%02x:%02x:%02x:%02x, peer_id: %u, type: %u '%s'",
        addr.addr[0u], addr.addr[1u], addr.addr[2u],
        addr.addr[3u], addr.addr[4u], addr.addr[5u],
        addr.addr_id_peer, addr.addr_type,
        ble_gap_addr_type_string(addr.addr_type));
}

void log_connection_parameters(logger::level                log_level,
                               ble_gap_conn_params_t const& conn_params)
{
    logger::instance().debug(
        "connection parameters: interval (min: %u, max: %u), latency: %u, timeout: %u",
        conn_params.min_conn_interval,
        conn_params.max_conn_interval,
        conn_params.slave_latency,
        conn_params.conn_sup_timeout);
}

void log_scan_parameters(logger::level                log_level,
                         ble_gap_scan_params_t const& scan_params)
{
    logger::instance().debug(
        "scan parameters: interval: %u, window: %u, timeout: %u",
        scan_params.interval,
        scan_params.window,
        scan_params.timeout);
    logger::instance().debug(
        "scan parameters: ext: %u, inc: %u, active: %u, fp: %u, phys: %u",
        scan_params.extended,
        scan_params.report_incomplete_evts,
        scan_params.active,
        scan_params.filter_policy,
        scan_params.scan_phys);
    logger::instance().debug("scan parameters: mask: %02x%02x%02x%02x%02x",
                             scan_params.channel_mask[0u],
                             scan_params.channel_mask[1u],
                             scan_params.channel_mask[2u],
                             scan_params.channel_mask[3u],
                             scan_params.channel_mask[4u]);
}

} // namespace nordic
