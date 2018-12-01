/**
 * @file ble/stack.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include <system_error>
#include <array>

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

    struct version
    {
        /**
         * The Bluetooth core specification version.
         * @see https://www.bluetooth.com/specifications/assigned-numbers/link-layer
         */
        uint8_t  link_layer_version;

        /**
         * The Bluetooth company identifier.
         * @see https://www.bluetooth.com/specifications/assigned-numbers/company-identifiers
         */
        uint16_t company_id;

        /** Vendor specific information */
        std::array<uint32_t, 2u> vendor_specific;
    };

    virtual ~stack()                = default;

    stack()                         = default;
    stack(stack const&)             = delete;
    stack(stack &&)                 = delete;
    stack& operator=(stack const&)  = delete;
    stack& operator=(stack&&)       = delete;

    /**
     * The details for the silicon vendor stack is initialized with its
     * operating parameters.
     *
     * @param peripheral_count The maximum number of peripheral connections the
     *                         stack will need to maintain.
     * @param central_count    The maximum number of central connections the
     *                         stack will need to maintain.
     *
     * @return std::errc       Whether the operation succeeded or not.
     */
    virtual std::errc init(unsigned int peripheral_count,
                           unsigned int central_count) = 0;

    /**
     * Set the maximum ATT MTU size, in octets.
     *
     * @param att::length The maximum number of octets within on ATT MTU.
     * The minimum length is  23 octets: ble::att::mtu_length_minimum.
     * The maximum length is 251 octets: ble::att::mtu_length_maximum
     * @return std::errc The standard error code.
     */
    virtual std::errc set_mtu_max_size(ble::att::length_t mtu_max_size) = 0;

    virtual std::errc enable() = 0;
    virtual std::errc disable() = 0;
    virtual bool      is_enabled() const = 0;

    constraints const& get_constraints() const {
        return this->constraints_;
    }

    virtual version get_version() const = 0;

protected:
    constraints constraints_;
};

} //namespace ble
