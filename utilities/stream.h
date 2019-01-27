/**
 * @file stream.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * A character driver-like set of classes.
 */

#pragma once

#include <cstddef>

namespace io
{

class input_stream
{
public:
    virtual ~input_stream()                       = default;

    input_stream()                                = default;
    input_stream(input_stream const&)             = delete;
    input_stream(input_stream&&)                  = default;
    input_stream& operator=(input_stream const&)  = delete;
    input_stream& operator=(input_stream&&)       = default;

    virtual std::size_t read(void *buffer, std::size_t length)      = 0;
    virtual std::size_t read_pending() const                        = 0;
    virtual std::size_t read_avail()   const                        = 0;
};

class output_stream
{
public:
    virtual ~output_stream()                        = default;

    output_stream()                                 = default;
    output_stream(output_stream const&)             = delete;
    output_stream(output_stream&&)                  = default;
    output_stream& operator=(output_stream const&)  = delete;
    output_stream& operator=(output_stream&&)       = default;

    virtual std::size_t write(void const *buffer, std::size_t length)   = 0;
    virtual std::size_t write_pending() const                           = 0;
    virtual std::size_t write_avail()   const                           = 0;
    virtual void        flush()                                         = 0;
};

}  // namespace io
