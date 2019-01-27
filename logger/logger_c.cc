/**
 * @file logger_c.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * C interface to the logger.
 */

#include "logger_c.h"
#include "logger.h"
#include "rtt_output_stream.h"

// static char rtt_os_buffer[4096u];
// static rtt_output_stream rtt_os(rtt_os_buffer, sizeof(rtt_os_buffer));

int logger_init()
{
    logger& logger = logger::instance();
    logger.set_level(logger::level::debug);
//    logger.set_output_stream(rtt_os);
    return 0;
}

size_t logger_write(enum logger_level log_level, char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    size_t n_write = logger::instance().vwrite(static_cast<logger::level>(log_level), fmt, args);

    va_end(args);
    return n_write;
}

void logger_process(void)
{
    logger::instance().flush();
}

void logger_flush(void)
{
    logger::instance().flush();
}

size_t logger_write_data(enum logger_level log_level,
                         void const        *data,
                         size_t            length,
                         bool              char_data,
                         enum data_prefix  prefix)
{
    return logger::instance().write_data(static_cast<logger::level>(log_level),
                                         data,
                                         length,
                                         char_data,
                                         static_cast<io::data_prefix>(prefix));
}


