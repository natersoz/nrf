/**
 * @file nordic_error.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "nordic_error.h"
#include "nrf_error.h"
#include "ble_err.h"
#include "logger.h"

std::errc nordic_to_system_error(uint32_t nordic_error)
{
    std::errc error_code = static_cast<std::errc>(0);

    logger &logger = logger::instance();

    switch (nordic_error)
    {
    case NRF_SUCCESS:
        error_code = static_cast<std::errc>(0);
        break;
    case NRF_ERROR_SVC_HANDLER_MISSING:
        error_code = std::errc::no_such_device_or_address;
        break;
    case NRF_ERROR_SOFTDEVICE_NOT_ENABLED:
        error_code = std::errc::network_down;
        break;
    case NRF_ERROR_INTERNAL:
        error_code = std::errc::owner_dead;
        break;
    case NRF_ERROR_NO_MEM:
        error_code = std::errc::not_enough_memory;
        break;
    case NRF_ERROR_NOT_FOUND:
        error_code = std::errc::no_message;
        break;
    case NRF_ERROR_NOT_SUPPORTED:
        error_code = std::errc::not_supported;
        break;
    case NRF_ERROR_INVALID_PARAM:
        error_code = std::errc::invalid_argument;
        break;
    case NRF_ERROR_INVALID_STATE:
        error_code = std::errc::state_not_recoverable;
        break;
    case NRF_ERROR_INVALID_LENGTH:
        error_code = std::errc::invalid_argument;
        break;
    case NRF_ERROR_INVALID_FLAGS:
        error_code = std::errc::invalid_argument;
        break;
    case NRF_ERROR_INVALID_DATA:
        error_code = std::errc::invalid_argument;
        break;
    case NRF_ERROR_DATA_SIZE:
        error_code = std::errc::invalid_argument;
        break;
    case NRF_ERROR_TIMEOUT:
        error_code = std::errc::timed_out;
        break;
    case NRF_ERROR_NULL:
        error_code = std::errc::invalid_argument;
        break;
    case NRF_ERROR_FORBIDDEN:
        error_code = std::errc::operation_not_permitted;
        break;
    case NRF_ERROR_INVALID_ADDR:
        error_code = std::errc::bad_address;
        break;
    case NRF_ERROR_BUSY:
        error_code = std::errc::device_or_resource_busy;
        break;
    case NRF_ERROR_CONN_COUNT:
        error_code = std::errc::too_many_links;
        break;
    case BLE_ERROR_INVALID_CONN_HANDLE:
        error_code = std::errc::not_connected;
        break;
    default:
        error_code = static_cast<std::errc>(-1);
        break;
    }

    if (nordic_error != NRF_SUCCESS)
    {
        logger.error("Nordic error: 0x%04x '%s'",
                     nordic_error, nordic_error_string(nordic_error));
    }

    return error_code;
}

char const* nordic_error_string(uint32_t nordic_error)
{
    switch (nordic_error)
    {
    case NRF_SUCCESS:                           return "success";
    case NRF_ERROR_SVC_HANDLER_MISSING:         return "SVC handler missing";
    case NRF_ERROR_SOFTDEVICE_NOT_ENABLED:      return "softdevice not enabled";
    case NRF_ERROR_INTERNAL:                    return "internal Error";
    case NRF_ERROR_NO_MEM:                      return "no memory";
    case NRF_ERROR_NOT_FOUND:                   return "not found";
    case NRF_ERROR_NOT_SUPPORTED:               return "not supported";
    case NRF_ERROR_INVALID_PARAM:               return "invalid parameter";
    case NRF_ERROR_INVALID_STATE:               return "invalid state";
    case NRF_ERROR_INVALID_LENGTH:              return "invalid length";
    case NRF_ERROR_INVALID_FLAGS:               return "invalid flags";
    case NRF_ERROR_INVALID_DATA:                return "invalid data";
    case NRF_ERROR_DATA_SIZE:                   return "invalid data size";
    case NRF_ERROR_TIMEOUT:                     return "timeout";
    case NRF_ERROR_NULL:                        return "null pointer";
    case NRF_ERROR_FORBIDDEN:                   return "forbidden";
    case NRF_ERROR_INVALID_ADDR:                return "invalid memory";
    case NRF_ERROR_BUSY:                        return "busy";
    case NRF_ERROR_CONN_COUNT:                  return "too many connections";
    case NRF_ERROR_RESOURCES:                   return "no resources";
    default:                                    return "unknown error";
    }
}
