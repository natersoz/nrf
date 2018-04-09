/**
 * @file spim_debug.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "spim_debug.h"
#include "nrf_cmsis.h"
#include "logger.h"

void log_spim_registers(NRF_SPIM_Type const *spim_registers)
{
    logger &logger = logger::instance();
    logger.debug("--- SPIM regs ---");
    logger.debug("TASKS_START   : 0x%08x", spim_registers->TASKS_START);
    logger.debug("TASKS_STOP    : 0x%08x", spim_registers->TASKS_STOP);
    logger.debug("TASKS_SUSPEND : 0x%08x", spim_registers->TASKS_SUSPEND);
    logger.debug("TASKS_RESUME  : 0x%08x", spim_registers->TASKS_RESUME);
    logger.debug("EVENTS_STOPPED: 0x%08x", spim_registers->EVENTS_STOPPED);
    logger.debug("EVENTS_ENDRX  : 0x%08x", spim_registers->EVENTS_ENDRX);
    logger.debug("SHORTS        : 0x%08x", spim_registers->SHORTS);
    logger.debug("INTENSET      : 0x%08x", spim_registers->INTENSET);
    logger.debug("INTENCLR      : 0x%08x", spim_registers->INTENCLR);
    logger.debug("ENABLE        : 0x%08x", spim_registers->ENABLE);
    logger.debug("PSEL.SCK      : 0x%08x", spim_registers->PSEL.SCK);
    logger.debug("PSEL.MOSI     : 0x%08x", spim_registers->PSEL.MOSI);
    logger.debug("PSEL.MISO     : 0x%08x", spim_registers->PSEL.MISO);
    logger.debug("FREQUENCY     : 0x%08x", spim_registers->FREQUENCY);
    logger.debug("RXD.PTR       : 0x%08x", spim_registers->RXD.PTR);
    logger.debug("RXD.MAXCNT    : 0x%08x", spim_registers->RXD.MAXCNT);
    logger.debug("RXD.AMOUNT    : 0x%08x", spim_registers->RXD.AMOUNT);
    logger.debug("RXD.LIST      : 0x%08x", spim_registers->RXD.LIST);
    logger.debug("TXD.PTR       : 0x%08x", spim_registers->TXD.PTR);
    logger.debug("TXD.MAXCNT    : 0x%08x", spim_registers->TXD.MAXCNT);
    logger.debug("TXD.AMOUNT    : 0x%08x", spim_registers->TXD.AMOUNT);
    logger.debug("TXD.LIST      : 0x%08x", spim_registers->TXD.LIST);
    logger.debug("CONFIG        : 0x%08x", spim_registers->CONFIG);
    logger.debug("ORC           : 0x%08x", spim_registers->ORC);
    logger.debug("-----------------");
}

