/**
 * @file observer.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once
#include <boost/intrusive/list.hpp>

/// Forward declare the observable so that friendship can be established.
template <typename _notification_type>
class observable;

/**
 * @class observer.
 * observer implementation using boost::inclusive::list.
 *
 * This interface defines the requirements of an observer
 * that can be attached and detached to the observable class.
 * Attached Observers receive event notifications from the observable.
 *
 * @see Design Patterns Gamma, Helm, et al.
 * 'observer' pattern 'Subject' class. pp. 294 - 303.
 *
 * @tparam NotifyType is the data type that will be passed as a notification.
 * list_base_hook: default tag, raw pointers stored, safe_link mode.
 */
template <typename _notification_type>
class observer
{
public:
    using notification_type = _notification_type;

    virtual ~observer()                     = default;
    observer()                              = default;
    observer(observer const&)               = delete;
    observer(observer &&)                   = delete;
    observer& operator = (observer const&)  = delete;
    observer& operator=(observer&&)         = delete;

    /**
     * Notifications are called onto this interface
     * using typename notification_type.
     *
     * @param notification The notification data passed to the observer from
     * the observable.
     */
    virtual void notify(notification_type const &notification) = 0;

    /**
     * @return bool To determine whether this observer is attached to an observable.
     * @retval true The observer is attached to the observable.
     * @retval false The observer is not attached to the observable.
     */
    bool is_attached() const { return this->hook.is_linked(); }

private:
    using list_hook_type = boost::intrusive::list_member_hook<
        boost::intrusive::link_mode<boost::intrusive::auto_unlink>
        >;

    list_hook_type hook;

    friend class observable<notification_type>;
};
