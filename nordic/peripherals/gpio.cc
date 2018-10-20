/**
 * @file gpio.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "gpio.h"
#include "logger.h"
#include "nrf_cmsis.h"
#include "arm_utilities.h"
#include "project_assert.h"

// Note that the nrf CMSIS symbol NRF_GPIO_BASE was changed to NRF_P0_BASE.
// This module uses the new name NRF_P0_BASE.

static constexpr gpio_pin_t   const gpio_pin_limit       = 32u;
static constexpr unsigned int const gpio_direction_limit = 2u;
static constexpr unsigned int const gpio_pull_limit      = 4u;
static constexpr unsigned int const gpio_drive_limit     = 8u;
static constexpr unsigned int const gpio_sense_limit     = 4u;

void gpio_configure(gpio_pin_t                  pin_no,
                    enum gpio_direction_t       direction,
                    bool                        input_connect,
                    enum gpio_pull_t            pull,
                    enum gpio_drive_t           drive,
                    enum gpio_sense_level_t     sense)
{
    ASSERT(pin_no       < gpio_pin_limit);
    ASSERT(direction    < gpio_direction_limit);
    ASSERT(pull         < gpio_pull_limit);
    ASSERT(drive        < gpio_drive_limit);
    ASSERT(sense        < gpio_sense_limit);

    NRF_GPIO_Type* const gpio_registers = reinterpret_cast<NRF_GPIO_Type *>(NRF_P0_BASE);

    uint32_t const input  = input_connect ? GPIO_PIN_CNF_INPUT_Connect :
                                            GPIO_PIN_CNF_INPUT_Disconnect;
    uint32_t const config = (direction  << GPIO_PIN_CNF_DIR_Pos)        |
                            (input      << GPIO_PIN_CNF_INPUT_Pos)      |
                            (pull       << GPIO_PIN_CNF_PULL_Pos)       |
                            (drive      << GPIO_PIN_CNF_DRIVE_Pos)      |
                            (sense      << GPIO_PIN_CNF_SENSE_Pos)      |
                            0u;

    gpio_registers->PIN_CNF[pin_no] = config;
}

void gpio_configure_input(gpio_pin_t                pin_no,
                          enum gpio_pull_t          pull,
                          enum gpio_sense_level_t   sense)
{
    bool const input_connect = true;
    gpio_configure(pin_no,
                   gpio_direction_in,
                   input_connect,
                   pull,
                   gpio_drive_s1s0,
                   sense);
}

void gpio_configure_output(gpio_pin_t               pin_no,
                           enum gpio_pull_t         pull,
                           enum gpio_drive_t        drive)
{
    bool const input_connect = true;
    gpio_configure(pin_no,
                   gpio_direction_out,
                   input_connect,
                   pull,
                   drive,
                   gpio_sense_disable);
}

void gpio_connect_input_buffer(gpio_pin_t pin_no)
{
    NRF_GPIO_Type* const gpio_registers = reinterpret_cast<NRF_GPIO_Type *>(NRF_P0_BASE);
    gpio_registers->PIN_CNF[pin_no] &= ~GPIO_PIN_CNF_INPUT_Msk;
    gpio_registers->PIN_CNF[pin_no] |= GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos;
}

void gpio_disconnect_input_buffer(gpio_pin_t pin_no)
{
    NRF_GPIO_Type* const gpio_registers = reinterpret_cast<NRF_GPIO_Type *>(NRF_P0_BASE);
    gpio_registers->PIN_CNF[pin_no] &= ~GPIO_PIN_CNF_INPUT_Msk;
    gpio_registers->PIN_CNF[pin_no] |= GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos;
}

void gpio_set_sense_level(gpio_pin_t                pin_no,
                          enum gpio_sense_level_t   sense)
{
    ASSERT(pin_no < gpio_pin_limit);
    ASSERT(sense  < gpio_sense_limit);

    uint32_t sense_bits = static_cast<uint32_t>(sense) << GPIO_PIN_CNF_SENSE_Pos;
    NRF_GPIO_Type* const gpio_registers = reinterpret_cast<NRF_GPIO_Type *>(NRF_P0_BASE);
    gpio_registers->PIN_CNF[pin_no] &= GPIO_PIN_CNF_SENSE_Msk;
    gpio_registers->PIN_CNF[pin_no] |= sense_bits;
}

enum gpio_sense_level_t gpio_get_sense_level(gpio_pin_t pin_no)
{
    NRF_GPIO_Type* const gpio_registers = reinterpret_cast<NRF_GPIO_Type *>(NRF_P0_BASE);
    uint32_t const config = gpio_registers->PIN_CNF[pin_no];
    uint32_t const sense = (config & GPIO_PIN_CNF_SENSE_Msk) >> GPIO_PIN_CNF_SENSE_Pos;
    return static_cast<enum gpio_sense_level_t>(sense);
}

void gpio_set_sense_detect_mode_direct()
{
    NRF_GPIO_Type* const gpio_registers = reinterpret_cast<NRF_GPIO_Type *>(NRF_P0_BASE);
    gpio_registers->DETECTMODE = 0u;
}

void gpio_set_sense_detect_mode_latched()
{
    NRF_GPIO_Type* const gpio_registers = reinterpret_cast<NRF_GPIO_Type *>(NRF_P0_BASE);
    gpio_registers->DETECTMODE = 1u;
}

bool gpio_sense_detect_mode_is_direct()
{
    return not gpio_sense_detect_mode_is_latched();
}

bool gpio_sense_detect_mode_is_latched()
{
    NRF_GPIO_Type* const gpio_registers = reinterpret_cast<NRF_GPIO_Type *>(NRF_P0_BASE);
    return bool(gpio_registers->DETECTMODE);
}

bool gpio_sense_detect_is_latched(gpio_pin_t pin_no)
{
    ASSERT(pin_no < gpio_pin_limit);
    NRF_GPIO_Type* const gpio_registers = reinterpret_cast<NRF_GPIO_Type *>(NRF_P0_BASE);
    return bool(gpio_registers->LATCH & (1u << pin_no));
}

bool gpio_pin_read(gpio_pin_t pin_no)
{
    ASSERT(pin_no < gpio_pin_limit);
    NRF_GPIO_Type* const gpio_registers = reinterpret_cast<NRF_GPIO_Type *>(NRF_P0_BASE);

    return bool((gpio_registers->IN >> pin_no) & 1u);
}

void gpio_pin_write(gpio_pin_t pin_no, bool level)
{
    if (level)
    {
        gpio_pin_set(pin_no);
    }
    else
    {
        gpio_pin_clear(pin_no);
    }
}

void gpio_pin_set(gpio_pin_t pin_no)
{
    ASSERT(pin_no < gpio_pin_limit);
    NRF_GPIO_Type* const gpio_registers = reinterpret_cast<NRF_GPIO_Type *>(NRF_P0_BASE);
    gpio_registers->OUTSET = (1u << pin_no);
}

void gpio_pin_clear(gpio_pin_t pin_no)
{
    ASSERT(pin_no < gpio_pin_limit);
    NRF_GPIO_Type* const gpio_registers = reinterpret_cast<NRF_GPIO_Type *>(NRF_P0_BASE);
    gpio_registers->OUTCLR = (1u << pin_no);
}

void gpio_pin_toggle(gpio_pin_t pin_no)
{
    ASSERT(pin_no < gpio_pin_limit);
    NRF_GPIO_Type* const gpio_registers = reinterpret_cast<NRF_GPIO_Type *>(NRF_P0_BASE);
    gpio_registers->OUT ^= (1u << pin_no);
}

