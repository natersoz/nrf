/**
 * @file logger.cc
 */

#include "logger.h"
#include "logger_c.h"
#include "vwritef.h"
#include "write_data.h"

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
        va_list args;
        va_start(args, fmt);

        n_written = vwritef(*this->os_, fmt, args);

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

size_t logger_write(enum logger_level log_level, char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    size_t n_write = logger::instance().vwrite(static_cast<logger::level>(log_level), fmt, args);

    va_end(args);
    return n_write;
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
                                         static_cast<write_data::data_prefix>(prefix));
}

