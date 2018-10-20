/**
 * @file leds.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "leds.h"
#include "project_assert.h"
#include "gpio.h"

#include <iterator>

// LEDs drive low to light the LED, pulled through 220 ohm R to VDD.
static bool const led_active_high = false;

// LEDs to gpio pin mapping.
static gpio_pin_t const led_gpio_pin_list[] = { 17u, 18u, 19u, 20u };

led_index_t const led_count = std::size(led_gpio_pin_list);

static bool led_polarity(bool value)
{
    return value ? led_active_high : (not led_active_high);
}

bool led_state_get(led_index_t led_index)
{
    ASSERT(led_index < led_count);
    bool const pin_value = gpio_pin_read(led_gpio_pin_list[led_index]);
    return led_polarity(pin_value);
}

void led_state_set(led_index_t led_index, bool enable)
{
    ASSERT(led_index < led_count);
    bool const pin_value = led_polarity(enable);
    gpio_pin_write(led_gpio_pin_list[led_index], pin_value);
}

void led_state_toggle(led_index_t led_index)
{
    ASSERT(led_index < led_count);
    gpio_pin_toggle(led_gpio_pin_list[led_index]);
}

void leds_board_init(void)
{
    led_index_t led_index = 0u;
    for (gpio_pin_t gpio_pin : led_gpio_pin_list)
    {
        gpio_configure_output(gpio_pin, gpio_pull_none, gpio_drive_s1h0);
        led_state_set(led_index++, false);
    }
}

