/**
 * @file null_stream.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "stream.h"
#include <limits>
#include <cstddef>

namespace io
{

/**
 * @class stdout_stream
 * Implement io::stream for a null stream (prints nothing).
 */
class nullout_stream: public io::output_stream
{
private:
    using super = io::output_stream;
public:
    using super::super;

    virtual std::size_t write(void const *buffer, size_t length) override
    {
        return length;
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
    }
};

} // namespace io
