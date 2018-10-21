/**
 * @file gpio_te.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "gpio_te.h"
#include "logger.h"
#include "nrf_cmsis.h"
#include "arm_utilities.h"
#include "project_assert.h"

#include <iterator>
#include <cstddef>

enum gpio_te_mode_t
{
    gpio_to_mode_disable        = 0u,
    gpio_to_mode_event          = 1u,
    gpio_to_mode_task           = 3u,
};

static size_t const config_count   = std::size(reinterpret_cast<NRF_GPIOTE_Type *>(NRF_GPIOTE_BASE)->CONFIG);
static size_t const event_in_count = std::size(reinterpret_cast<NRF_GPIOTE_Type *>(NRF_GPIOTE_BASE)->EVENTS_IN);
static size_t const task_out_count = std::size(reinterpret_cast<NRF_GPIOTE_Type *>(NRF_GPIOTE_BASE)->TASKS_OUT);
static size_t const task_clr_count = std::size(reinterpret_cast<NRF_GPIOTE_Type *>(NRF_GPIOTE_BASE)->TASKS_CLR);
static size_t const task_set_count = std::size(reinterpret_cast<NRF_GPIOTE_Type *>(NRF_GPIOTE_BASE)->TASKS_SET);

// Unfortunately the sizes above do not resolve at compile time.
// Likely due to their volatile type nature.
// Set the channel count here and check it at run time against the others.
static constexpr gpio_te_channel_t const gpio_te_channel_count = 8u;

// The maximum number of NRF GPIO pins that can be accessed within a GPIO port.
static constexpr gpio_pin_t const gpio_pin_limit = 32u;

struct gpio_te_control_block_t
{
    /// Pointer to the GPIO TE peripheral instance registers.
    NRF_GPIOTE_Type* const gpio_te_registers;
    NRF_GPIO_Type*   const gpio_registers;

    /**
     * GPIO TE peripheral instance IRQ number.
     * Since there is only one GPIO TE per SoC, this is set to GPIOTE_IRQn.
     */
    IRQn_Type const irq_type;

    /// The number of GPIO TE channels available on the device.
    gpio_te_channel_t const channel_count;

    /// Is the GPIO TE module initialized? true if so, false if not.
    bool initialized;

    /**
     * @{
     * Each GPIO TE channel is allocated a event callback handler and a context
     * since each pin and its event are independent of each other and occur for
     * different reasons.
     */
    gpio_te_pin_event_handler_t     pin_event_handlers[gpio_te_channel_count];
    void*                           pin_event_contexts[gpio_te_channel_count];
    /** @} */

    /**
     * @{
     * The PORT event handler and stored user context.
     */
    gpio_te_port_event_handler_t    port_event_handler;
    void*                           port_event_context;
    /** @} */
};

static struct gpio_te_control_block_t gpio_te_instance_0 =
{
    .gpio_te_registers      = reinterpret_cast<NRF_GPIOTE_Type *>(NRF_GPIOTE_BASE),
    .gpio_registers         = reinterpret_cast<NRF_GPIO_Type *>(NRF_P0_BASE),
    .irq_type               = GPIOTE_IRQn,
    .channel_count          = gpio_te_channel_count,
    .initialized            = false,

    .pin_event_handlers     = {nullptr},
    .pin_event_contexts     = {nullptr},

    .port_event_handler     = nullptr,
    .port_event_context     = nullptr,
};

static void irq_handler_gpio_te(struct gpio_te_control_block_t* gpio_te_control);

extern "C" void GPIOTE_IRQHandler(void)
{
    irq_handler_gpio_te(&gpio_te_instance_0);
}

static void gpio_te_clear_event_register(uint32_t volatile* gpio_te_register)
{
    *gpio_te_register = 0u;
    volatile uint32_t dummy = *gpio_te_register;
    (void) dummy;
}

