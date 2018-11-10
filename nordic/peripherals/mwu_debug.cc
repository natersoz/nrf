/**
 * @file nordic/peripherals/mwu_debug.cc
 * Enable/Disable the nRF5x Memory watch unit.
 */

#include "mwu.h"
#include "nrf_cmsis.h"
#include "logger.h"

#include <iterator>

void mwu_debug(void)
{
    logger& logger = logger::instance();

    logger.debug("----- MWU:");
    logger.debug("INTEN:          0x%08x", NRF_MWU->INTEN);
    logger.debug("NMIEN:          0x%08x", NRF_MWU->NMIEN);
    logger.debug("REGIONEN:       0x%08x", NRF_MWU->REGIONEN);

    for (size_t index = 0u; index < std::size(NRF_MWU->REGION); ++index)
    {
        logger.debug("REGION[%u]:     [0x%08x:0x%08x]", index,
                     NRF_MWU->REGION[index].START, NRF_MWU->REGION[index].END);
    }

    for (size_t index = 0u; index < std::size(NRF_MWU->PREGION); ++index)
    {
        logger.debug("PREGION[%u]:    [0x%08x:0x%08x]", index,
                     NRF_MWU->PREGION[index].START,
                     NRF_MWU->PREGION[index].END);
    }

    for (size_t index = 0u; index < std::size(NRF_MWU->EVENTS_REGION); ++index)
    {
        if (NRF_MWU->EVENTS_REGION[index].RA || NRF_MWU->EVENTS_REGION[index].WA)
        {
            logger.error("EVT  REG[%u] RA: 0x%08x, WA: 0x%08x", index,
                         NRF_MWU->EVENTS_REGION[index].RA,
                         NRF_MWU->EVENTS_REGION[index].WA);
        }
        else
        {
            logger.debug("EVT  REG[%u] RA: 0x%08x, WA: 0x%08x", index,
                         NRF_MWU->EVENTS_REGION[index].RA,
                         NRF_MWU->EVENTS_REGION[index].WA);
        }
    }

    for (size_t index = 0u; index < std::size(NRF_MWU->EVENTS_PREGION); ++index)
    {
        if (NRF_MWU->EVENTS_PREGION[index].RA || NRF_MWU->EVENTS_PREGION[index].WA)
        {
            logger.error("EVT PREG[%u] RA: 0x%08x, WA: 0x%08x", index,
                         NRF_MWU->EVENTS_PREGION[index].RA,
                         NRF_MWU->EVENTS_PREGION[index].WA);
        }
        else
        {
            logger.debug("EVT PREG[%u] RA: 0x%08x, WA: 0x%08x", index,
                         NRF_MWU->EVENTS_PREGION[index].RA,
                         NRF_MWU->EVENTS_PREGION[index].WA);
        }
    }
}
