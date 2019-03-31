/**
 * @file twi_common.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Common data types for both the TWIM and TWIS driver modules.
 */

#pragma once

#include <stdint.h>
#include "gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

// The NRF52840 allows for 16-bit DMA length transfers.
// The NRF52810 allows for 10-bite DMA length transfers,
//     downgraded to 8-bit here.
// The NRF52832 allows for 8-bit DMA length transfers.
#if defined (NRF52840_XXAA)
    typedef uint16_t dma_size_t;
#else
    typedef uint8_t dma_size_t;
#endif

typedef uint8_t twi_port_t;

struct twi_gpio_config_t
{
    gpio_pin_t          pin_no;
    enum gpio_pull_t    pull;
    enum gpio_drive_t   drive;
};

/**
 * Contains the addressing type. Allow for 10 bit I2C addressing.
 */
typedef uint16_t twi_addr_t;

#define twi_addr_invalid    ((twi_addr_t)(-1))

enum twi_result_t
{
    twi_result_success = 0,

    /// Attempt to initialize and already initialized TWIM control block.
    twi_result_invalid_state,

    /// A TWI RX transfer is already in progress.
    twi_result_rx_busy,

    /// A TWI TX transfer is already in progress.
    twi_result_tx_busy,
};

void twi_pin_config(twi_gpio_config_t const* twi_gpio);

enum
{
    twi_event_none          = 0u,
    twi_event_stopped       = (1u <<  0u),
    twi_event_tx_started    = (1u <<  1u),
    twi_event_rx_started    = (1u <<  2u),
    twis_event_write_cmd    = (1u <<  3u),
    twis_event_read_cmd     = (1u <<  4u),
    twim_event_suspended    = (1u <<  5u),
    twi_event_tx_overrun    = (1u <<  6u),
    twi_event_rx_overrun    = (1u <<  7u),
    twim_event_addr_nack    = (1u <<  8u),
    twi_event_data_nack     = (1u <<  9u)
};

#ifdef __cplusplus
}
#endif
