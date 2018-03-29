/**
 * @file leds.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "leds.h"
#include "project_assert.h"
#include "nrf_gpio.h"

// Set to true  for LEDs which are lit when the gpio pin drives high.
// Set to false for LEDs which are lit when the gpio pin drives low.
static bool const led_active_high = false;

// LEDs to gpio pin mapping.
static uint8_t const led_gpio_pin_list[] = { 17u, 18u, 19u, 20u };

led_index_t const led_count = sizeof(led_gpio_pin_list) / sizeof(led_gpio_pin_list[0]);

static bool led_polarity(bool value)
{
    return value ? led_active_high : (not led_active_high);
}

bool led_state_get(led_index_t led_index)
{
    ASSERT(led_index < led_count);
    bool const pin_value = nrf_gpio_pin_out_read(led_gpio_pin_list[led_index]);
    return led_polarity(pin_value);
}

void led_state_set(led_index_t led_index, bool enable)
{
    ASSERT(led_index < led_count);
    bool const pin_value = led_polarity(enable);
    nrf_gpio_pin_write(led_gpio_pin_list[led_index], pin_value);
}

void led_state_toggle(led_index_t led_index)
{
    ASSERT(led_index < led_count);
    nrf_gpio_pin_toggle(led_gpio_pin_list[led_index]);
}

void led_init(led_index_t led_index)
{
    nrf_gpio_cfg_output(led_gpio_pin_list[led_index]);
    led_state_set(led_index, false);
}

void leds_board_init(void)
{
    for (led_index_t led_index = 0u; led_index < led_count; ++led_index)
    {
        led_init(led_index);
    }
}

