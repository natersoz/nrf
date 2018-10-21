/**
 * @file gpio_te.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "gpio_pin.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/// A type for enumerating, identifying Nordic GPIO Task/Event (TE) channels.
typedef uint8_t gpio_te_channel_t;

/**
 * The GPIO TE interrupt handler function signature.
 *
 * @param gpio_te_channel The GPIO TE channel which triggered the interrupt.
 * @param The user supplied context associated with GPIO TE events.
 */
typedef void (* gpio_te_pin_event_handler_t) (gpio_te_channel_t gpio_te_channel,
                                              void*             context);

typedef void (* gpio_te_port_event_handler_t) (uint32_t latch_detect_pins,
                                               void*    context);

/**
 * @enum gpio_te_polarity_t
 * Select the polarity for both GPIO TE tasks and events.
 * For tasks, this determines what output the pins will have when an event
 * occurs. rising: high level, falling: low level, toggle: change the level.
 * For events, this determins what triggers the event.
 * In this case toggle means that any change on the pin triggers the event.
 */
enum gpio_te_polarity_t
{
    gpio_te_polarity_none       = 0u,
    gpio_te_polarity_rising     = 1u,
    gpio_te_polarity_falling    = 2u,
    gpio_te_polarity_toggle     = 3u,
    gpio_te_polarity_limit      = 4u        ///< Used for range error checking.
};

/**
 * @enum gpio_te_output_init_t
 * For tasks, set the initial output value.
 */
enum gpio_te_output_init_t
{
    gpio_te_output_init_low     = 0u,
    gpio_te_output_init_high    = 1u,
    gpio_te_output_init_limit   = 2u        ///< Used for range error checking.
};

enum { gpio_te_channel_invalid = (gpio_te_channel_t)(-1) };

/**
 * Initialize the GPIO TE module.
 * @note Interrupt processing is optional and only relevant for GPIO TE events,
 * not tasks. If interrupts are to be ignored then parameters
 * event_handler and context should be set to null.
 * Setting the event_handler to null will disable GPIO TE pin related events.
 * @note Calling this function disables the port events.
 * To enable port events call gpio_te_port_enable().
 * Port events are handled independently of pin events.
 *
 * @param irq_priority The interrupt priority for handling GPIO events.
 */
void gpio_te_init(uint8_t irq_priority);

/**
 * Determine whether the GPIO TE module has been initialized.
 * @return bool true if it has been initialized; false if not.
 */
bool gpio_te_is_initialized(void);

/**
 * Enable the Port event.
 * @see nRF52832 Product Specification v1.4, 21.2 GPIOTE Port event
 *
 * Once GPIO pins have been configured as the source for DETECT, this funcion
 * can be called to enable the Port event.
 *
 * @param port_event_handler When the Port event occurs this function will be called.
 * @param port_event_context When the Port event occurs the port event handler
 *                           will be passed context as its parameter.
 * @param latched_detection  true Latches GPIO pin values to their 'detect' state
 *                           when the gpio is configure for detect. The latched
 *                           value is stored in the LATCH register.
 *                           false  TBD
 * @todo Determine the DETECT behavior - its not well documented.
 */
void gpio_te_port_enable(gpio_te_port_event_handler_t   port_event_handler,
                         void*                          port_event_context,
                         bool                           latched_detection);

/**
 * Disable the Port event.
 * @see nRF52832 Product Specification v1.4, 21.2 GPIOTE Port event
 * This function should be called prior to configuring GPIO pins as the source
 * for DETECT.
 */
void gpio_te_port_disable(void);

/**
 * Determine whether the GPIO TE channel is allocated or not.
 * @note This value is boolean opposite of calling gpio_te_channel_is_free().
 *
 * @param channel The GPIO TE channel.
 * @return bool true if allocated, false if free.
 */
bool gpio_te_channel_is_allocated(gpio_te_channel_t channel);

/**
 * Determine whether the GPIO TE channel is free or not; opposite of allocated.
 * @param channel The GPIO TE channel.
 * @return bool true if free, false if allocated.
 */
bool gpio_te_channel_is_free(gpio_te_channel_t channel);