void gpio_te_init(uint8_t irq_priority)
{
    // If the module GPIO TE channel count value does not match the
    // Noridc peripheral count values then catch it here.
    // If it fails compilation is likely an NRF51 device.
    ASSERT(gpio_te_channel_count == config_count);
    ASSERT(gpio_te_channel_count == event_in_count);
    ASSERT(gpio_te_channel_count == task_out_count);
    ASSERT(gpio_te_channel_count == task_clr_count);
    ASSERT(gpio_te_channel_count == task_set_count);

    ASSERT(interrupt_priority_is_valid(irq_priority));

    gpio_te_instance_0.initialized = true;

    // Disable all interrupts.
    gpio_te_instance_0.gpio_te_registers->INTENCLR = UINT32_MAX;

    // Clear the port event.
    // Also clears port_event_handler, port_event_context.
    gpio_te_port_disable();

    // Clear all gpio te channel events and configuration.
    // Also clears pin_event_handlers[] and pin_event_contexts[].
    for (gpio_te_channel_t channel = 0u; channel < gpio_te_instance_0.channel_count; ++channel)
    {
        gpio_te_channel_disable(channel);
        gpio_te_instance_0.gpio_te_registers->CONFIG[channel] = 0u;
    }

    NVIC_SetPriority(gpio_te_instance_0.irq_type, irq_priority);
    NVIC_ClearPendingIRQ(gpio_te_instance_0.irq_type);
    NVIC_EnableIRQ(gpio_te_instance_0.irq_type);

    logger::instance().debug("channel count: %u", gpio_te_instance_0.channel_count);
}

bool gpio_te_is_initialized(void)
{
    return gpio_te_instance_0.initialized;
}

void gpio_te_port_enable(gpio_te_port_event_handler_t   port_event_handler,
                         void*                          port_context,
                         bool                           latched_detection)
{
    gpio_te_instance_0.port_event_handler = port_event_handler;
    gpio_te_instance_0.port_event_context = port_context;

    uint32_t const detect_mode = latched_detection ?
        (GPIO_DETECTMODE_DETECTMODE_LDETECT << GPIO_DETECTMODE_DETECTMODE_Pos) :
        (GPIO_DETECTMODE_DETECTMODE_Default << GPIO_DETECTMODE_DETECTMODE_Pos) ;

    gpio_te_instance_0.gpio_registers->DETECTMODE  = detect_mode;
    gpio_te_instance_0.gpio_registers->LATCH       = UINT32_MAX;
    gpio_te_instance_0.gpio_te_registers->INTENCLR = GPIOTE_INTENSET_PORT_Msk;
    gpio_te_clear_event_register(&gpio_te_instance_0.gpio_te_registers->EVENTS_PORT);
    gpio_te_instance_0.gpio_te_registers->INTENSET = GPIOTE_INTENSET_PORT_Msk;
}

void gpio_te_port_disable(void)
{
    gpio_te_instance_0.gpio_te_registers->INTENCLR = GPIOTE_INTENSET_PORT_Msk;
    gpio_te_clear_event_register(&gpio_te_instance_0.gpio_te_registers->EVENTS_PORT);

    gpio_te_instance_0.port_event_handler = nullptr;
    gpio_te_instance_0.port_event_context = nullptr;
}

bool gpio_te_channel_is_allocated(gpio_te_channel_t channel)
{
    ASSERT(channel < gpio_te_instance_0.channel_count);

    return gpio_te_channel_is_allocated_as_task(channel) ||
           gpio_te_channel_is_allocated_as_event(channel);
}

bool gpio_te_channel_is_free(gpio_te_channel_t channel)
{
    return not gpio_te_channel_is_allocated(channel);
}

bool gpio_te_channel_is_allocated_as_task(gpio_te_channel_t channel)
{
    ASSERT(channel < gpio_te_instance_0.channel_count);

    uint32_t const config = gpio_te_instance_0.gpio_te_registers->CONFIG[channel];
    bool const config_task = (config & GPIOTE_CONFIG_MODE_Msk) ==
                             (GPIOTE_CONFIG_MODE_Task << GPIOTE_CONFIG_MODE_Pos);
    return config_task;
}

