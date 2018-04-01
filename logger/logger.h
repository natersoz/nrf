/**
 * @file logger.h
 *
 * @todo @bug logger::writer_ should be atomic for thread/ISR safety.
 * log_level should also, but is not fatal.
 */

#pragma once

#include "rtc.h"
#include "stream.h"
#include "write_data.h"

#include <cstdarg>
#include <cstddef>
#include <type_traits>

class logger
{
public:
    enum class level
    {
        error = 0,
        warning,
        info,
        debug
    };

    ~logger()                               = default;
    logger(logger const& other)             = delete;
    logger& operator=(logger const& other)  = delete;

    static logger& instance();
    logger(): os_(nullptr), rtc_(nullptr), log_level_(level::warning) {}
/*
    logger(output_stream& os,
           level log_level = level::warning)
        : os_(&os), log_level_(log_level) {}
*/
    size_t error(char const *fmt, ...);
    size_t warn( char const *fmt, ...);
    size_t info( char const *fmt, ...);
    size_t debug(char const *fmt, ...);

    size_t write(level log_level, char const *fmt, ...);
    size_t vwrite(level log_level, char const *fmt, va_list args);

    void   flush();

    size_t write_data(level                   log_level,
                      void const              *data,
                      size_t                  length,
                      bool                    char_data = false,
                      write_data::data_prefix prefix    = write_data::data_prefix::index);

    void set_output_stream(output_stream& os)
    {
        this->os_ = &os;
    }

    void set_level(level log_level)
    {
        this->log_level_ = log_level;
    }

    void set_rtc(rtc &rtc)
    {
        this->rtc_ = &rtc;
    }

private:
    output_stream   *os_;
    rtc             *rtc_;
    level           log_level_;

    size_t log_time();

};

