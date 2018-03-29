/**
 * @file leds.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t led_index_t;

extern led_index_t const led_count;

bool led_state_get(led_index_t led_index);
void led_state_set(led_index_t led_index, bool enable);
void led_state_toggle(led_index_t led_index);
void led_init(led_index_t led_index);
void leds_board_init(void);

#ifdef __cplusplus
}
#endif