bool gpio_te_channel_is_allocated_as_event(gpio_te_channel_t channel)
{
    ASSERT(channel < gpio_te_instance_0.channel_count);

    uint32_t const config = gpio_te_instance_0.gpio_te_registers->CONFIG[channel];
    bool const config_event = (config & GPIOTE_CONFIG_MODE_Msk) ==
                              (GPIOTE_CONFIG_MODE_Event << GPIOTE_CONFIG_MODE_Pos);
    return config_event ||
        bool(gpio_te_instance_0.gpio_te_registers->EVENTS_IN[channel]) ||
        bool(gpio_te_instance_0.pin_event_handlers[channel]);
}

gpio_te_channel_t gpio_te_allocate_channel_task(
    gpio_pin_t                 pin_no,
    enum gpio_te_polarity_t    polarity,
    enum gpio_te_output_init_t initial_output)
{
    ASSERT(pin_no         < gpio_pin_limit);
    ASSERT(polarity       < gpio_te_polarity_limit);
    ASSERT(initial_output < gpio_te_output_init_limit);

    for (gpio_te_channel_t channel = 0u; channel < gpio_te_instance_0.channel_count; ++channel)
    {
        if (gpio_te_channel_is_free(channel))
        {
            *gpio_te_channel_get_task_out(channel) = 0u;    // Clear all tasks
            *gpio_te_channel_get_task_clr(channel) = 0u;
            *gpio_te_channel_get_task_set(channel) = 0u;

            uint32_t const config =
                (GPIOTE_CONFIG_MODE_Task               << GPIOTE_CONFIG_MODE_Pos)     |
                (static_cast<uint32_t>(pin_no)         << GPIOTE_CONFIG_PSEL_Pos)     |
                (static_cast<uint32_t>(polarity)       << GPIOTE_CONFIG_POLARITY_Pos) |
                (static_cast<uint32_t>(initial_output) << GPIOTE_CONFIG_OUTINIT_Pos)  |
                0u;

            gpio_te_instance_0.gpio_te_registers->CONFIG[channel] = config;
            return channel;
        }
    }

    // Failed to allocate a GPIO TE channel.
    return gpio_te_channel_invalid;
}

gpio_te_channel_t gpio_te_allocate_channel_event(
    gpio_pin_t                  pin_no,
    enum gpio_te_polarity_t     polarity,
    uint32_t volatile*          event_register_pointer,
    gpio_te_pin_event_handler_t pin_event_handler,
    void*                       pin_context)
{
    ASSERT(pin_no   < gpio_pin_limit);
    ASSERT(polarity < gpio_te_polarity_limit);

    // A GPIO event must either trigger an interrupt or trigger a task or both.
    ASSERT(bool(event_register_pointer) || bool(pin_event_handler));

    // An interrupt context only makese sense if there is a handler.
    if (pin_context) { ASSERT(pin_event_handler); }

    for (gpio_te_channel_t channel = 0u; channel < gpio_te_instance_0.channel_count; ++channel)
    {
        if (gpio_te_channel_is_free(channel))
        {
            uint32_t const config =
                (GPIOTE_CONFIG_MODE_Event           << GPIOTE_CONFIG_MODE_Pos)     |
                (static_cast<uint32_t>(pin_no)      << GPIOTE_CONFIG_PSEL_Pos)     |
                (static_cast<uint32_t>(polarity)    << GPIOTE_CONFIG_POLARITY_Pos) |
                0u;

            gpio_te_instance_0.pin_event_handlers[channel] = pin_event_handler;
            gpio_te_instance_0.pin_event_contexts[channel] = pin_context;

            gpio_te_instance_0.gpio_te_registers->CONFIG[channel] = config;
            gpio_te_channel_bind_event(channel, event_register_pointer);

            return channel;
        }
    }

    // Failed to allocate a GPIO TE channel.
    return gpio_te_channel_invalid;
}

