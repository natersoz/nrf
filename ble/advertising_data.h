/**
 * @file ble/advertising_data.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * A set of classes for configuring and performing BLE advertising.
 */

#pragma once

#include "fixed_allocator.h"
#include <cstddef>
#include <cstring>
#include <vector>

namespace ble
{

class advertising_data_t: public std::vector<uint8_t, fixed_allocator<uint8_t> >
{
public:
    static std::size_t const length = 31u;

    ~advertising_data_t()                                       = default;

    advertising_data_t(advertising_data_t const&)               = delete;
    advertising_data_t(advertising_data_t &&)                   = delete;
    advertising_data_t& operator=(advertising_data_t const&)    = delete;
    advertising_data_t& operator=(advertising_data_t&&)         = delete;

    advertising_data_t()
        : std::vector<uint8_t, fixed_allocator<uint8_t> >(),
          allocator_()
    {
        memset(this->data_, 0, sizeof(this->data_));
        this->allocator_.set_backing_store(this->data_, sizeof(this->data_));
    }

private:
    uint8_t                     data_[length];
    fixed_allocator<uint8_t>    allocator_;
};

} // namespace ble

