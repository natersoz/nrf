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
    extern uint32_t __ram_begin__;
    extern uint32_t __ram_end__;

    uintptr_t const ram_begin = reinterpret_cast<uintptr_t>(&__ram_begin__);
    uintptr_t const ram_end   = reinterpret_cast<uintptr_t>(&__ram_end__);

    uintptr_t const addr_begin = reinterpret_cast<uintptr_t>(ptr);
    uintptr_t const addr_end   = addr_begin + length;

    return (addr_begin >= ram_begin) && (addr_end < ram_end);
}

static inline bool is_valid_flash(void const *ptr, size_t length)
{
    extern uint32_t __flash_begin__;
    extern uint32_t __flash_end__;

    uintptr_t const flash_begin = reinterpret_cast<uintptr_t>(&__flash_begin__);
    uintptr_t const flash_end   = reinterpret_cast<uintptr_t>(&__flash_end__);

    uintptr_t const addr_begin = reinterpret_cast<uintptr_t>(ptr);
    uintptr_t const addr_end   = addr_begin + length;

    return (addr_begin >= flash_begin) && (addr_end < flash_end);
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

/**
 * Get the ARM interrupt priority of the running ISR of execution.
 * The ARM Cortex M series Interrupt Program Status Register (IPSR)
 * returns the ISR number in bits [8:0] IPSR_ISR_Msk.
 * The return value of the IPSR ISR number is mapped as:
 *
 *     0: Thread mode      7-10: Reserved
 *     1: Reserved         11: SVCall
 *     2: NMI              12: Reserved for Debug
 *     3: HardFault        13: Reserved
 *     4: MemManage        14: PendSV
 *     5: BusFault         15: SysTick
 *     6: UsageFault       16: IRQ0.
 *
 * By subtracting 16 from the IPSR number we get back to the original Cortex
 * enum IRQn_Type
 *
 * https://developer.arm.com/docs/dui0553/latest/the-cortex-m4-processor/programmers-model/core-registers
 *
 * @return IRQn_Type The IRQ number when this function is called.
 */
static inline IRQn_Type interrupt_type_get(void)
{
    uint32_t const irq_num = __get_IPSR() & IPSR_ISR_Msk;
    return (IRQn_Type) (irq_num - 16);
}

static inline uint32_t interrupt_priority_get(void)
{
    IRQn_Type const irq_type = interrupt_type_get();
    return NVIC_GetPriority(irq_type);
}

#if 0
static inline uint8_t privilege_level_get(void)
{
#if __CORTEX_M == (0x00U) || defined(_WIN32) || defined(__unix) || defined(__APPLE__)
    /* the Cortex-M0 has no concept of privilege */
        /* Thread Mode, check nPRIV */
        int32_t control = __get_CONTROL();
        return control & CONTROL_nPRIV_Msk ? APP_LEVEL_UNPRIVILEGED : APP_LEVEL_PRIVILEGED;
    }
    else
    {
        /* Handler Mode, always privileged */
        return APP_LEVEL_PRIVILEGED;
    }
#endif
}
#endif

static inline bool debugger_detected(void)
{
#if defined(CoreDebug)
    return (CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk)? true : false;
#else
    return false;
#endif
}

#ifdef __cplusplus
}
#endif

