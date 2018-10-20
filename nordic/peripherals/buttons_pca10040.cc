/**
 * @file buttons_pca10040.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "buttons.h"
#include "gpio.h"
#include "project_assert.h"

#include <iterator>

// Set to true  for buttons when pressed the logic leven is high.
// Set to false for buttons when pressed the logic leven is low.
static bool const button_active_high = false;

// The pull down configuration for the button gpio.
//static nrf_gpio_pin_pull_t const button_gpio_pull = NRF_GPIO_PIN_PULLUP;

// Buttons to gpio pin mapping.
static gpio_pin_t const button_gpio_pin_list[] = { 13u, 14u, 15u, 16u };
button_index_t const button_count = std::size(button_gpio_pin_list);

static bool button_polarity(bool value)
{
    return value ? button_active_high : (not button_active_high);
}

bool button_state_get(button_index_t button_index)
{
    ASSERT(button_index < button_count);
    bool const pin_value = gpio_pin_read(button_gpio_pin_list[button_index]);
    return button_polarity(pin_value);
}

void buttons_board_init(void)
{
    for (gpio_pin_t gpio_pin : button_gpio_pin_list)
    {
        enum gpio_sense_level_t const gpio_sense_level = button_active_high ?
                                                         gpio_sense_high :
                                                         gpio_sense_low  ;

        gpio_configure_input(gpio_pin, gpio_pull_up, gpio_sense_level);
    }
}

