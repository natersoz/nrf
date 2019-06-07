/**
 * @file stack_usage.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "stack_usage.h"
#include "cmsis_gcc.h"

static uint32_t fill_pattern_ = 0u;

// Alignment mask for 32-bit pointers.
constexpr uint32_t const mask_32 = ~0x03u;

extern uint32_t const __StackTop;
extern uint32_t const __StackLimit;

// Reminder: stack grows from top down.
// Stack is in use from __StackTop down to stack pointer.
void stack_fill(uint32_t pattern)
{
    fill_pattern_ = pattern;

    uintptr_t stack_ptr = __get_MSP();
    stack_ptr &= mask_32;

    uint32_t* stack_iter  = reinterpret_cast<uint32_t *>(stack_ptr);
    uint32_t* stack_limit = const_cast<uint32_t *>(&__StackLimit);
    stack_iter -= 4u;

    for ( ; stack_iter >= stack_limit; --stack_iter)
    {
        *stack_iter = fill_pattern_;
    }
}

size_t stack_free(void)
{
    for (uint32_t const* stack_iter = &__StackLimit; stack_iter < &__StackTop;
          ++stack_iter)
    {
        if (*stack_iter != fill_pattern_)
        {
            ptrdiff_t const free_32 = stack_iter - &__StackLimit;
            return free_32 * sizeof(uint32_t);
        }
    }

    // Unexpected: no stack used. Likely a bug in this code.
    return stack_size();
}

size_t stack_used(void)
{
    return stack_size() - stack_free();
}

size_t stack_size(void)
{
    return (&__StackTop - &__StackLimit) * sizeof(uint32_t);
}
