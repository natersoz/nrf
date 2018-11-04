/**
 * @file logger.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "logger.h"
#include "logger_c.h"
#include "vwritef.h"
#include "write_data.h"

static constexpr char const error_string[]  = { 'e', 'r', 'r', 'o', 'r', ':', ' ' };
static constexpr char const warn_string[]   = { 'w', 'a', 'r', 'n', 'i', 'n', 'g', ':', ' ' };
static constexpr char const info_string[]   = { 'i', 'n', 'f', 'o', ':', ' ' };
static constexpr char const debug_string[]  = { 'd', 'e', 'b', 'u', 'g', ':', ' ' };

static constexpr char const color_reset_string[]    = { 0x1B, '[', '3', '9', ';', '4', '9', 'm' };
static constexpr char const color_red_string[]      = { 0x1B, '[', '3', '1', 'm' };
static constexpr char const color_yellow_string[]   = { 0x1B, '[', '9', '3', 'm' };

static constexpr size_t const color_reset_string_length = sizeof(color_reset_string);


static logger logger_instance;

logger& logger::instance()
{
    return logger_instance;
}

static uint64_t rtc_ticks_to_msec(uint64_t ticks, uint32_t const ticks_per_sec)
{
    ticks *= 1000u;
    ticks /= ticks_per_sec;
    return ticks;
}

size_t logger::error(char const *fmt, ...)
{
    size_t n_written = 0u;
    if (this->os_ != nullptr)
    {
        n_written += this->log_time();
        n_written += this->os_->write(color_red_string, sizeof(color_red_string));
        n_written += this->os_->write(error_string, sizeof(error_string));

        va_list args;
        va_start(args, fmt);
        n_written += vwritef(*this->os_, fmt, args);
        va_end(args);

        n_written += this->os_->write(color_reset_string, sizeof(color_reset_string));
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
            n_written += this->log_time();
            n_written += this->os_->write(color_yellow_string, sizeof(color_yellow_string));
            n_written += this->os_->write(warn_string, sizeof(warn_string));

            va_list args;
            va_start(args, fmt);
            n_written = vwritef(*this->os_, fmt, args);
            va_end(args);

            n_written += this->os_->write(color_reset_string, sizeof(color_reset_string));
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
            n_written += this->log_time();
            n_written += this->os_->write(info_string, sizeof(info_string));

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
            n_written += this->log_time();
            n_written += this->os_->write(debug_string, sizeof(debug_string));

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
            n_written += this->log_time();

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

size_t logger::write(char const *fmt, ...)
{
    size_t n_written = 0u;
    if (this->os_ != nullptr)
    {
        n_written += this->log_time();

        va_list args;
        va_start(args, fmt);
        n_written = vwritef(*this->os_, fmt, args);
        va_end(args);

        char const new_line = '\n';
        n_written += this->os_->write(&new_line, sizeof(new_line));
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
            n_written += this->log_time();

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

size_t logger::log_time()
{
    if (this->rtc_)
    {
        uint64_t const timer_ticks = this->rtc_->get_count_extend_64();
        uint64_t const timer_msec  = rtc_ticks_to_msec(timer_ticks, this->rtc_->ticks_per_second());
        return writef(*this->os_, "%6llu.%03llu ", timer_msec / 1000u, timer_msec % 1000u);
    }

    return 0u;
}


