/**
 * @file test_write_data.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "write_data.h"
#include "stream.h"

#include <iostream>
#include <iomanip>
#include <cassert>

// static constexpr bool debug_print = false;

class stdout_stream: public output_stream
{
public:
    virtual ~stdout_stream() override = default;

    stdout_stream() = default;
    stdout_stream(stdout_stream const& other) = delete;
    stdout_stream& operator=(stdout_stream const& other) = delete;

    virtual size_t write(void const *buffer, size_t length) override
    {
        return fwrite(buffer, sizeof(char), length, stdout);
    }

    virtual void flush() override
    {
        fflush(stdout);
    }
};

int main(int argc, char **argv)
{
    int result = 0;

    stdout_stream os;

    char test_data[251];
    for (size_t iter = 0x78u; iter < sizeof(test_data); ++iter)
    {
        test_data[iter] = static_cast<char>(iter);
    }

    write_data::write_data(os, test_data, sizeof(test_data), true,
                           write_data::data_prefix::index);

    return result;
}
