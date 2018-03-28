/**
 * @file spim_debug.cc
 */

#include "spim_debug.h"
#include "nrf_cmsis.h"
#include "logger.h"

void log_spim_registers(NRF_SPIM_Type const *spim_registers)
{
    logger &logger = logger::instance();
    logger.info("--- SPIM regs ---");
    logger.info("TASKS_START   : 0x%08x", spim_registers->TASKS_START);
    logger.info("TASKS_STOP    : 0x%08x", spim_registers->TASKS_STOP);
    logger.info("TASKS_SUSPEND : 0x%08x", spim_registers->TASKS_SUSPEND);
    logger.info("TASKS_RESUME  : 0x%08x", spim_registers->TASKS_RESUME);
    logger.info("EVENTS_STOPPED: 0x%08x", spim_registers->EVENTS_STOPPED);
    logger.info("EVENTS_ENDRX  : 0x%08x", spim_registers->EVENTS_ENDRX);
    logger.info("SHORTS        : 0x%08x", spim_registers->SHORTS);
    logger.info("INTENSET      : 0x%08x", spim_registers->INTENSET);
    logger.info("INTENCLR      : 0x%08x", spim_registers->INTENCLR);
    logger.info("ENABLE        : 0x%08x", spim_registers->ENABLE);
    logger.info("PSEL.SCK      : 0x%08x", spim_registers->PSEL.SCK);
    logger.info("PSEL.MOSI     : 0x%08x", spim_registers->PSEL.MOSI);
    logger.info("PSEL.MISO     : 0x%08x", spim_registers->PSEL.MISO);
    logger.info("FREQUENCY     : 0x%08x", spim_registers->FREQUENCY);
    logger.info("RXD.PTR       : 0x%08x", spim_registers->RXD.PTR);
    logger.info("RXD.MAXCNT    : 0x%08x", spim_registers->RXD.MAXCNT);
    logger.info("RXD.AMOUNT    : 0x%08x", spim_registers->RXD.AMOUNT);
    logger.info("RXD.LIST      : 0x%08x", spim_registers->RXD.LIST);
    logger.info("TXD.PTR       : 0x%08x", spim_registers->TXD.PTR);
    logger.info("TXD.MAXCNT    : 0x%08x", spim_registers->TXD.MAXCNT);
    logger.info("TXD.AMOUNT    : 0x%08x", spim_registers->TXD.AMOUNT);
    logger.info("TXD.LIST      : 0x%08x", spim_registers->TXD.LIST);
    logger.info("CONFIG        : 0x%08x", spim_registers->CONFIG);
    logger.info("ORC           : 0x%08x", spim_registers->ORC);
    logger.info("-----------------");
}

