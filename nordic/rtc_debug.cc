/**
 * @file spim_debug.cc
 */

#include "rtc_debug.h"
#include "nrf_cmsis.h"
#include "logger.h"

void log_rtc_registers(NRF_RTC_Type const *rtc_registers)
{
    logger &logger = logger::instance();
    logger.info("--- RTC regs ---");
    logger.info("TASKS_START      : 0x%08x", rtc_registers->TASKS_START);
    logger.info("TASKS_STOP       : 0x%08x", rtc_registers->TASKS_STOP);
    logger.info("TASKS_CLEAR      : 0x%08x", rtc_registers->TASKS_CLEAR);
    logger.info("TASKS_TRIGOVRFLW : 0x%08x", rtc_registers->TASKS_TRIGOVRFLW);
    logger.info("EVENTS_TICK      : 0x%08x", rtc_registers->EVENTS_TICK);
    logger.info("EVENTS_OVRFLW    : 0x%08x", rtc_registers->EVENTS_OVRFLW);
    logger.info("EVENTS_COMPARE   : 0x%08x, 0x%08x, 0x%08x, 0x%08x",
                rtc_registers->EVENTS_COMPARE[0],
                rtc_registers->EVENTS_COMPARE[1],
                rtc_registers->EVENTS_COMPARE[2],
                rtc_registers->EVENTS_COMPARE[3]);
    logger.info("INTENSET         : 0x%08x", rtc_registers->INTENSET);
    logger.info("INTENCLR         : 0x%08x", rtc_registers->INTENCLR);
    logger.info("EVTEN            : 0x%08x", rtc_registers->EVTEN);
    logger.info("EVTENSET         : 0x%08x", rtc_registers->EVTENSET);
    logger.info("EVTENCLR         : 0x%08x", rtc_registers->EVTENCLR);
    logger.info("COUNTER          : 0x%08x", rtc_registers->COUNTER);
    logger.info("PRESCALER        : 0x%08x", rtc_registers->PRESCALER);
    logger.info("CC               : 0x%08x, 0x%08x, 0x%08x, 0x%08x",
                rtc_registers->CC[0],
                rtc_registers->CC[1],
                rtc_registers->CC[2],
                rtc_registers->CC[3]);
    logger.info("-----------------");
}

