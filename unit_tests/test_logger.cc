/**
 * @file test_logger.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "logger.h"
#include <cstdio>

static constexpr char const error_string[]  = "error";
static constexpr char const warn_string[]   = "warning";
static constexpr char const info_string[]   = "info";
static constexpr char const debug_string[]  = "debug";

static constexpr char const color_reset_string[]    = { 0x1B, '[', '3', '9', ';', '4', '9', 'm', 0 };
static constexpr char const color_red_string[]      = { 0x1B, '[', '9', '1', 'm', 0 };
static constexpr char const color_yellow_string[]   = { 0x1B, '[', '9', '3', 'm', 0 };

static logger logger_instance;

logger& logger::instance()
{
    return logger_instance;
}

size_t logger::error(char const *fmt, ...)
{
    size_t n_written = 0u;
    if (this->os_ != nullptr)
    {
        std::fprintf(stderr, "%s%s", color_red_string, error_string);

        va_list args;
        va_start(args, fmt);
        n_written += std::vfprintf(stderr, fmt, args);
        va_end(args);

        std::fprintf(stderr, "%s\n", color_reset_string);
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
            std::fprintf(stderr, "%s%s", color_yellow_string, warn_string);

            va_list args;
            va_start(args, fmt);
            n_written += std::vfprintf(stderr, fmt, args);
            va_end(args);

            std::fprintf(stderr, "%s\n", color_reset_string);
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
            std::fprintf(stderr, "%s", info_string);

            va_list args;
            va_start(args, fmt);
            n_written += std::vfprintf(stderr, fmt, args);
            va_end(args);

            std::fprintf(stderr, "\n");
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
            std::fprintf(stderr, "%s", debug_string);

            va_list args;
            va_start(args, fmt);
            n_written += std::vfprintf(stderr, fmt, args);
            va_end(args);

            std::fprintf(stderr, "\n");
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
            va_list args;
            va_start(args, fmt);
            n_written += std::vfprintf(stderr, fmt, args);
            va_end(args);

            std::fprintf(stderr, "\n");
        }
    }

    return n_written;
}

size_t logger::write(char const *fmt, ...)
{
    size_t n_written = 0u;
    if (this->os_ != nullptr)
    {
//        n_written += this->log_time();

        va_list args;
        va_start(args, fmt);
        n_written += std::vfprintf(stderr, fmt, args);
        va_end(args);

        std::fprintf(stderr, "\n");
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
//            n_written += this->log_time();

            n_written += std::vfprintf(stderr, fmt, args);
            std::fprintf(stderr, "\n");
        }
    }

    return n_written;
}

void logger::flush()
{
    std::fflush(stderr);
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
            n_written = 0u; //write_data::write_data(*this->os_, data, length, char_data, prefix);
        }
    }

    return n_written;
}


