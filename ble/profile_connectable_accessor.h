/**
 * @file ble/prpfile_connectable_accessor.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Generic GATTS BLE event observer interface.
 */

#pragma once

#include <cstdint>
#include "ble/att.h"

namespace ble
{
namespace profile
{

class connectable;

/**
 * @class connectable_accessor
 * This utility class is used by
 * - ble::gap::connection
 * - ble::gatts:event_observer
 * - ble::gattc::observer
 * To carry a link to their owning connectable classes.
 */
class connectable_accessor
{
public:
    virtual ~connectable_accessor()                              = default;

    connectable_accessor(connectable_accessor const&)            = delete;
    connectable_accessor(connectable_accessor &&)                = delete;
    connectable_accessor& operator=(connectable_accessor const&) = delete;
    connectable_accessor& operator=(connectable_accessor&&)      = delete;

    connectable_accessor(): connectable_(nullptr) {}

    ble::profile::connectable const *get_connecteable() const {
        return this->connectable_;
    }

    ble::profile::connectable *get_connecteable() {
        return this->connectable_;
    }

    void set_connecteable(ble::profile::connectable *connectable) {
        this->connectable_ = connectable;
    }

private:
    ble::profile::connectable *connectable_;
};

} // namespace profile
} // namespace ble
