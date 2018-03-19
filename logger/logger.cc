/**
 * @file logger.cc
 */

#include "logger.h"
#include "logger_c.h"
#include "vwritef.h"
#include "write_data.h"
#include "app_timer.h"

static logger logger_instance;

logger& logger::instance()
{
    return logger_instance;
}

static uint32_t app_timer_ticks_to_msec(uint32_t ticks)
{
    uint64_t msec = ticks;
    msec *= APP_TIMER_CONFIG_RTC_FREQUENCY + 1u;
    msec *= 1000;
    msec /= APP_TIMER_CLOCK_FREQ;
    return (uint32_t) msec;
}

size_t log_app_time_msec(output_stream &os)
{
    uint32_t const timer_ticks = app_timer_cnt_get();
    uint32_t const timer_msec  = app_timer_ticks_to_msec(timer_ticks);

    return writef(os, "%6u.%03u ", timer_msec / 1000u, timer_msec % 1000u);
}

size_t logger::error(char const *fmt, ...)
{
    size_t n_written = 0u;
    if (this->os_ != nullptr)
    {
        n_written += log_app_time_msec(*this->os_);

        va_list args;
        va_start(args, fmt);
        n_written += vwritef(*this->os_, fmt, args);
        va_end(args);

        char const new_line = '\n';
        n_written += this->os_->write(&new_line, sizeof(new_line));
    }

    return n_written;
}

size_t logger::warn( char const *fmt, ...)
{
    size_t n_written = 0u;
    if (this->os_ != nullptr)
    {
        if (this->log_level_ >= level::warning)
        {
            n_written += log_app_time_msec(*this->os_);

            va_list args;
            va_start(args, fmt);
            n_written = vwritef(*this->os_, fmt, args);
            va_end(args);

            char const new_line = '\n';
            n_written += this->os_->write(&new_line, sizeof(new_line));
        }
    }

    return n_written;
}

size_t logger::info( char const *fmt, ...)
{
    size_t n_written = 0u;
    if (this->os_ != nullptr)
    {
        if (this->log_level_ >= level::info)
        {
            n_written += log_app_time_msec(*this->os_);

            va_list args;
            va_start(args, fmt);
            n_written = vwritef(*this->os_, fmt, args);
            va_end(args);

            char const new_line = '\n';
            n_written += this->os_->write(&new_line, sizeof(new_line));
        }
    }

    return n_written;
}

size_t logger::debug(char const *fmt, ...)
{
    size_t n_written = 0u;
    if (this->os_ != nullptr)
    {
        if (this->log_level_ >= level::debug)
        {
            n_written += log_app_time_msec(*this->os_);

            va_list args;
            va_start(args, fmt);
            n_written = vwritef(*this->os_, fmt, args);
            va_end(args);

            char const new_line = '\n';
            n_written += this->os_->write(&new_line, sizeof(new_line));
        }
    }

    return n_written;
}

size_t logger::write(level log_level, char const *fmt, ...)
{
    size_t n_written = 0u;
    if (this->os_ != nullptr)
    {
        if (this->log_level_ >= log_level)
        {
            n_written += log_app_time_msec(*this->os_);

            va_list args;
            va_start(args, fmt);
            n_written = vwritef(*this->os_, fmt, args);
            va_end(args);

            char const new_line = '\n';
            n_written += this->os_->write(&new_line, sizeof(new_line));
        }
    }

    return n_written;
}

size_t logger::vwrite(level log_level, char const *fmt, va_list args)
{
    size_t n_written = 0u;
    if (this->os_ != nullptr)
    {
        if (this->log_level_ >= log_level)
        {
            n_written += log_app_time_msec(*this->os_);

            n_written = vwritef(*this->os_, fmt, args);
            char const new_line = '\n';
            n_written += this->os_->write(&new_line, sizeof(new_line));
        }
    }

    return n_written;
}

void logger::flush()
{
    this->os_->flush();
}

size_t logger::write_data(level                     log_level,
                          void const                *data,
                          size_t                    length,
                          bool                      char_data,
                          write_data::data_prefix   prefix)
{
    size_t n_written = 0u;
    if (this->os_ != nullptr)
    {
        if (this->log_level_ >= log_level)
        {
            n_written = write_data::write_data(*this->os_, data, length, char_data, prefix);
        }
    }

    return n_written;
}

