/**
 * @file nrf_log.h
 * Macros for faking out the NORDIC LOGGER versions.
 */

#pragma once

#include "logger_c.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NRF_LOG_MODULE_REGISTER()
#define NRF_LOG_INIT(time_callback)             logger_init()

/// @todo modules like nrf_drv_gpiote.c should not depend on
/// nrf_log.h for this definition.
#include "nrf_strerror.h"
#define NRF_LOG_ERROR_STRING_GET(code)          nrf_strerror_get(code)

#define NRF_LOG_ERROR(  fmt, ...)               logger_write(logger_level_error,   fmt, ##__VA_ARGS__)
#define NRF_LOG_WARNING(fmt, ...)               logger_write(logger_level_warning, fmt, ##__VA_ARGS__)
#define NRF_LOG_INFO(   fmt, ...)               logger_write(logger_level_info,    fmt, ##__VA_ARGS__)
#define NRF_LOG_DEBUG(  fmt, ...)               logger_write(logger_level_debug,   fmt, ##__VA_ARGS__)

#define NRF_LOG_HEXDUMP_ERROR(   p_data, len)   logger_write_data(logger_level_error,   p_data, len, false, data_prefix_address)
#define NRF_LOG_HEXDUMP_WARNING( p_data, len)   logger_write_data(logger_level_warning, p_data, len, false, data_prefix_address)
#define NRF_LOG_HEXDUMP_INFO(    p_data, len)   logger_write_data(logger_level_info,    p_data, len, false, data_prefix_address)
#define NRF_LOG_HEXDUMP_DEBUG(   p_data, len)   logger_write_data(logger_level_debug,   p_data, len, false, data_prefix_address)
#define NRF_LOG_RAW_HEXDUMP_INFO(p_data, len)   logger_write_data(logger_level_info,    p_data, len, false, data_prefix_address)

//#define NRF_LOG_GETCHAR()                       NRF_LOG_INTERNAL_GETCHAR()

#define NRF_LOG_PUSH(_str)                      (_str)

#ifdef __cplusplus
}
#endif
