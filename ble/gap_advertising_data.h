/**
 * @file ble/gap_advertising_data.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * A set of classes for configuring and performing BLE advertising.
 */

#pragma once

#include "ble/att.h"

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <array>

namespace ble
{
namespace gap
{

class advertising_data_t
{
public:
    static constexpr std::size_t const max_length = 31u;

    using container = std::array<uint8_t, max_length>;

    ~advertising_data_t()                                       = default;

    advertising_data_t(advertising_data_t const&)               = delete;
    advertising_data_t(advertising_data_t &&)                   = delete;
    advertising_data_t& operator=(advertising_data_t const&)    = delete;
    advertising_data_t& operator=(advertising_data_t&&)         = delete;

    advertising_data_t(): data_(), index_(data_.begin())
    {
        this->data_.fill(0);
    }

    void push_back(uint8_t value) { *this->index_++ = value; }

    std::size_t size()     const { return this->index_ - this->data_.begin(); }
    std::size_t capacity() const { return this->data_.max_size(); }

    container::const_iterator end() const { return this->index_; }
    container::iterator       end()       { return this->index_; }

    uint8_t const* data() const { return this->data_.data(); }
    uint8_t*       data()       { return this->data_.data(); }

private:
    container data_;
    container::iterator index_;
};

} // namespace gap
} // namespace ble

