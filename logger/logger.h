/**
 * @file logger.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
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
        always = 0,
        error,
        warning,
        info,
        debug
    };

    ~logger()                               = default;
    logger(logger const& other)             = delete;
    logger& operator=(logger const& other)  = delete;

    static logger& instance();
    logger() : os_(nullptr), rtc_(nullptr), log_level_(logger::level::warning)
    {
    }
/*
    logger(output_stream& os,
           level log_level = level::warning)
        : os_(&os), log_level_(log_level) {}
*/
    size_t error(char const* fmt, ...);
    size_t warn(char const* fmt, ...);
    size_t info(char const* fmt, ...);
    size_t debug(char const* fmt, ...);

    size_t write(logger::level log_level, char const* fmt, ...);

    /// Write a log entry without respect to any level. Always writes.
    size_t write(char const* fmt, ...);

    size_t vwrite(logger::level log_level, char const* fmt, va_list& args);

    void flush();

    size_t write_data(
        logger::level   log_level,
        void const*     data,
        size_t          length,
        bool            char_data   = false,
        io::data_prefix prefix      = io::data_prefix::index);

    size_t write_data(
        logger::level           log_level,
        void const volatile*    data,
        size_t                  length,
        bool                    char_data   = false,
        io::data_prefix         prefix      = io::data_prefix::index);

    void set_output_stream(io::output_stream& os) { this->os_ = &os; }

    io::output_stream* get_output_stream() { return this->os_; }

    void set_level(logger::level log_level) { this->log_level_ = log_level; }
    logger::level get_level() const { return this->log_level_; }

    void set_rtc(rtc& rtc) { this->rtc_ = &rtc; }

private:
    io::output_stream*  os_;
    rtc*                rtc_;
    logger::level       log_level_;

    size_t log_time();
    size_t write_preamble(logger::level log_level);
    size_t write_postamble(logger::level log_level);
};

inline bool operator == (logger::level log_level_1, logger::level log_level_2)
{
    return static_cast<int>(log_level_1) == static_cast<int>(log_level_2);
}

inline bool operator != (logger::level log_level_1, logger::level log_level_2)
{
    return static_cast<int>(log_level_1) != static_cast<int>(log_level_2);
}

inline bool operator < (logger::level log_level_1, logger::level log_level_2)
{
    return static_cast<int>(log_level_1) < static_cast<int>(log_level_2);
}

inline bool operator <= (logger::level log_level_1, logger::level log_level_2)
{
    return static_cast<int>(log_level_1) <= static_cast<int>(log_level_2);
}

inline bool operator > (logger::level log_level_1, logger::level log_level_2)
{
    return static_cast<int>(log_level_1) > static_cast<int>(log_level_2);
}

inline bool operator >= (logger::level log_level_1, logger::level log_level_2)
{
    return static_cast<int>(log_level_1) >= static_cast<int>(log_level_2);
}