void gpio_te_channel_release(gpio_te_channel_t channel)
{
    ASSERT(gpio_te_channel_is_allocated(channel));

    gpio_te_channel_disable(channel);
    gpio_te_instance_0.gpio_te_registers->CONFIG[channel] = 0u;

     gpio_te_channel_bind_event(  channel,   nullptr);
    *gpio_te_channel_get_task_out(channel) = 0u;
    *gpio_te_channel_get_task_clr(channel) = 0u;
    *gpio_te_channel_get_task_set(channel) = 0u;

    gpio_te_instance_0.pin_event_handlers[channel] = nullptr;
    gpio_te_instance_0.pin_event_contexts[channel] = nullptr;
}

gpio_pin_t gpio_te_channel_get_pin(gpio_te_channel_t channel)
{
    ASSERT(channel < gpio_te_instance_0.channel_count);

    if (gpio_te_channel_is_allocated(channel))
    {
        uint32_t const config = gpio_te_instance_0.gpio_te_registers->CONFIG[channel];
        gpio_pin_t const pin_no = static_cast<gpio_pin_t> (
            (config & GPIOTE_CONFIG_PSEL_Msk) >> GPIOTE_CONFIG_PSEL_Pos);
        return pin_no;
    }
    else
    {
        // If the channel is not allocated then the pin cannot be known.
        return static_cast<gpio_pin_t>(-1);
    }
}

void gpio_te_channel_enable_task(gpio_te_channel_t channel)
{
    ASSERT(gpio_te_channel_is_allocated_as_task(channel));

    gpio_te_channel_disable(channel);

    // Set the configuration for the channel to 'task'.
    // Interrupts are not enabled for GPIO TE task channels.
    gpio_te_instance_0.gpio_te_registers->CONFIG[channel] |=
        (GPIOTE_CONFIG_MODE_Task << GPIOTE_CONFIG_MODE_Pos);
}

void gpio_te_channel_enable_event(gpio_te_channel_t channel)
{
    ASSERT(gpio_te_channel_is_allocated_as_event(channel));

    gpio_te_channel_disable(channel);

    // Set the configuration for the channel to 'event'.
    gpio_te_instance_0.gpio_te_registers->CONFIG[channel] |=
        (GPIOTE_CONFIG_MODE_Event << GPIOTE_CONFIG_MODE_Pos);

    if (gpio_te_instance_0.pin_event_handlers[channel])
    {
        // Enable the channel interrupt for events if there is a
        // event handler associated with the channel.
        gpio_te_instance_0.gpio_te_registers->INTENSET =
            (GPIOTE_INTENSET_IN0_Msk << channel);
    }
}

void gpio_te_channel_disable(gpio_te_channel_t channel)
{
    ASSERT(channel < gpio_te_instance_0.channel_count);

    // Clear configuration for the channel.
    gpio_te_instance_0.gpio_te_registers->CONFIG[channel] &=
        ~(GPIOTE_CONFIG_MODE_Msk << GPIOTE_CONFIG_MODE_Pos);

    // Disable the event interrupt. Do it even if its allocated a task.
    gpio_te_instance_0.gpio_te_registers->INTENCLR =
        (GPIOTE_INTENSET_IN0_Pos << channel);

    // Clear events which may have been queued.
    gpio_te_clear_event_register(&gpio_te_instance_0.gpio_te_registers->EVENTS_IN[channel]);
}

uint32_t volatile* gpio_te_channel_get_task_out(gpio_te_channel_t channel)
{
    ASSERT(channel < gpio_te_instance_0.channel_count);
    return &gpio_te_instance_0.gpio_te_registers->TASKS_OUT[channel];
}

uint32_t volatile* gpio_te_channel_get_task_clr(gpio_te_channel_t channel)
{
    ASSERT(channel < gpio_te_instance_0.channel_count);
    return &gpio_te_instance_0.gpio_te_registers->TASKS_CLR[channel];
}

uint32_t volatile* gpio_te_channel_get_task_set(gpio_te_channel_t channel)
{
    ASSERT(channel < gpio_te_instance_0.channel_count);
    return &gpio_te_instance_0.gpio_te_registers->TASKS_SET[channel];
}

void gpio_te_channel_bind_event(gpio_te_channel_t channel,
                                uint32_t volatile* event_in_register_pointer)
{
    ASSERT(channel < gpio_te_instance_0.channel_count);
    gpio_te_instance_0.gpio_te_registers->EVENTS_IN[channel] =
        reinterpret_cast<uint32_t>(event_in_register_pointer);
}

