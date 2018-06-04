/**
 * @file app_timer.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Public interface for a app_timer instance using the the rtc_observer
 * as a private implementation (pimpl).
 */

#include "app_timer.h"
#include "rtc_observer.h"
#include "logger.h"
#include "project_assert.h"
#include "nrf_error.h"
#include <functional>

static rtc_observable<> *app_timer_rtc_observable = nullptr;

using expiration_handler = std::function<void(void*)>;

class app_timer_rtc1_observer: public rtc_observer
{
public:
   virtual ~app_timer_rtc1_observer() override = default;

    app_timer_rtc1_observer(
        expiration_type      expiry_type     = expiration_type::one_shot,
        uint32_t             expiry_ticks    = UINT32_MAX,
        expiration_handler   expiry_handler  = nullptr)
    : rtc_observer(expiry_type, expiry_ticks),
      expiration_handler_(expiry_handler),
      context_(nullptr)
    {
    }

    void set_context(void *context)
    {
        this->context_ = context;
    }

    virtual void expiration_notify() override
    {
        if (this->expiration_handler_)
        {
            this->expiration_handler_(this->context_);
        }
    }

private:
    expiration_handler expiration_handler_;
    void               *context_;
};

static_assert(sizeof(struct app_timer_t) >= sizeof(app_timer_rtc1_observer));

void app_timer_init(rtc_observable<> &rtc)
{
    logger &logger = logger::instance();
    logger.debug("sizeof: app_timer_t: %u / %u",
                 sizeof(app_timer_rtc1_observer), sizeof(struct app_timer_t));

    // app_timer_rtc_observable must not already be set.
    ASSERT(not app_timer_rtc_observable);
    app_timer_rtc_observable = &rtc;
}

static rtc_observer::expiration_type expiration_mode_to_type(app_timer_mode_t mode)
{
    switch (mode)
    {
    case APP_TIMER_MODE_SINGLE_SHOT:
        return rtc_observer::expiration_type::one_shot;

    case APP_TIMER_MODE_REPEATED:
        return rtc_observer::expiration_type::continuous;

    default:
        ASSERT(0);
        return rtc_observer::expiration_type::one_shot;
        break;
    }
}

/**
 * Convert the time interval in milliseconds to the same timer interval
 * in app_timer ticks.
 *
 * @param time_in_msec The number of milliseconds of interest.
 *
 * @return uint32_t The equivalent number of ticks.
 */
uint32_t APP_TIMER_TICKS(uint32_t time_in_msec)
{
    ASSERT(app_timer_rtc_observable);
    return app_timer_rtc_observable->msec_to_ticks(time_in_msec);
}

uint32_t app_timer_create(app_timer_id_t const *      p_timer_id,
                          app_timer_mode_t            mode,
                          app_timer_timeout_handler_t timeout_handler)
{
    ASSERT(app_timer_rtc_observable);
    ASSERT(timeout_handler);

    ASSERT(p_timer_id);
    ASSERT(*p_timer_id);
    struct app_timer_t *app_timer = *p_timer_id;

    // rtc_observer *rtc_observer =
    new(app_timer->data) app_timer_rtc1_observer(
        expiration_mode_to_type(mode),
        UINT32_MAX,
        timeout_handler);

    return NRF_SUCCESS;
}

uint32_t app_timer_start(app_timer_id_t   timer_id,
                         uint32_t         expiration_ticks,
                         void             *context)
{
    ASSERT(app_timer_rtc_observable);
    ASSERT(timer_id);
    app_timer_rtc1_observer *observer = reinterpret_cast<app_timer_rtc1_observer*>(timer_id->data);
    observer->expiration_set(expiration_ticks);
    observer->set_context(context);

    if (not observer->is_attached())
    {
        app_timer_rtc_observable->attach(*observer);
    }

    return NRF_SUCCESS;
}

uint32_t app_timer_stop(app_timer_id_t timer_id)
{
    ASSERT(app_timer_rtc_observable);
    ASSERT(timer_id);
    app_timer_rtc1_observer *observer = reinterpret_cast<app_timer_rtc1_observer*>(timer_id->data);

    if (observer->is_attached())
    {
        app_timer_rtc_observable->detach(*observer);
    }

    return NRF_SUCCESS;
}

uint32_t app_timer_stop_all(void)
{
    ASSERT(app_timer_rtc_observable);
    app_timer_rtc_observable->stop();
    return NRF_SUCCESS;
}

uint32_t app_timer_cnt_get(void)
{
    ASSERT(app_timer_rtc_observable);
    return app_timer_rtc_observable->cc_get_count();
}

void app_timer_pause(void)
{
    ASSERT(app_timer_rtc_observable);
    app_timer_rtc_observable->stop();
}

void app_timer_resume(void)
{
    ASSERT(app_timer_rtc_observable);
    app_timer_rtc_observable->start();
}

