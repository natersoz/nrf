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

typedef uint8_t button_index_t;

extern button_index_t const button_count;

bool button_state_get(button_index_t button_index);
void button_init(button_index_t button_index);
void buttons_board_init(void);

#ifdef __cplusplus
}
#endif