static void irq_handler_gpio_te(struct gpio_te_control_block_t* gpio_te_control)
{
    logger &logger = logger::instance();

    if (gpio_te_control->gpio_te_registers->EVENTS_PORT)
    {
        gpio_te_clear_event_register(&gpio_te_control->gpio_te_registers->EVENTS_PORT);

        /**
         * I have not idea why this works, but it does. Clearing the LATCH before
         * reading should reset its value to zero, but it does not.
         * LATCH carries the correct value when it should be zero.
         *
         * If LATCH is written after reading then the value of the previous
         * EVENTS_PORT is bit-wise or'd with the current value most of the time.
         *
         * This is probably related to:
         * Errata nRF52832 Rev2, v1.0 3.44 [173]
         * GPIO: Writes to LATCH register take several CPU cycles to take effect
         * Conditions:   Reading the LATCH register right after writing to it.
         * Consequences: Old value of the LATCH register is read.
         * Workaround:   Have at least 3 CPU cycles of delay between the write
         *               and the subsequent read to the LATCH register.
         *               This can be achieved by having 3 dummy reads to the
         *               LATCH register.
         *
         * @note I have tried the errata workaround with no success.
         *       Also tried reading the NRF_GPIO_Type::IN - which has the bit
         *       expected set, but others set as well.
         */
        gpio_te_control->gpio_registers->LATCH = UINT32_MAX;
        uint32_t const latched = gpio_te_control->gpio_registers->LATCH;

        logger.debug("GPIO TE event: port, latched: 0x%08x", latched);
        gpio_te_control->port_event_handler(latched,
                                            gpio_te_control->port_event_context);
    }

    for (gpio_te_channel_t channel = 0u; channel < gpio_te_instance_0.channel_count; ++channel)
    {
        if (gpio_te_control->gpio_te_registers->EVENTS_IN[channel])
        {
            if (channel < gpio_te_control->channel_count)
            {
                gpio_te_clear_event_register(
                    &gpio_te_control->gpio_te_registers->EVENTS_IN[channel]);

                logger.debug("GPIO TE event: channel[%u]", channel);

                if (gpio_te_control->pin_event_handlers[channel])
                {
                    gpio_te_control->pin_event_handlers[channel](
                        channel,
                        gpio_te_control->pin_event_contexts[channel]);
                }
            }
            else
            {
                logger.error("irq_handler_gpio_te: invalid channel: %u / %u",
                             channel < gpio_te_control->channel_count);
            }
        }
    }
}


/**
 * The event will be generated on the rising edge of the DETECT signal.
 * See GPIO — General purpose input/output on page 111 for more information
 * about the DETECT signal.
 *
 * Putting the system into System ON IDLE while DETECT is high will not
 * cause DETECT to wake the system up again.
 * Make sure to clear all DETECT sources before entering sleep.
 *
 * If the LATCH register is used as a source,
 * if any bit in LATCH is still high after clearing all or part of the register
 * (for instance due to one of the PINx.DETECT signal still high),
 * a new rising edge will be generated on DETECT, see Pin configuration on page 111.
 *
 * Trying to put the system to System OFF while DETECT is high will cause a wakeup
 * from System OFF reset.
 * This feature is always enabled although the peripheral itself appears to be IDLE.
 * i.e. no clocks or other power intensive infrastructure have to be requested to
 * keep this feature enabled.
 *
 * This feature can therefore be used to wake up the CPU from a WFI or WFE type
 * sleep in System ON with all peripherals and the CPU idle,
 * i.e. lowest power consumption in System ON mode.
 *
 * In order to prevent spurious interrupts from the PORT event while configuring
 * the sources, the user shall first disable interrupts on the PORT event
 * (through INTENCLR.PORT), then configure the sources (PIN_CNF[n].SENSE),
 * clear any potential event that could have occurred during configuration
 * (write '1' to EVENTS_PORT), and finally enable interrupts (through INTENSET.PORT).
 */
