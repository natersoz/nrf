/**
 * @file test_write_data.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "write_data.h"
#include "std_stream.h"

int main(int argc, char **argv)
{
    int result = 0;

    io::stdout_stream os;

    char test_data[251];
    for (size_t iter = 0u; iter < sizeof(test_data); ++iter)
    {
        test_data[iter] = static_cast<char>(iter);
    }

    io::write_data(os,
                   test_data,
                   sizeof(test_data),
                   true,
                   io::data_prefix::index);

    return result;
}
