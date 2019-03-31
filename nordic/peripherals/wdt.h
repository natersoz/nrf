/**
 * @file wdt.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Nordic Wathdog Timer.
 */

#pragma once

#include <stdint.h>

typedef void (* wdt_event_handler_t) (void* context);

/**
 * Determine the number of WDT timer ticks occur within a one second period.
 * @return uint32_t For the Nordic WDT this will be 32,768 ticks/second.
 */
uint32_t wdt_ticks_per_second();

/**
 * Convert milliseconds to WDT ticks.
 * @param msec      The number of milliseconds.
 * @return uint32_t The number of WDT timer ticks.
 */
uint32_t wdt_msec_to_ticks(uint32_t msec);

/**
 * Initialize the watchdog timer.
 * @note This does not start the WDT.
 * @note The configuration is setup so that the WDT does not expired when
 *       sleeping (WFE) or when halted by the debugger.
 *
 * @param irq_priority The interrupt priority to assign WDT interrupts.
 * @param handler      A handler to call when the WDT expires.
 * @param context      A user supplied context.
 */
void wdt_init(uint8_t               irq_priority,
              wdt_event_handler_t   handler,
              void*                 context);

/**
 * Start the WDT. Once started it cannot be stopped.
 *
 * @param ticks_expiration The number of WDT ticks to count before the WDT
 *                         triggers a timeout interrupt followed by a reset.
 */
void wdt_start(uint32_t ticks_expiration);

/**
 * Determine whether the WDT is started or not.
 * @return bool true if the WDT is started; false if not started.
 */
bool wdt_is_started();

/**
 * Service the WDT. This will cause the WDT to reset its expiration time.
 */
void wdt_service();
