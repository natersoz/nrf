/**
 * @file test_observer.cc
 * Test application for classes Observer and Observable.
 */

#include <cstddef>
#include <cassert>
#include <iostream>
#include "observer.h"
#include "observable.h"

static constexpr bool const debug_print = false;

static observable<int> test_observable;

/**
 * @class test_observer
 * A simple observer which gets notifications of integers.
 */
class test_observer: public observer<int>
{
public:
    using observer<int>::observer;

    virtual ~test_observer() {};
    test_observer() = delete;

    test_observer(int id): id_(id) {};

    virtual void notify(int const &notification) override;

    int get_id() const { return this->id_; }

private:
        int id_;
};

std::ostream& operator << (std::ostream &os, test_observer const &observer)
{
     std::cout << "test_observer[" << observer.get_id() << "]";
     return os;
};

void test_observer::notify(int const &notification)
{
    if (debug_print)
    {
        std::cout << *this << " received: " << notification << std::endl;
    }

    // When the value of the notification matches our id, detach from the observable.
    // This will test that the remove operation from the observer_list works properly.
    if (notification == this->id_)
    {
        test_observable.detach(*this);
        if (debug_print)
        {
            std::cout << "observer " << this->id_ << " is attached: " << this->is_attached() << std::endl;
        }
    }
};

static test_observer test_observer_1(1);
static test_observer test_observer_2(2);
static test_observer test_observer_3(3);
static test_observer test_observer_4(4);

int main(void)
{
    bool result = true;

    assert(test_observable.get_observer_count() == 0u);
    assert(test_observer_1.is_attached() == false);
    assert(test_observer_2.is_attached() == false);
    assert(test_observer_3.is_attached() == false);
    assert(test_observer_4.is_attached() == false);

    test_observable.attach(test_observer_1);
    assert(test_observable.get_observer_count() == 1u);
    assert(test_observer_1.is_attached() == true);

    test_observable.attach(test_observer_2);
    assert(test_observable.get_observer_count() == 2u);
    assert(test_observer_2.is_attached() == true);

    test_observable.attach(test_observer_3);
    assert(test_observable.get_observer_count() == 3u);
    assert(test_observer_3.is_attached() == true);

    test_observable.attach(test_observer_4);
    assert(test_observable.get_observer_count() == 4u);
    assert(test_observer_4.is_attached() == true);

    test_observable.notify_all(1);
    assert(test_observable.get_observer_count() == 3u);
    assert(test_observer_1.is_attached() == false);

    test_observable.notify_all(1);
    assert(test_observable.get_observer_count() == 3u);

    test_observable.notify_all(2);
    assert(test_observable.get_observer_count() == 2u);
    assert(test_observer_2.is_attached() == false);

    test_observable.notify_all(2);
    assert(test_observable.get_observer_count() == 2u);

    test_observable.notify_all(3);
    assert(test_observable.get_observer_count() == 1u);
    assert(test_observer_3.is_attached() == false);

    test_observable.notify_all(4);
    assert(test_observable.get_observer_count() == 0u);
    assert(test_observer_4.is_attached() == false);

    std::cerr << (result ? "-- PASS --" : "-- FAIL --") << std::endl;
    return result ? 0 : -1;
}
