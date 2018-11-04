/**
 * @file stack_usage.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Fill the unused region of stack with a 32-bit user defined patter.
 * @param pattern A 32-bit value.
 */
void stack_fill(uint32_t pattern);

/// @return size_t The number of bytes unused by the stack.
size_t stack_free(void);

/// @return size_t The number of bytes used by the stack.
size_t stack_used(void);

/// @return size_t The number of bytes allocated for use by the stack.
size_t stack_size(void);

#ifdef __cplusplus
}
#endif

