/**
 * @file wdt.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Nordic Watchdog Timer peripheral.
 */

#include "wdt.h"
#include "nrf_cmsis.h"
#include "logger.h"
#include "project_assert.h"

#include <algorithm>

static constexpr uint32_t const wdt_ticks_per_sec = 32'768u;

struct wdt_control_block_t
{
    /**
     * Pointer to the structure with WDT peripheral instance registers.
     * This must be NRF_WDT_BASE.
     */
    NRF_WDT_Type* const wdt_registers;

    /** WDT peripheral instance IRQ number. This must be WDT_IRQHandler */
    IRQn_Type const irq_type;

    /// The user supplied callback function.
    /// When the spi transfer is complete this function is called.
    wdt_event_handler_t handler;

    /// The user supplied context.
    /// This is carried by the TWI interface but never modified by the TWI driver.
    void *context;
};

static struct wdt_control_block_t wdt_instance =
{
    .wdt_registers          = reinterpret_cast<NRF_WDT_Type *>(NRF_WDT_BASE),
    .irq_type               = WDT_IRQn,
    .handler                = nullptr,
    .context                = nullptr,
};

static void wdt_clear_event_register(uint32_t volatile* wdt_register)
{
    *wdt_register = 0u;
    volatile uint32_t dummy = *wdt_register;
    (void) dummy;
}

uint32_t wdt_ticks_per_second()
{
    return wdt_ticks_per_sec;
}

uint32_t wdt_msec_to_ticks(uint32_t msec)
{
    uint32_t const max_msec = (1u << (32u - 15u));
    msec = std::min(msec, max_msec);
    uint32_t ticks = wdt_ticks_per_second();
    ticks *= msec;
    ticks /= 1000u;
    return ticks;
}

void wdt_init(uint8_t               irq_priority,
              wdt_event_handler_t   handler,
              void*                 context)
{
    wdt_instance.handler = handler;
    wdt_instance.context = context;

    // Pause the WDT when sleeping or halted by the debugger.
    wdt_instance.wdt_registers->CONFIG   = 0u;
    wdt_instance.wdt_registers->INTENSET = WDT_INTENSET_TIMEOUT_Msk;

    NVIC_SetPriority(wdt_instance.irq_type, irq_priority);
    NVIC_ClearPendingIRQ(wdt_instance.irq_type);
}

void wdt_start(uint32_t ticks_expiration)
{
    wdt_instance.wdt_registers->CRV         = ticks_expiration;
    wdt_instance.wdt_registers->RREN        = 1u;
    wdt_instance.wdt_registers->TASKS_START = 1u;
}

bool wdt_is_started()
{
    return bool(wdt_instance.wdt_registers->RUNSTATUS);
}

void wdt_service()
{
    wdt_instance.wdt_registers->RR[0] = WDT_RR_RR_Reload;
}

extern "C" void WDT_IRQHandler(void)
{
    logger &logger = logger.instance();
    logger.error("%s", __func__);

    __asm("bkpt #0");

    wdt_clear_event_register(&wdt_instance.wdt_registers->EVENTS_TIMEOUT);
    if (wdt_instance.handler)
    {
        wdt_instance.handler(wdt_instance.context);
    }
}
