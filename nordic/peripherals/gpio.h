/**
 * @file gpio.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "gpio_pin.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

enum gpio_direction_t
{
    gpio_direction_in       = 0u,
    gpio_direction_out      = 1u,
};

enum gpio_pull_t
{
    gpio_pull_none          = 0u,
    gpio_pull_down          = 1u,
    gpio_pull_up            = 3u,
};

enum gpio_drive_t
{   //                                  1       0
    gpio_drive_s1s0         = 0u,   // std     std
    gpio_drive_s1h0         = 1u,   // std     hard
    gpio_drive_h1s0         = 2u,   // hard    std
    gpio_drive_h1h0         = 3u,   // hard    hard
    gpio_drive_s1d0         = 4u,   // std     discon
    gpio_drive_h1d0         = 5u,   // hard    discon
    gpio_drive_d1s0         = 6u,   // discon  std
    gpio_drive_d1h0         = 7u,   // discon  hard
};

enum gpio_sense_level_t
{
    gpio_sense_disable      = 0u,
    gpio_sense_high         = 2u,
    gpio_sense_low          = 3u,
};

enum gpio_detect_mode_t
{
    gpio_detect_mode_direct  = 0u,
    gpio_detect_mode_latched = 1u,
};

/**
 * The generic function for configuring a Nordic nRF GPIO pin.
 *
 * @param pin_no    The pin number [0:31].
 * @param direction The direction: input or output.
 * @param pull      The pull up/down type.
 * @param drive     The drive strength and type.
 * @param sense     If the input level matches the sense setting then the
 *                  criteria for DETECT is met.
 *                  The LATCHED register can be read via
 *                  gpio_sense_detect_is_latched() to see if the input has hit
 *                  the sense value.
 *                  More importantly, the Port event can be triggered which can
 *                  wake the CPU from sleep EVENTS_PORT is enabled when the
 *                  DETECT criteria (sense) is met.
 */
void gpio_configure(gpio_pin_t                      pin_no,
                    enum gpio_direction_t           direction,
                    bool                            input_connect,
                    enum gpio_pull_t                pull,
                    enum gpio_drive_t               drive,
                    enum gpio_sense_level_t         sense);

/**
 * A specialized for setting an nRF GPIO pin as an input.
 * The input buffer is connected so that the input level can be read.
 *
 * @param pin_no    The pin number [0:31].
 * @param pull      The pull up/down type.
 * @param sense     If the input level matches the sense setting then the
 *                  criteria for DETECT is met.
 */
void gpio_configure_input(gpio_pin_t                pin_no,
                          enum gpio_pull_t          pull,
                          enum gpio_sense_level_t   sense);

/**
 * A specialized for setting an nRF GPIO pin as an input.
 * The input buffer is connected so that the output level can be read.
 *
 * @param pin_no    The pin number [0:31].
 * @param pull      The pull up/down type.
 * @param drive     The drive strength and type.
 */
void gpio_configure_output(gpio_pin_t               pin_no,
                           enum gpio_pull_t         pull,
                           enum gpio_drive_t        drive);

/**
 * Connect the input buffer from the input pin.
 * The input signal can be read.
 *
 * @param pin_no    The pin number [0:31].
 */
void gpio_connect_input_buffer(gpio_pin_t pin_no);

/**
 * Disonnect the input buffer from the input pin.
 * The input signal cannot be read.
 * Power consumption is reduced.
 *
 * @param pin_no    The pin number [0:31].
 */
void gpio_disconnect_input_buffer(gpio_pin_t pin_no);

/**
 * Set the sense level on the input pin.
 * When the input pit matches the level set then the detect criteria is met
 * and the latch bit for that pin will be set.
 *
 * @param pin_no    The pin number [0:31].
 * @param sense     The sense level for this pin.
 */
void gpio_set_sense_level(gpio_pin_t                pin_no,
                          enum gpio_sense_level_t   sense);

/**
 * Get the sense level for a pin
 *
 * @param pin_no                    The pin number [0:31].
 * @return enum gpio_sense_level_t  The sense level for this pin.
 */
enum gpio_sense_level_t gpio_get_sense_level(gpio_pin_t pin_no);

void gpio_set_sense_detect_mode_direct();
void gpio_set_sense_detect_mode_latched();

bool gpio_sense_detect_mode_is_direct();
bool gpio_sense_detect_mode_is_latched();
bool gpio_sense_detect_is_latched(gpio_pin_t pin_no);

bool gpio_pin_read(gpio_pin_t pin_no);
void gpio_pin_write(gpio_pin_t pin_no, bool level);

void gpio_pin_set(gpio_pin_t pin_no);
void gpio_pin_clear(gpio_pin_t pin_no);

void gpio_pin_toggle(gpio_pin_t pin_no);

#ifdef __cplusplus
}   // extern "C"
#endif
