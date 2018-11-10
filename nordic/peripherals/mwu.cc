/**
 * @file nordic/peripherals/mwu.cc
 * Enable/Disable the nRF5x Memory watch unit.
 */

#include "mwu.h"
#include "nrf_cmsis.h"

void mwu_enable(void)
{
    NRF_MWU->REGIONENSET =
        ((MWU_REGIONENSET_RGN0WA_Set  << MWU_REGIONENSET_RGN0WA_Pos) |
         (MWU_REGIONENSET_PRGN0WA_Set << MWU_REGIONENSET_PRGN0WA_Pos));
}

void mwu_disable(void)
{
    NRF_MWU->REGIONENCLR =
        ((MWU_REGIONENCLR_RGN0WA_Clear  << MWU_REGIONENCLR_RGN0WA_Pos) |
         (MWU_REGIONENCLR_PRGN0WA_Clear << MWU_REGIONENCLR_PRGN0WA_Pos));
}
