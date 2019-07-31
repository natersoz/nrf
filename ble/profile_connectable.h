/**
 * @file ble/profile_connectable.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "ble/stack.h"
#include "ble/gap_connection.h"
#include "ble/gatts_event_observer.h"
#include "ble/gatts_operations.h"
#include "ble/gattc_event_observer.h"
#include "ble/gattc_operations.h"
#include "ble/gattc_service_builder.h"
#include "ble/gatt_service.h"
#include "ble/gatt_service_container.h"
#include "ble/hci_error_codes.h"

#include <boost/intrusive/list.hpp>

namespace ble
{
namespace profile
{

/**
 * @struct connectable
 * A base class for profile::peripheral and profile::central, aggregate the
 * specific classes which compose a BLE profile which connects with a peer.
 */
struct connectable
{
public:
    /**
     * @struct ble::profile::connectable::gatts
     * Aggregates ble::gatts::event_observer
     *            ble::gatts::operations.
     */
    struct gatts
    {
        ~gatts()                        = default;

        gatts(gatts const&)             = delete;
        gatts(gatts&&)                  = delete;
        gatts& operator=(gatts const&)  = delete;
        gatts& operator=(gatts&&)       = delete;

        gatts(ble::gatts::event_observer&   evt_observer,
              ble::gatts::operations&       ops)
            : event_observer(&evt_observer), operations(&ops) {}

        gatts() : event_observer(nullptr), operations(nullptr) {}

        ble::gatts::event_observer* const   event_observer;
        ble::gatts::operations* const       operations;
    };

    /**
     * @struct ble::profile::connectable::gattc
     * Aggregates ble::gatts::event_observer,
     *            ble::gatts::operations,
     *            ble::gattc::service_builder.
     */
    struct gattc
    {
        ~gattc()                        = default;

        gattc(gattc const&)             = delete;
        gattc(gattc&&)                  = delete;
        gattc& operator=(gattc const&)  = delete;
        gattc& operator=(gattc&&)       = delete;

        gattc()
            : event_observer(nullptr),
              operations(nullptr),
              service_builder(nullptr) {}

        gattc(ble::gattc::event_observer&  evt_observer,
              ble::gattc::operations&      ops,
              ble::gattc::service_builder& svc_builder)
            : event_observer(&evt_observer),
              operations(&ops),
              service_builder(&svc_builder) {}

        ble::gattc::event_observer* const   event_observer;
        ble::gattc::operations* const       operations;
        ble::gattc::service_builder* const  service_builder;
    };

    virtual ~connectable()                      = default;

    connectable()                               = delete;
    connectable(connectable const&)             = delete;
    connectable(connectable&&)                  = delete;
    connectable& operator=(connectable const&)  = delete;
    connectable& operator=(connectable&&)       = delete;

    /// ctor: A connectable with both GATT server and client.
    connectable(ble::stack&                     ble_stack,
                ble::gap::connection&           gap_connection,
                ble::gatts::event_observer&     gatts_event_observer,
                ble::gatts::operations&         gatts_operations,
                ble::gattc::event_observer&     gattc_event_observer,
                ble::gattc::operations&         gattc_operations,
                ble::gattc::service_builder&    gattc_service_builder)
    : stack(ble_stack),
      gap(gap_connection),
      gatts(gatts_event_observer, gatts_operations),
      gattc(gattc_event_observer, gattc_operations, gattc_service_builder),
      service_builder_completion(nullptr),
      service_container(),
      priv_hook_()
    {
        this->gap.set_connecteable(this);
        this->gatts.event_observer->set_connecteable(this);
        this->gatts.operations->set_connecteable(this);
        this->gattc.event_observer->set_connecteable(this);
    }

    /// ctor: A connectable with a GATT server only; no client.
    connectable(ble::stack&                     ble_stack,
                ble::gap::connection&           gap_connection,
                ble::gatts::event_observer&     gatts_event_observer,
                ble::gatts::operations&         gatts_operations)
    : stack(ble_stack),
      gap(gap_connection),
      gatts(gatts_event_observer, gatts_operations),
      service_builder_completion(nullptr),
      service_container(),
      priv_hook_()
    {
        this->gap.set_connecteable(this);
        this->gatts.event_observer->set_connecteable(this);
        this->gatts.operations->set_connecteable(this);
    }

    /// ctor: A connectable with a GATT client only; no server.
    connectable(ble::stack&                     ble_stack,
                ble::gap::connection&           gap_connection,
                ble::gattc::event_observer&     gattc_event_observer,
                ble::gattc::operations&         gattc_operations,
                ble::gattc::service_builder&    gattc_service_builder)
    : stack(ble_stack),
      gap(gap_connection),
      gattc(gattc_event_observer, gattc_operations, gattc_service_builder),
      service_builder_completion(nullptr),
      service_container(),
      priv_hook_()
    {
        this->gap.set_connecteable(this);
        this->gattc.event_observer->set_connecteable(this);
    }

    ble::stack&                                     stack;
    ble::gap::connection&                           gap;
    struct gatts                                    gatts;
    struct gattc                                    gattc;
    ble::gattc::service_builder::completion_notify* service_builder_completion;
    ble::gatt::service_container                    service_container;

    void service_add(ble::gatt::service &service_to_add)
    {
        service_to_add.connectable_ = this;
        this->service_container.push_back(service_to_add);

        if (this->gatts.operations)
        {
            this->gatts.operations->service_add(service_to_add);
        }
    }

    bool is_linked() const { return this->priv_hook_.is_linked(); }
    void unlink() { return this->priv_hook_.unlink(); }

private:
    using list_hook_type = boost::intrusive::list_member_hook<
        boost::intrusive::link_mode<boost::intrusive::auto_unlink>
        >;

    list_hook_type priv_hook_;

    using list_type = boost::intrusive::list<
        ble::profile::connectable,
        boost::intrusive::constant_time_size<false>,
        boost::intrusive::member_hook<ble::profile::connectable,
                                      ble::profile::connectable::list_hook_type,
                                      &ble::profile::connectable::priv_hook_>
        >;

//    friend class container;

public:
    class container: public ble::profile::connectable::list_type
    {
    public:
        ~container()                            = default;

        container()                             = default;
        container(container const&)             = delete;
        container(container &&)                 = delete;
        container& operator=(container const&)  = delete;
        container& operator=(container&&)       = delete;

    };
};

} // namespace profile
} // namespace ble
