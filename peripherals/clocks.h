/**
 * @file clocks.h A Real Time Counter class based on the Nordic RTC peripheral.
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * @see Product Specification CLOCK - Clock Control
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum lfclk_source
{
    LFCLK_SOURCE_RC,
    LFCLK_SOURCE_XO,
    LFCLK_SOURCE_HF,
};

/**
 * Enables the 32,768 Hz clock source.
 *
 * The source of which can be:
 * - 32.768 kHz +/-250 ppm RC oscillator
 * - 32.768 kHz crystal oscillator, using external 32,768 Hz crystal
 * - 32.768 kHz oscillator synthesized from the 64 MHz oscillator
 *
 * @param source
 */
void lfclk_enable(enum lfclk_source source);
void lfclk_disable();

#ifdef __cplusplus
}
#endif


