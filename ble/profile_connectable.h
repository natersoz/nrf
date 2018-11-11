/**
 * @file ble/profile_connectable.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "ble/gap_connection.h"
#include "ble/gatts_event_observer.h"
#include "ble/gatts_operations.h"
#include "ble/gattc_event_observer.h"
#include "ble/gatt_service.h"
#include "ble/gatt_service_container.h"
#include "ble/stack.h"
#include "ble/hci_error_codes.h"

namespace ble
{
namespace profile
{

/**
 * @class connectable
 * A base class for profile::peripheral and profile::central, aggregate the
 * specific classes which compose a BLE profile which connects with a peer.
 */
class connectable
{
public:
    virtual ~connectable()                      = default;

    connectable()                               = delete;
    connectable(connectable const&)             = delete;
    connectable(connectable &&)                 = delete;
    connectable& operator=(connectable const&)  = delete;
    connectable& operator=(connectable&&)       = delete;

    /// ctor: A connectable with both GATT server and client.
    connectable(ble::stack                      &ble_stack,
               ble::gap::connection             &ble_gap_connection,
               ble::gatts::event_observer       &ble_gatts_event_observer,
               ble::gatts::operations           &ble_gatts_operations,
               ble::gattc::event_observer       &ble_gattc_event_observer)
    : ble_stack_(ble_stack),
      gap_connection_(ble_gap_connection),
      gatts_event_observer_(&ble_gatts_event_observer),
      gatts_operations_(&ble_gatts_operations),
      gattc_event_observer_(&ble_gattc_event_observer)
    {
        this->gap_connection_.set_connecteable(this);
        this->gatts_event_observer_->set_connecteable(this);
        this->gatts_operations_->set_connecteable(this);
        this->gattc_event_observer_->set_connecteable(this);
    }

    /// ctor: A connectable with a GATT server only; no client.
    connectable(ble::stack                      &ble_stack,
               ble::gap::connection             &ble_gap_connection,
               ble::gatts::event_observer       &ble_gatts_event_observer,
               ble::gatts::operations           &ble_gatts_operations)
    : ble_stack_(ble_stack),
      gap_connection_(ble_gap_connection),
      gatts_event_observer_(&ble_gatts_event_observer),
      gatts_operations_(&ble_gatts_operations),
      gattc_event_observer_(nullptr)
    {
        this->gap_connection_.set_connecteable(this);
        this->gatts_event_observer_->set_connecteable(this);
        this->gatts_operations_->set_connecteable(this);
    }

    /// ctor: A connectable with a GATT client only; no server.
    connectable(ble::stack                      &ble_stack,
               ble::gap::connection             &ble_gap_connection,
               ble::gattc::event_observer       &ble_gattc_event_observer)
    : ble_stack_(ble_stack),
      gap_connection_(ble_gap_connection),
      gatts_event_observer_(nullptr),
      gatts_operations_(nullptr),
      gattc_event_observer_(&ble_gattc_event_observer)
    {
        this->gap_connection_.set_connecteable(this);
        this->gattc_event_observer_->set_connecteable(this);
    }

    ble::stack&       ble_stack()       { return this->ble_stack_; }
    ble::stack const& ble_stack() const { return this->ble_stack_; }

    ble::gap::connection const& connection() const { return this->gap_connection_; }
    ble::gap::connection&       connection()       { return this->gap_connection_; }

    ble::gatts::operations const* gatts() const { return this->gatts_operations_; }
    ble::gatts::operations*       gatts()       { return this->gatts_operations_; }

    ble::gatt::service_container const& service_container() const { return this->service_container_; }
    ble::gatt::service_container&       service_container()       { return this->service_container_; }

    void service_add(ble::gatt::service &service_to_add)
    {
        service_to_add.connectable_ = this;
        this->service_container_.push_back(service_to_add);

        if (this->gatts_operations_)
        {
            this->gatts_operations_->service_add(service_to_add);
        }
    }

private:
    ble::stack                      &ble_stack_;
    ble::gap::connection            &gap_connection_;
    ble::gatts::event_observer      *gatts_event_observer_;
    ble::gatts::operations          *gatts_operations_;
    ble::gattc::event_observer      *gattc_event_observer_;
    ble::gatt::service_container    service_container_;
};

} // namespace profile
} // namespace ble
