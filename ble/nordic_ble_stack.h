/**
 * @file ble/nordic_ble_stack.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "ble/stack.h"
#include "rtc.h"

namespace ble
{
namespace nordic
{

class stack: public ble::stack
{
public:
    virtual ~stack() override       = default;

    stack()                         = delete;
    stack(stack const&)             = delete;
    stack(stack &&)                 = delete;
    stack& operator=(stack const&)  = delete;
    stack& operator=(stack&&)       = delete;

    stack(rtc &rtc):
        ble::stack(),
        rtc_(rtc)
    {
    }

    std::errc init() override;
    std::errc enable() override;
    std::errc disable() override;
    bool      is_enabled() const override;

private:
    rtc &rtc_;

    uint32_t connection_parameters_init();
};

} // namespace nordic
} // namespace ble




