/**
 * @file A timer_counter class based on the Nordic timer/counter peripheral.
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "timer.h"
#include "project_assert.h"
#include "nrf_cmsis.h"

#include <iterator>

struct timer_control_block_t
{
    NRF_TIMER_Type* const   registers;
    IRQn_Type       const   irq_type;
    timer_cc_index_t        cc_alloc_count;
    timer_event_handler_t   handler;
    void*                   context;
};

static void irq_handler_timer(timer_control_block_t *timer_control);

#if defined TIMER0_ENABLED
static struct timer_control_block_t timer_instance_0 =
{
    .registers              = NRF_TIMER0,
    .irq_type               = TIMER0_IRQn,
    .cc_alloc_count         = 4u,
    .handler                = nullptr,
    .context                = nullptr,
};
static struct timer_control_block_t* const timer_instance_ptr_0 = &timer_instance_0;

extern "C" void TIMER0_IRQHandler(void)
{
    irq_handler_timer(&timer_instance_0);
}
#else
static struct timer_control_block_t* const timer_instance_ptr_0 = nullptr;
#endif  // TIMER0_ENABLED

#if defined TIMER1_ENABLED
static struct timer_control_block_t timer_instance_1 =
{
    .registers              = NRF_TIMER1,
    .irq_type               = TIMER1_IRQn,
    .cc_alloc_count         = 4u,
    .handler                = nullptr,
    .context                = nullptr,
};
static struct timer_control_block_t* const timer_instance_ptr_1 = &timer_instance_1;

extern "C" void TIMER1_IRQHandler(void)
{
    irq_handler_timer(&timer_instance_1);
}
#else
static struct timer_control_block_t* const timer_instance_ptr_1 = nullptr;
#endif  // TIMER1_ENABLED

#if defined TIMER2_ENABLED
static struct timer_control_block_t timer_instance_2 =
{
    .registers              = NRF_TIMER2,
    .irq_type               = TIMER2_IRQn,
    .cc_alloc_count         = 4u,
    .handler                = nullptr,
    .context                = nullptr,
};
static struct timer_control_block_t* const timer_instance_ptr_2 = &timer_instance_2;

extern "C" void TIMER2_IRQHandler(void)
{
    irq_handler_timer(&timer_instance_2);
}
#else
static struct timer_control_block_t* const timer_instance_ptr_2 = nullptr;
#endif  // TIMER2_ENABLED

#if defined TIMER3_ENABLED
static struct timer_control_block_t timer_instance_3 =
{
    .registers              = NRF_TIMER3,
    .irq_type               = TIMER3_IRQn,
    .cc_alloc_count         = 6u,
    .handler                = nullptr,
    .context                = nullptr,
};
static struct timer_control_block_t* const timer_instance_ptr_3 = &timer_instance_3;

extern "C" void TIMER3_IRQHandler(void)
{
    irq_handler_timer(&timer_instance_3);
}
#else
static struct timer_control_block_t* const timer_instance_ptr_3 = nullptr;
#endif  // TIMER3_ENABLED

#if defined TIMER4_ENABLED
static struct timer_control_block_t timer_instance_4 =
{
    .registers              = NRF_TIMER4,
    .irq_type               = TIMER4_IRQn,
    .cc_alloc_count         = 6u,
    .handler                = nullptr,
    .context                = nullptr,
};
static struct timer_control_block_t* const timer_instance_ptr_4 = &timer_instance_4;

extern "C" void TIMER4_IRQHandler(void)
{
    irq_handler_timer(&timer_instance_4);
}
#else
static struct timer_control_block_t* const timer_instance_ptr_4 = nullptr;
#endif  // TIMER4_ENABLED

struct timer_control_block_t* const timer_instances[] =
{
    timer_instance_ptr_0,
    timer_instance_ptr_1,
    timer_instance_ptr_2,
    timer_instance_ptr_3,
    timer_instance_ptr_4,
};

/// The High frequnecy clock which drives the timer prescaler.
static uint32_t const hfclk_frequency_Hz = 16000000;

/// The maximum prescaler exponent allowed.
/// The prescaler divides the 16 MHz HFCLK / 2 ^ prescaler.
static uint8_t const prescaler_exp_max = 9u;

static struct timer_control_block_t* const timer_control_block(timer_instance_t timer_instance)
{
    if (timer_instance < std::size(timer_instances))
    {
        return timer_instances[timer_instance];
    }
    else
    {
        return nullptr;
    }
}

static void timer_clear_compare_event(struct timer_control_block_t* timer_control,
                                      timer_cc_index_t              cc_index)
{
    timer_control->registers->EVENTS_COMPARE[cc_index] = 0u;
    volatile uint32_t dummy = timer_control->registers->EVENTS_COMPARE[cc_index];
    (void) dummy;
}

void timer_init(timer_instance_t        timer_instance,
                enum timer_mode_t       timer_mode,
                uint8_t                 prescaler_exp,
                uint8_t                 irq_priority,
                timer_event_handler_t   handler,
                void                    *context)
{
    struct timer_control_block_t* const timer_control = timer_control_block(timer_instance);
    ASSERT(timer_control);
    ASSERT(prescaler_exp <= prescaler_exp_max);
    ASSERT(timer_control->registers->TASKS_START == 0u);
    ASSERT(handler);

    if (timer_mode == timer_mode_timer)
    {
        timer_control->registers->MODE = TIMER_MODE_MODE_Timer << TIMER_MODE_MODE_Pos;
    }
    else if (timer_mode == timer_mode_counter)
    {
        timer_control->registers->MODE = TIMER_MODE_MODE_LowPowerCounter << TIMER_MODE_MODE_Pos;
    }
    else
    {
        ASSERT(0);
    }

    timer_control->handler                  = handler;
    timer_control->context                  = context;

    timer_control->registers->BITMODE       = TIMER_BITMODE_BITMODE_32Bit << TIMER_BITMODE_BITMODE_Pos;
    timer_control->registers->PRESCALER     = prescaler_exp << TIMER_PRESCALER_PRESCALER_Pos;

    timer_control->registers->TASKS_STOP    = 1u;       // Make certain the timer is stopped.
    timer_control->registers->TASKS_CLEAR   = 1u;
    timer_control->registers->SHORTS        = 0u;
    timer_control->registers->INTENCLR      = UINT32_MAX;

    for (timer_cc_index_t cc_index = 0u; cc_index < timer_control->cc_alloc_count; ++cc_index)
    {
        timer_clear_compare_event(timer_control, cc_index);
        timer_control->registers->CC[cc_index] = 0u;
    }

    NVIC_SetPriority(timer_control->irq_type, irq_priority);
    NVIC_ClearPendingIRQ(timer_control->irq_type);
    NVIC_EnableIRQ(timer_control->irq_type);
}

void timer_deinit(timer_instance_t timer_instance)
{
    struct timer_control_block_t* const timer_control = timer_control_block(timer_instance);
    ASSERT(timer_control);

    NVIC_ClearPendingIRQ(timer_control->irq_type);
    NVIC_DisableIRQ(timer_control->irq_type);
    timer_control->registers->TASKS_STOP     = 1u;
    timer_control->registers->TASKS_SHUTDOWN = 1u;
}

void timer_start(timer_instance_t timer_instance)
{
    struct timer_control_block_t* const timer_control = timer_control_block(timer_instance);
    ASSERT(timer_control);

    timer_control->registers->INTENSET    = 1u;
    timer_control->registers->TASKS_START = 1u;
}

void timer_stop(timer_instance_t timer_instance)
{
    struct timer_control_block_t* const timer_control = timer_control_block(timer_instance);
    ASSERT(timer_control);

    timer_control->registers->TASKS_STOP = 1u;
}

void timer_reset(timer_instance_t timer_instance)
{
    struct timer_control_block_t* const timer_control = timer_control_block(timer_instance);
    ASSERT(timer_control);

    timer_control->registers->TASKS_CLEAR = 1u;
}

void timer_cc_set(timer_instance_t  timer_instance,
                  timer_cc_index_t  cc_index,
                  uint32_t          timer_ticks)
{
    struct timer_control_block_t* const timer_control = timer_control_block(timer_instance);
    ASSERT(timer_control);
    ASSERT(cc_index < timer_control->cc_alloc_count);

    timer_control->registers->CC[cc_index] = timer_ticks;
    timer_control->registers->INTENSET     = (1u << cc_index) << TIMER_INTENSET_COMPARE0_Pos;
}

uint32_t timer_cc_get(timer_instance_t timer_instance, timer_cc_index_t cc_index)
{
    struct timer_control_block_t* const timer_control = timer_control_block(timer_instance);
    ASSERT(timer_control);
    ASSERT(cc_index < timer_control->cc_alloc_count);
    return timer_control->registers->CC[cc_index];
}

uint32_t volatile* timer_cc_get_event(timer_instance_t timer_instance, timer_cc_index_t cc_index)
{
    struct timer_control_block_t* const timer_control = timer_control_block(timer_instance);

    ASSERT(timer_control);
    ASSERT(cc_index < std::size(timer_control->registers->EVENTS_COMPARE));

    return &timer_control->registers->EVENTS_COMPARE[cc_index];
}

uint32_t timer_cc_get_count(timer_instance_t timer_instance,
                            timer_cc_index_t cc_index)
{
    struct timer_control_block_t* const timer_control = timer_control_block(timer_instance);
    ASSERT(timer_control);

    timer_control->registers->TASKS_CAPTURE[cc_index] = 1u;
    uint32_t const timer_count = timer_control->registers->CC[cc_index];

    return timer_count;
}

void timer_cc_disable(timer_instance_t  timer_instance,
                      timer_cc_index_t  cc_index)
{
    struct timer_control_block_t* const timer_control = timer_control_block(timer_instance);
    ASSERT(timer_control);

    timer_control->registers->INTENCLR = (1u << cc_index) << TIMER_INTENCLR_COMPARE0_Pos;
}

uint32_t timer_ticks_per_second(timer_instance_t timer_instance)
{
    struct timer_control_block_t* const timer_control = timer_control_block(timer_instance);
    ASSERT(timer_control);

    return hfclk_frequency_Hz / (1u << timer_control->registers->PRESCALER);
}

void timer_enable_interrupt(timer_instance_t timer_instance)
{
    struct timer_control_block_t* const timer_control = timer_control_block(timer_instance);
    ASSERT(timer_control);

    NVIC_ClearPendingIRQ(timer_control->irq_type);
    NVIC_EnableIRQ(timer_control->irq_type);
}

static void irq_handler_timer(timer_control_block_t *timer_control)
{
    for (timer_cc_index_t cc_index = 0u; cc_index < timer_control->cc_alloc_count; ++cc_index)
    {
        if (timer_control->registers->EVENTS_COMPARE[cc_index])
        {
            uint32_t const cc_count = timer_control->registers->CC[cc_index];
            timer_control->handler(timer_control->context, cc_index, cc_count);
            timer_clear_compare_event(timer_control, cc_index);
        }
    }
}

extern "C" void timer_event_handler(void                *context,
                                    timer_cc_index_t    cc_index,
                                    uint32_t            cc_count)
{
    timer *timer_with_event = reinterpret_cast<timer*>(context);
    timer_with_event->event_notify(cc_index, cc_count);
}

timer::timer(timer_instance_t  timer_instance,
             uint8_t           prescaler_exp,
             uint8_t           irq_priority)
    : cc_alloc_count(timer_instances[timer_instance] ?
                     timer_instances[timer_instance]->cc_alloc_count : 0u),
    timer_instance_(timer_instance)
{
    timer_init(timer_instance,
               timer_mode_timer,
               prescaler_exp,
               irq_priority,
               timer_event_handler,
               this);
}

timer::~timer()
{
    timer_deinit(this->timer_instance_);
}

void timer::start()
{
    timer_start(this->timer_instance_);
}

void timer::stop()
{
    timer_stop(this->timer_instance_);
}

void timer::reset()
{
    timer_reset(this->timer_instance_);
}

void timer::cc_set(timer_cc_index_t cc_index, uint32_t timer_ticks)
{
    timer_cc_set(this->timer_instance_, cc_index, timer_ticks);
}

uint32_t timer::cc_get(timer_cc_index_t cc_index) const
{
    return timer_cc_get(this->timer_instance_, cc_index);
}

uint32_t volatile* timer::cc_get_event(timer_cc_index_t cc_index)
{
    return timer_cc_get_event(this->timer_instance_, cc_index);
}

uint32_t timer::cc_get_count(timer_cc_index_t cc_index) const
{
    return timer_cc_get_count(this->timer_instance_, cc_index);
}

void timer::cc_disable(timer_cc_index_t cc_index)
{
    timer_cc_disable(this->timer_instance_, cc_index);
}

uint32_t timer::ticks_per_second() const
{
    return timer_ticks_per_second(this->timer_instance_);
}

uint32_t timer::usec_to_ticks(uint32_t usec) const
{
    uint64_t usec64 = usec;
    usec64 *= this->ticks_per_second();
    usec64 /= 1000u * 1000u;
    return static_cast<uint32_t>(usec64);
}

uint32_t timer::msec_to_ticks(uint32_t msec) const
{
    uint64_t msec64 = msec;
    msec64 *= this->ticks_per_second();
    msec64 /= 1000u;
    return static_cast<uint32_t>(msec64);
}

