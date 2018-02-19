/**
 * @file stream.h
 */

#pragma once

#include <cstddef>

class input_stream
{
public:
    virtual ~input_stream()                             = default;

    input_stream()                                      = default;
    input_stream(input_stream const& other)             = default;
    input_stream& operator=(input_stream const& other)  = default;

    virtual size_t read(void *buffer, size_t length)    = 0;
};

class output_stream
{
public:
    virtual ~output_stream()                                = default;

    output_stream()                                         = default;
    output_stream(output_stream const& other)               = default;
    output_stream& operator=(output_stream const& other)    = default;

    virtual size_t write(void const *buffer, size_t length) = 0;
    virtual void   flush()                                  = 0;
};

