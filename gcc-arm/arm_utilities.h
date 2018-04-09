/**
 * @file arm_utilities.h
 */

#pragma once

#include <cstddef>
#include "nrf_cmsis.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @todo Move these functions to a generic ARM utilities header file.
 *
 * @return bool
 */
static inline bool interrupt_context_check()
{
    return bool(SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk);
}

static inline bool is_valid_ram(void const *ptr, size_t length)
{
    /**
     * @todo use the linker file RAM region to determine whether the address
     * and length are within the valid RAM region. For now, hardcode this.
     * @todo For async SPI calls we should check that the RAM used is not
     * within stack range.
     */
    uintptr_t const __ram_begin = 0x20000000uL;
    uintptr_t const __ram_end   = __ram_begin + 64u * 1024u;

    uintptr_t const addr_begin = reinterpret_cast<uintptr_t>(ptr);
    uintptr_t const addr_end   = addr_begin + length;

    return (addr_begin >= __ram_begin) && (addr_end < __ram_end);
}

static inline bool is_valid_flash(void const *ptr, size_t length)
{
    /// @todo this would also be a nice to have.
    return true;
}

static inline bool interrupt_priority_is_valid(uint8_t irq_priority)
{
#if defined NRF51
    #ifdef SOFTDEVICE_PRESENT
        return (irq_priority == 1u) || (irq_priority == 3u);
    #else
        return (irq_priority < 4u);
    #endif
#else
    #ifdef SOFTDEVICE_PRESENT
        return ((irq_priority > 1u) && (irq_priority < 4u)) &&
               ((irq_priority > 4u) && (irq_priority < 8u));
    #else
        return (irq_priority < 8u);
    #endif
#endif
}

#ifdef __cplusplus
}
#endif

