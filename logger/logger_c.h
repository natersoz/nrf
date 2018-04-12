/**
 * @file logger_c.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * C interface to the logger.
 */

#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

enum logger_level
{
    logger_level_error = 0,
    logger_level_warning,
    logger_level_info,
    logger_level_debug
};

enum data_prefix
{
    data_prefix_none = 0,       ///< No prefix written.
    data_prefix_index,          ///< An index into the data, starting with zero.
    data_prefix_address         ///< The data address.
};

int    logger_init();

size_t logger_write(enum logger_level log_level, char const *fmt, ...);
size_t logger_process(void);    ///< @return the number of bytes pending output.
void   logger_flush(void);

size_t logger_write_data(enum logger_level log_level,
                         void const        *data,
                         size_t            length,
                         bool              char_data,
                         enum data_prefix  prefix);

#ifdef __cplusplus
}
#endif

