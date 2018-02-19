/**
 * @file write_data.h
 */

#pragma once

#include "stream.h"
#include <cstddef>
#include <cstdint>

namespace write_data
{

/// The type of prefix to write before each row of data written.
enum class data_prefix
{
    none = 0,           ///< No prefix written.
    index,              ///< An index into the data, starting with zero.
    address             ///< The data address.
};

size_t write_data(output_stream &os,
                  void const *data,
                  size_t length,
                  bool char_data = false,
                  data_prefix prefix = data_prefix::index);

size_t write_data_16(output_stream &os,
                     uint16_t const *data,
                     size_t length,
                     bool char_data = false,
                     data_prefix prefix = data_prefix::index);

size_t write_data_32(output_stream &os,
                     uint32_t const *data,
                     size_t length,
                     bool char_data = false,
                     data_prefix prefix = data_prefix::index);

};
