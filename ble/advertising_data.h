/**
 * @file ble/advertising_data.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * A set of classes for configuring and performing BLE advertising.
 */

#pragma once

#include <cstdint>
#include <cstddef>
#include <cstring>

namespace ble
{

class advertising_data_t
{
public:
    static std::size_t const length = 31u;

    ~advertising_data_t()                                       = default;

    advertising_data_t(advertising_data_t const&)               = delete;
    advertising_data_t(advertising_data_t &&)                   = delete;
    advertising_data_t& operator=(advertising_data_t const&)    = delete;
    advertising_data_t& operator=(advertising_data_t&&)         = delete;

    advertising_data_t(): data_index_(0u)
    {
        memset(this->data_, 0, sizeof(this->data_));
    }

    std::size_t size()     const { return this->data_index_; }
    std::size_t capacity() const { return sizeof(this->data_); }

    uint8_t*       begin()       { return &this->data_[0]; }
    uint8_t const* begin() const { return &this->data_[0]; }

    uint8_t*       end()         { return &this->data_[this->data_index_]; }
    uint8_t const* end()   const { return &this->data_[this->data_index_]; }

    void push_back(uint8_t value) { this->data_[this->data_index_++] = value; }

private:
    uint8_t                     data_index_;
    uint8_t                     data_[length];
};

} // namespace ble

