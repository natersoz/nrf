/**
 * @file nrf_cmsis.h
 *
 * Based on symbols set in the Makefile specific to Nordic devices select the
 * proper set of header files. No deprecated or translations are included.
 */

#pragma once

/* Device selection for device includes. */
#if defined (NRF51)
    #include <nrf51.h>
    #include <nrf51_bitfields.h>
#elif defined (NRF52840_XXAA)
    #include <nrf52840.h>
    #include <nrf52840_bitfields.h>
#elif defined (NRF52832_XXAA) || defined (NRF52832_XXAB)
    #include <nrf52.h>
    #include <nrf52_bitfields.h>
#elif defined (NRF52810_XXAA)
    #include <nrf52810.h>
    #include <nrf52810_bitfields.h>
#else
    #error "Device must be defined. NRF52840_XXAA, NRF52832_XXAA, ..."
#endif

