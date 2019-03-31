/**
 * @file twim_common.c
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "twi_common.h"
#include "project_assert.h"

void twi_pin_config(twi_gpio_config_t const* twi_gpio)
{
    ASSERT((twi_gpio->drive == gpio_drive_d1s0) ||
           (twi_gpio->drive == gpio_drive_d1h0));

    ASSERT((twi_gpio->pull == gpio_pull_none) ||
           (twi_gpio->pull == gpio_pull_up));

    bool const connect_pin_input = true;
    gpio_configure(twi_gpio->pin_no,
                   gpio_direction_in,
                   connect_pin_input,
                   twi_gpio->pull,
                   twi_gpio->drive,
                   gpio_sense_disable);
}

