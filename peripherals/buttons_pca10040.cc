/**
 * @file buttons_pca10040.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "buttons.h"
#include "nrf_gpio.h"
#include "project_assert.h"

// Set to true  for buttons when pressed the logic leven is high.
// Set to false for buttons when pressed the logic leven is low.
static bool const button_active_high = false;

// The pull down configuration for the button gpio.
static nrf_gpio_pin_pull_t const button_gpio_pull = NRF_GPIO_PIN_PULLUP;

// Buttons to gpio pin mapping.
static uint8_t const button_gpio_pin_list[] = { 13u, 14u, 15u, 16u };

button_index_t const button_count = sizeof(button_gpio_pin_list) / sizeof(button_gpio_pin_list[0]);

static bool button_polarity(bool value)
{
    return value ? button_active_high : (not button_active_high);
}

bool button_state_get(button_index_t button_index)
{
    ASSERT(button_index < button_count);
    bool const pin_value = nrf_gpio_pin_read(button_gpio_pin_list[button_index]);
    return button_polarity(pin_value);
}

void button_init(button_index_t button_index)
{
    nrf_gpio_cfg_input(button_gpio_pin_list[button_index], button_gpio_pull);
}

void buttons_board_init(void)
{
    for (button_index_t button_index = 0u; button_index < button_count; ++button_index)
    {
        button_init(button_index);
    }
}

