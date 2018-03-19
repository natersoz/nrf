/**
 * @file spim_debug.cc
 */

#include "spis_debug.h"
#include "nrf_cmsis.h"
#include "logger.h"

void log_spim_registers(NRF_SPIS_Type const *spis_registers)
{
    logger &logger = logger::instance();
    logger.info("--- SPIS regs ---");
    logger.info("TASKS_ACQUIRE   : 0x%08x", spis_registers->TASKS_ACQUIRE);
    logger.info("TASKS_RELEASE   : 0x%08x", spis_registers->TASKS_RELEASE);
    logger.info("EVENTS_END      : 0x%08x", spis_registers->EVENTS_END);
    logger.info("EVENTS_ENDRX    : 0x%08x", spis_registers->EVENTS_ENDRX);
    logger.info("EVENTS_ACQUIRED : 0x%08x", spis_registers->EVENTS_ACQUIRED);
    logger.info("SHORTS          : 0x%08x", spis_registers->SHORTS);
    logger.info("INTENSET        : 0x%08x", spis_registers->INTENSET);
    logger.info("INTENCLR        : 0x%08x", spis_registers->INTENCLR);
    logger.info("SEMSTAT         : 0x%08x", spis_registers->SEMSTAT);
    logger.info("STATUS          : 0x%08x", spis_registers->STATUS);
    logger.info("ENABLE          : 0x%08x", spis_registers->ENABLE);
    logger.info("PSEL.SCK        : 0x%08x", spis_registers->PSEL.SCK);
    logger.info("PSEL.MISO       : 0x%08x", spis_registers->PSEL.MISO);
    logger.info("PSEL.MOSI       : 0x%08x", spis_registers->PSEL.MOSI);
    logger.info("PSEL.CSN        : 0x%08x", spis_registers->PSEL.CSN);
    logger.info("RXD.PTR         : 0x%08x", spis_registers->RXD.PTR);
    logger.info("RXD.MAXCNT      : 0x%08x", spis_registers->RXD.MAXCNT);
    logger.info("RXC.AMOUNT      : 0x%08x", spis_registers->RXD.AMOUNT);
    logger.info("TXD.PTR         : 0x%08x", spis_registers->TXD.PTR);
    logger.info("TXD.MAXCNT      : 0x%08x", spis_registers->TXD.MAXCNT);
    logger.info("TXC.AMOUNT      : 0x%08x", spis_registers->TXD.AMOUNT);
    logger.info("CONFIG          : 0x%08x", spis_registers->CONFIG);
    logger.info("DEF             : 0x%08x", spis_registers->DEF);
    logger.info("ORC             : 0x%08x", spis_registers->ORC);
    logger.info("-----------------");
}

