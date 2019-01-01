/**
 * @file observable.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include <cstddef>
#include "observer.h"

/**
 * @class observable
 * Use within a class as an aggregate implementation of the observable 'Subject'
 * @see Design Patterns Gamma, Helm, et al.
 * 'Observer' pattern 'Subject' class. p. 294.
 *
 * @tparam _notification_type The type of data passed to observers as the
 * notification within the notify() function.
 *
 * @note Attaching, Detaching, Notifying and storing observers is done with
 * non-const references since calling observer::notify() is expected
 * to have side effects (non-const).
 */
template <typename _notification_type>
class observable
{
public:
    using notification_type = _notification_type;

    ~observable()                               = default;
    observable()                                = default;
    observable(observable const&)               = delete;
    observable(observable&&)                    = delete;
    observable& operator = (observable const&)  = delete;
    observable& operator=(observable&&)         = delete;

    /**
     * Attach an observer to this observable.
     * The observer will be notified with type notification_type const reference
     * when needed.
     *
     * @param observer The observer to notify with events.
     */
    void attach(observer<notification_type> &observer);

    /**
     * Detach the observer from this observable.
     * The observer will no longer be notified.
     *
     * @param observer The observer to withdraw from receiving event notifications.
     */
    void detach(observer<notification_type> &observer);

    /**
     * Notify all observers of an event.
     * @note This method cannot be const since the call to the observer may
     * result in a Detach() operation.
     *
     * @param notification The information to be sent as a notification event.
     */
    void notify_all(notification_type const &notification);

    /** @return size_t The number of Observers attached to this observable. */
    std::size_t get_observer_count(void) const;

private:
        using list_type =
        boost::intrusive::list<
            observer<notification_type>,
            boost::intrusive::constant_time_size<false>,
            boost::intrusive::member_hook<
                observer<notification_type>,
                typename observer<notification_type>::list_hook_type,
                &observer<notification_type>::hook>
        >;

        list_type observer_list;
};

template <typename notification_type>
void observable<notification_type>::attach(observer<notification_type> &observer)
{
    observer_list.push_back(observer);
}

template <typename notification_type>
void observable<notification_type>::detach(observer<notification_type> &observer)
{
    if (observer.hook.is_linked())
    {
        observer.hook.unlink();
    }
}

template <typename notification_type>
void observable<notification_type>::notify_all(notification_type const &notification)
{
    // Note that the iteration happens prior to calling observer->notify().
    // The observer may remove itself from the list rendering the current
    // iterator position invalid. Therefore: iterate first, operate next.
    for (auto observer_iter = observer_list.begin();
         observer_iter != observer_list.end();
        )
    {
        observer<notification_type> &observer = *observer_iter;
        ++observer_iter;
        observer.notify(notification);
    }
}

template <typename notification_type>
std::size_t observable<notification_type>::get_observer_count() const
{
    return observer_list.size();
}
