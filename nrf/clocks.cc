/**
 * @file clocks.cc A Real Time Counter class based on the Nordic RTC peripheral.
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "clocks.h"
#include "nrf_cmsis.h"
#include "project_assert.h"

/**
 * @todo Things that should get implemented, but not expedient right now.
 * - Implement LFCLK CAL task, interrupt.
 * - Understand 'bypass' operation.
 */
void lfclk_enable(enum lfclk_source source)
{
    uint32_t source_bits = 0u;
    switch (source)
    {
    case LFCLK_SOURCE_RC:
        source_bits = (CLOCK_LFCLKSRC_SRC_RC << CLOCK_LFCLKSRC_SRC_Pos);
        break;
    case LFCLK_SOURCE_XO:
        source_bits = (CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos);
        break;
    case LFCLK_SOURCE_HF:
        source_bits = (CLOCK_LFCLKSRC_SRC_Synth << CLOCK_LFCLKSRC_SRC_Pos);
        break;
    default:
        ASSERT(0);
        break;
    }

    (void) source_bits;

    NRF_CLOCK->LFCLKSRC            = source_bits;
    NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_LFCLKSTART    = 1;

    while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0)
    {
        // Do nothing.
    }
}

void lfclk_disable(enum lfclk_source source)
{
    NRF_CLOCK->TASKS_LFCLKSTOP = 1u;
}

extern "C"
void POWER_CLOCK_IRQHandler(void)
{
}
