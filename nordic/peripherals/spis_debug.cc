/**
 * @file spim_debug.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "spis_debug.h"
#include "nrf_cmsis.h"
#include "logger.h"

void log_spim_registers(NRF_SPIS_Type const *spis_registers)
{
    logger &logger = logger::instance();
    logger.debug("--- SPIS regs ---");
    logger.debug("TASKS_ACQUIRE   : 0x%08x", spis_registers->TASKS_ACQUIRE);
    logger.debug("TASKS_RELEASE   : 0x%08x", spis_registers->TASKS_RELEASE);
    logger.debug("EVENTS_END      : 0x%08x", spis_registers->EVENTS_END);
    logger.debug("EVENTS_ENDRX    : 0x%08x", spis_registers->EVENTS_ENDRX);
    logger.debug("EVENTS_ACQUIRED : 0x%08x", spis_registers->EVENTS_ACQUIRED);
    logger.debug("SHORTS          : 0x%08x", spis_registers->SHORTS);
    logger.debug("INTENSET        : 0x%08x", spis_registers->INTENSET);
    logger.debug("INTENCLR        : 0x%08x", spis_registers->INTENCLR);
    logger.debug("SEMSTAT         : 0x%08x", spis_registers->SEMSTAT);
    logger.debug("STATUS          : 0x%08x", spis_registers->STATUS);
    logger.debug("ENABLE          : 0x%08x", spis_registers->ENABLE);
    logger.debug("PSEL.SCK        : 0x%08x", spis_registers->PSEL.SCK);
    logger.debug("PSEL.MISO       : 0x%08x", spis_registers->PSEL.MISO);
    logger.debug("PSEL.MOSI       : 0x%08x", spis_registers->PSEL.MOSI);
    logger.debug("PSEL.CSN        : 0x%08x", spis_registers->PSEL.CSN);
    logger.debug("RXD.PTR         : 0x%08x", spis_registers->RXD.PTR);
    logger.debug("RXD.MAXCNT      : 0x%08x", spis_registers->RXD.MAXCNT);
    logger.debug("RXC.AMOUNT      : 0x%08x", spis_registers->RXD.AMOUNT);
    logger.debug("TXD.PTR         : 0x%08x", spis_registers->TXD.PTR);
    logger.debug("TXD.MAXCNT      : 0x%08x", spis_registers->TXD.MAXCNT);
    logger.debug("TXC.AMOUNT      : 0x%08x", spis_registers->TXD.AMOUNT);
    logger.debug("CONFIG          : 0x%08x", spis_registers->CONFIG);
    logger.debug("DEF             : 0x%08x", spis_registers->DEF);
    logger.debug("ORC             : 0x%08x", spis_registers->ORC);
    logger.debug("-----------------");
}

