/**
 * @file ble/stack.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include <system_error>
#include "ble/att.h"

namespace ble
{

struct stack
{
    struct constraints
    {
        constraints()
            : att_mtu_maximum_length(ble::att::mtu_length_maximum)
        {
        }

        uint8_t att_mtu_maximum_length;
    };

    virtual ~stack()                = default;

    stack()                         = default;
    stack(stack const&)             = delete;
    stack(stack &&)                 = delete;
    stack& operator=(stack const&)  = delete;
    stack& operator=(stack&&)       = delete;

    virtual std::errc init() = 0;
    virtual std::errc enable() = 0;
    virtual std::errc disable() = 0;
    virtual bool      is_enabled() const = 0;

    constraints const& get_constraints() const {
        return this->constraints_;
    }

protected:
    constraints constraints_;
};

} //namespace ble
