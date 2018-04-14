/**
 *
 * @details This module enables the application to create multiple timer instances based on the RTC1
 *          peripheral. Checking for time-outs and invocation of user time-out handlers is performed
 *          in the RTC1 interrupt handler. List handling is done using a software interrupt (SWI0).
 *          Both interrupt handlers are running in APP_LOW priority level.
 *
 * @details When calling app_timer_start() or app_timer_stop(), the timer operation is just queued,
 *          and the software interrupt is triggered. The actual timer start/stop operation is
 *          executed by the SWI0 interrupt handler. Since the SWI0 interrupt is running in APP_LOW,
 *          if the application code calling the timer function is running in APP_LOW or APP_HIGH,
 *          the timer operation will not be performed until the application handler has returned.
 *          This will be the case, for example, when stopping a timer from a time-out handler when not using
 *          the scheduler.
 *
 * @details Use the USE_SCHEDULER parameter of the APP_TIMER_INIT() macro to select if the
 *          @ref app_scheduler should be used or not. Even if the scheduler is
 *          not used, app_timer.h will include app_scheduler.h, so when
 *          compiling, app_scheduler.h must be available in one of the compiler include paths.
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "rtc.h"

#ifdef __cplusplus

#include "rtc_observer.h"

/**
 * Initialize the app_timer module by passing in an RTC observable instance.
 * The app_timer will use the rtc_observable to perform app_timer operations
 * with each app_timer implemented as an rtc_observer attached to this
 * rtc_observable.
 *
 * @note The app_timer can only be initialized from this C++ call.
 */
void app_timer_init(rtc_observable<>& rtc);

extern "C" {
#endif

/// Minimum value of the timeout_ticks parameter of app_timer_start().
#define APP_TIMER_MIN_TIMEOUT_TICKS     5

/// Application time-out handler type.
typedef void (* app_timer_timeout_handler_t)(void *context);

typedef enum
{
    APP_TIMER_MODE_SINGLE_SHOT,     /// The timer will expire only once.
    APP_TIMER_MODE_REPEATED         /// The timer will restart each time it expires.
} app_timer_mode_t;

/// @todo comments.
#define APP_TIMER_NODE_SIZE     ((size_t) 56u)
struct app_timer_t
{
    uint32_t data[APP_TIMER_NODE_SIZE / sizeof(uint32_t)];
};

typedef struct app_timer_t app_timer_t;

/**
 * Timer ID type.
 * Never declare a variable of this type, but use the macro @ref APP_TIMER_DEF instead.
 */
typedef struct app_timer_t* app_timer_id_t;

/**
 * @brief Create a timer identifier and statically allocate memory for the timer.
 *
 * @param timer_id Name of the timer identifier variable that will be used to control the timer.
 */
#define APP_TIMER_DEF(timer_id)                                      \
    static app_timer_t CONCAT_2(timer_id,_data) = { {0} };           \
    static const app_timer_id_t timer_id = &CONCAT_2(timer_id,_data)

/**
 * Convert the time interval in milliseconds to the same timer interval
 * in app_timer ticks.
 *
 * @param time_in_msec The number of milliseconds of interest.
 *
 * @return uint32_t The equivalent number of ticks.
 */
uint32_t APP_TIMER_TICKS(uint32_t time_in_msec);

/**
 * Create an app_timer instance. The app_timer_id_t must have been
 * instantiated with the APP_TIMER_DEF() macro.
 *
 * @param p_timer_id The app timer instance to use as the memory for
 *                   timer creation.
 * @param mode       @see app_timer_mode_t
 * @param timeout_handler The timer expiration completon handler.
 *
 * @return uint32_t Always NRF_SUCCESS (can be ignored).
 */
uint32_t app_timer_create(app_timer_id_t const        *p_timer_id,
                          app_timer_mode_t            mode,
                          app_timer_timeout_handler_t timeout_handler);

uint32_t app_timer_start(app_timer_id_t   timer_id,
                         uint32_t         timeout_ticks,
                         void             *context);

uint32_t app_timer_stop(app_timer_id_t timer_id);

uint32_t app_timer_stop_all(void);

/** @return uint32_t The difference between 2 application timer counts. */
static inline uint32_t app_timer_cnt_diff_compute(uint32_t   ticks_to,
                                                  uint32_t   ticks_from)
{
    return ticks_to - ticks_from;
}

/**
 * @return uint32_t The running RTC value read from the app_timer RTC1.
 * This is a 32-bit value that wraps around.
 */
uint32_t app_timer_cnt_get(void);

#ifdef __cplusplus
}
#endif

