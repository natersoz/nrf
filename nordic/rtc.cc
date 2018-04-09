/**
 * @file A Real Time Counter class based on the Nordic RTC peripheral.
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "rtc.h"
#include "project_assert.h"

struct rtc_control_block_t
{
    NRF_RTC_Type            *registers;
    IRQn_Type               irq_type;
    cc_index_t              cc_count;
    uint64_t                counter_extend;
    rtc_event_handler_t     handler;
    void                    *context;
};

static void irq_handler_rtc(rtc_control_block_t *rtc_control);

#if defined RTC0_ENABLED
static struct rtc_control_block_t rtc_instance_0 =
{
    .registers              = NRF_RTC0,
    .irq_type               = RTC0_IRQn,
    .cc_count               = 4u,
    .counter_extend         = 0u,
    .handler                = nullptr,
    .context                = nullptr,
};
static struct rtc_control_block_t* const rtc_instance_ptr_0 = &rtc_instance_0;

extern "C" void RTC0_IRQHandler(void)
{
    irq_handler_rtc(&rtc_instance_0);
}
#else
static struct rtc_control_block_t* const rtc_instance_ptr_0 = nullptr;
#endif  // RTC0_ENABLED

#if defined RTC1_ENABLED
static struct rtc_control_block_t rtc_instance_1 =
{
    .registers              = NRF_RTC1,
    .irq_type               = RTC1_IRQn,
    .cc_count               = 4u,
    .counter_extend         = 0u,
    .handler                = nullptr,
    .context                = nullptr,
};
static struct rtc_control_block_t* const rtc_instance_ptr_1 = &rtc_instance_1;

extern "C" void RTC1_IRQHandler(void)
{
    irq_handler_rtc(&rtc_instance_1);
}
#else
static struct rtc_control_block_t* const rtc_instance_ptr_1 = nullptr;
#endif  // RTC1_ENABLED

#if defined RTC2_ENABLED
static struct rtc_control_block_t rtc_instance_2 =
{
    .registers              = NRF_RTC2,
    .irq_type               = RTC2_IRQn,
    .cc_count               = 4u,
    .counter_extend         = 0u,
    .handler                = nullptr,
    .context                = nullptr,
};
static struct rtc_control_block_t* const rtc_instance_ptr_2 = &rtc_instance_2;

extern "C" void RTC2_IRQHandler(void)
{
    irq_handler_rtc(&rtc_instance_2);
}
#else
static struct rtc_control_block_t* const rtc_instance_ptr_2 = nullptr;
#endif  // RTC2_ENABLED

struct rtc_control_block_t* const rtc_instances[] =
{
    rtc_instance_ptr_0,
    rtc_instance_ptr_1,
    rtc_instance_ptr_2,
};

/// The High frequnecy clock which drives the RTC prescaler.
static uint32_t const lfclk_frequency_Hz = 32768;

/// The maximum prescaler allowed. The prescaler is an 11-bit unsigned value.
/// The RTC clock frequency is lfclk / (prescaler + 1u).
static uint32_t const prescaler_max = (1u << 11u);

/// The default RTC prescaler value.
/// This matches the RTC prescaler value on reset.
static uint32_t const prescaler_default = 0u;

/// The RTC counter bit width.
static size_t const rtc_counter_width = 24u;

static struct rtc_control_block_t* const rtc_control_block(rtc_instance_t rtc_instance)
{
    if (rtc_instance < sizeof(rtc_instances) / sizeof(rtc_instances[0]))
    {
        return rtc_instances[rtc_instance];
    }
    else
    {
        return nullptr;
    }
}

static void rtc_clear_compare_event(struct rtc_control_block_t* rtc_control,
                                    cc_index_t                  cc_index)
{
    rtc_control->registers->EVENTS_COMPARE[cc_index] = 0u;
    volatile uint32_t dummy = rtc_control->registers->EVENTS_COMPARE[cc_index];
    (void) dummy;
}

static void rtc_clear_overflow_event(struct rtc_control_block_t* rtc_control)
{
    rtc_control->registers->EVENTS_OVRFLW = 0u;
    volatile uint32_t dummy = rtc_control->registers->EVENTS_OVRFLW;
    (void) dummy;
}

void rtc_init(rtc_instance_t        rtc_instance,
              uint32_t              prescaler,
              uint8_t               irq_priority,
              rtc_event_handler_t   handler,
              void                  *context)
{
    struct rtc_control_block_t* const rtc_control = rtc_control_block(rtc_instance);
    ASSERT(rtc_control);
    ASSERT(prescaler <= prescaler_max);
    ASSERT(prescaler >  0u);
    ASSERT(rtc_control->registers->TASKS_START == 0u);
    ASSERT(handler);

    uint32_t const prescaler_reg_value  = ((prescaler - 1u) << RTC_PRESCALER_PRESCALER_Pos);

    rtc_control->counter_extend         = 0u;
    rtc_control->handler                = handler;
    rtc_control->context                = context;
    rtc_control->registers->PRESCALER   = prescaler_reg_value;
    rtc_control->registers->TASKS_STOP  = 1u;       // Make certain the RTC is stopped.
    rtc_control->registers->TASKS_CLEAR = 1u;
    rtc_control->registers->INTENCLR    = UINT32_MAX;

    rtc_clear_overflow_event(rtc_control);
    for (cc_index_t cc_index = 0u; cc_index < rtc_control->cc_count; ++cc_index)
    {
        rtc_clear_compare_event(rtc_control, cc_index);
        rtc_control->registers->CC[cc_index] = 0u;
    }

    NVIC_SetPriority(rtc_control->irq_type, irq_priority);
    NVIC_ClearPendingIRQ(rtc_control->irq_type);
    NVIC_EnableIRQ(rtc_control->irq_type);
}

void rtc_deinit(rtc_instance_t rtc_instance)
{
    struct rtc_control_block_t* const rtc_control = rtc_control_block(rtc_instance);
    ASSERT(rtc_control);

    NVIC_ClearPendingIRQ(rtc_control->irq_type);
    NVIC_DisableIRQ(rtc_control->irq_type);
    rtc_control->registers->TASKS_STOP     = 1u;
}

void rtc_start(rtc_instance_t rtc_instance)
{
    struct rtc_control_block_t* const rtc_control = rtc_control_block(rtc_instance);
    ASSERT(rtc_control);

    rtc_control->registers->INTENSET    = 1u << RTC_INTENSET_OVRFLW_Pos;
    rtc_control->registers->TASKS_START = 1u;
}

void rtc_stop(rtc_instance_t rtc_instance)
{
    struct rtc_control_block_t* const rtc_control = rtc_control_block(rtc_instance);
    ASSERT(rtc_control);

    rtc_control->registers->TASKS_STOP = 1u;
}

void rtc_reset(rtc_instance_t rtc_instance)
{
    struct rtc_control_block_t* const rtc_control = rtc_control_block(rtc_instance);
    ASSERT(rtc_control);

    rtc_control->registers->TASKS_CLEAR = 1u;
}

void rtc_cc_set(rtc_instance_t  rtc_instance,
                cc_index_t      cc_index,
                uint32_t        rtc_ticks)
{
    struct rtc_control_block_t* const rtc_control = rtc_control_block(rtc_instance);
    ASSERT(rtc_control);
    ASSERT(cc_index < rtc_control->cc_count);

    rtc_control->registers->CC[cc_index] = rtc_ticks;
    rtc_control->registers->INTENSET     = (1u << cc_index) << RTC_INTENSET_COMPARE0_Pos;
}

uint32_t rtc_cc_get(rtc_instance_t rtc_instance, cc_index_t cc_index)
{
    struct rtc_control_block_t* const rtc_control = rtc_control_block(rtc_instance);
    ASSERT(rtc_control);
    ASSERT(cc_index < rtc_control->cc_count);
    return rtc_control->registers->CC[cc_index];
}

uint32_t rtc_cc_get_count(rtc_instance_t rtc_instance, cc_index_t cc_index)
{
    (void) cc_index;
    struct rtc_control_block_t* const rtc_control = rtc_control_block(rtc_instance);
    ASSERT(rtc_control);

    uint32_t const rtc_count = rtc_control->registers->COUNTER;
    return rtc_count;
}

uint64_t rtc_get_count_ext(rtc_instance_t rtc_instance)
{
    struct rtc_control_block_t* const rtc_control = rtc_control_block(rtc_instance);
    ASSERT(rtc_control);

    uint32_t const rtc_count     = rtc_control->registers->COUNTER;
    uint64_t const rtc_count_ext = rtc_control->counter_extend + rtc_count;
    return rtc_count_ext;
}

void rtc_cc_disable(rtc_instance_t  rtc_instance,
                    cc_index_t      cc_index)
{
    struct rtc_control_block_t* const rtc_control = rtc_control_block(rtc_instance);
    ASSERT(rtc_control);

    rtc_control->registers->INTENCLR = (1u << cc_index) << RTC_INTENCLR_COMPARE0_Pos;
}

uint32_t rtc_ticks_per_second(rtc_instance_t rtc_instance)
{
    struct rtc_control_block_t* const rtc_control = rtc_control_block(rtc_instance);
    ASSERT(rtc_control);

    return lfclk_frequency_Hz / (rtc_control->registers->PRESCALER + 1);
}

void rtc_enable_interrupt(rtc_instance_t rtc_instance)
{
    struct rtc_control_block_t* const rtc_control = rtc_control_block(rtc_instance);
    ASSERT(rtc_control);

    NVIC_ClearPendingIRQ(rtc_control->irq_type);
    NVIC_EnableIRQ(rtc_control->irq_type);
}

static void irq_handler_rtc(rtc_control_block_t *rtc_control)
{
    // Handle the overflow event first so that observers will get
    // notified with the extended count value.
    if (rtc_control->registers->EVENTS_OVRFLW)
    {
        rtc_control->counter_extend += (1u << rtc_counter_width);
        rtc_clear_overflow_event(rtc_control);
    }

    for (cc_index_t cc_index = 0u; cc_index < rtc_control->cc_count; ++cc_index)
    {
        if (rtc_control->registers->EVENTS_COMPARE[cc_index])
        {
            uint32_t const cc_count = rtc_control->registers->CC[cc_index];
            rtc_control->handler(rtc_control->context, cc_index, cc_count);
            rtc_clear_compare_event(rtc_control, cc_index);
        }
    }
}

extern "C" void rtc_event_handler(void        *context,
                                  cc_index_t  cc_index,
                                  uint32_t    cc_count)
{
    rtc *rtc_with_event = reinterpret_cast<rtc*>(context);
    rtc_with_event->event_notify(cc_index, cc_count);
}

rtc::rtc(rtc_instance_t rtc_instance, uint8_t  prescaler, uint8_t irq_priority)
    : cc_count(rtc_instances[rtc_instance] ?
               rtc_instances[rtc_instance]->cc_count : 0u),
      rtc_instance_(rtc_instance)
{
    rtc_init(rtc_instance,
             prescaler,
             irq_priority,
             rtc_event_handler,
             this);
}

rtc::~rtc()
{
    rtc_deinit(this->rtc_instance_);
}

void rtc::start()
{
    rtc_start(this->rtc_instance_);
}

void rtc::stop()
{
    rtc_stop(this->rtc_instance_);
}

void rtc::reset()
{
    rtc_reset(this->rtc_instance_);
}

void rtc::cc_set(cc_index_t cc_index, uint32_t rtc_ticks)
{
    rtc_cc_set(this->rtc_instance_, cc_index, rtc_ticks);
}

uint32_t rtc::cc_get(cc_index_t cc_index) const
{
    return rtc_cc_get(this->rtc_instance_, cc_index);
}

uint32_t rtc::cc_get_count(cc_index_t cc_index) const
{
    return rtc_cc_get_count(this->rtc_instance_, cc_index);
}

uint32_t rtc::cc_get_count() const
{
    return rtc_cc_get_count(this->rtc_instance_, 0u);
}

uint32_t rtc::get_count_extend_32() const
{
    return static_cast<uint32_t>(this->get_count_extend_64());
}

uint64_t rtc::get_count_extend_64() const
{
    return rtc_get_count_ext(this->rtc_instance_);
}

void rtc::cc_disable(cc_index_t cc_index)
{
    rtc_cc_disable(this->rtc_instance_, cc_index);
}

uint32_t rtc::ticks_per_second() const
{
    return rtc_ticks_per_second(this->rtc_instance_);
}

uint32_t rtc::usec_to_ticks(uint32_t usec) const
{
    uint64_t usec64 = usec;
    usec64 *= this->ticks_per_second();
    usec64 /= 1000u * 1000u;
    return static_cast<uint32_t>(usec64);
}

uint32_t rtc::msec_to_ticks(uint32_t msec) const
{
    uint64_t msec64 = msec;
    msec64 *= this->ticks_per_second();
    msec64 /= 1000u;
    return static_cast<uint32_t>(msec64);
}