/**
 * Determine if the GPIO TE channel is allocated as an event.
 *
 * @param channel The GPIO TE channel.
 * @return bool true if allocated as an event; false if free or
 *                   the channel is allocated as a task.
 */
bool gpio_te_channel_is_allocated_as_event(gpio_te_channel_t channel);

/**
 * Determine if the GPIO TE channel is allocated as a task.
 *
 * @param channel The GPIO TE channel.
 * @return bool true if allocated as a task; false if free or
 *                   the channel is allocated as an event.
 */
bool gpio_te_channel_is_allocated_as_task(gpio_te_channel_t channel);

/**
 * Allocate a GPIO TE channel for handling tasks;
 * that is, providing a GIPO output level when the attached event occurs.
 *
 * @param pin_no
 * @param polarity
 * @param initial_output
 * @param task_out_register_pointer
 * @param task_clr_register_pointer
 * @param task_set_register_pointer
 *
 * @return gpio_te_channel_t The GPIO TE channel allocated for use.
 * @retval gpio_te_channel_invalid If no channel was free for allocation.
 */
gpio_te_channel_t gpio_te_allocate_channel_task(
    gpio_pin_t                 pin_no,
    enum gpio_te_polarity_t    polarity,
    enum gpio_te_output_init_t initial_output);

/**
 * Allocate a GPIO TE channel for handling events.
 * - The event will trigger an interrupt which can be caught with the type
 *   gpio_te_event_handler_t passed into gpio_init().
 * - The event can be used to trigger a task.
 *
 * @param pin_no
 * @param polarity
 * @param event_register_pointer
 * @param pin_event_handler
 * @param pin_context
 *
 * @return gpio_te_channel_t The GPIO TE channel allocated for use.
 * @retval gpio_te_channel_invalid If no channel was free for allocation.
 */
gpio_te_channel_t gpio_te_allocate_channel_event(
    gpio_pin_t                  pin_no,
    enum gpio_te_polarity_t     polarity,
    uint32_t volatile*          event_register_pointer,
    gpio_te_pin_event_handler_t pin_event_handler,
    void*                       pin_context);

/**
 * Allocate a GPIO TE channel from use.
 * @param channel The GPIO TE channel.
 */
void gpio_te_channel_release(gpio_te_channel_t channel);

/**
 * Determine the GPIO pin assigned to a specific GPIO TE channel.
 *
 * @param channel The GPIO TE channel.
 * @return gpio_pin_t The pin assigned to the GPIO TE channel.
 * @retval (gpio_pin_t)(-1) If the channel is not allocated.
 */
gpio_pin_t gpio_te_channel_get_pin(gpio_te_channel_t channel);

/**
 * Enable the GPIO TE task. ASSERTs that a channel is allocated as a task.
 * @param channel The GPIO TE channel.
 */
void gpio_te_channel_enable_task(gpio_te_channel_t channel);

/**
 * Enable the GPIO TE event. ASSERTs that a channel is allocated as an event.
 * @param channel The GPIO TE channel.
 */
void gpio_te_channel_enable_event(gpio_te_channel_t channel);

/**
 * Disable the channel. The channel may be either a task or event.
 * @param channel The GPIO TE channel.
 */
void gpio_te_channel_disable(gpio_te_channel_t channel);

/**
 * @{
 * Get the task to a channel.
 * The channel must have already been allocated and no checking is performed.
 *
 * @param channel The GPIO TE channel.
 * @return uint32_t volatile* The task register pointer.
 */
uint32_t volatile* gpio_te_channel_get_task_out(gpio_te_channel_t channel);
uint32_t volatile* gpio_te_channel_get_task_clr(gpio_te_channel_t channel);
uint32_t volatile* gpio_te_channel_get_task_set(gpio_te_channel_t channel);
/** @} */

/**
 * Bind an event to a GPIO TE channel.
 *
 * @param channel The GPIO TE channel.
 * @param event_in_register_pointer
 */
void gpio_te_channel_bind_event(gpio_te_channel_t channel,
                                uint32_t volatile* event_in_register_pointer);

#ifdef __cplusplus
}   // extern "C"
#endif
