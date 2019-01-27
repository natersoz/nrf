/**
 * @file std_stream.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "stream.h"
#include <limits>
#include <cstddef>
#include <cstdio>

namespace io
{

/**
 * @class stdout_stream
 * Implement io::stream for stdout
 */
class stdout_stream: public io::output_stream
{
public:
    virtual ~stdout_stream() override           = default;
    stdout_stream()                             = default;
    stdout_stream(stdout_stream const&)         = delete;
    stdout_stream(stdout_stream&&)              = default;
    stdout_stream& operator=(stdout_stream&)    = delete;
    stdout_stream& operator=(stdout_stream&&)   = default;

    virtual std::size_t write(void const *buffer, size_t length) override
    {
        return fwrite(buffer, sizeof(char), length, stdout);
    }

    virtual std::size_t write_pending() const override
    {
        return 0u;
    }

    virtual std::size_t write_avail() const override
    {
        return std::numeric_limits<std::size_t>::max();
    }

    virtual void flush() override
    {
        fflush(stdout);
    }
};

} // namespace io
