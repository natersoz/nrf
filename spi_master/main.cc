/**
 * Copyright (c) 2015 - 2017, Nordic Semiconductor ASA
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "nrf_drv_spi.h"
#include "app_util_platform.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "boards.h"
#include "app_error.h"

#include "logger.h"
#include "segger_rtt_output_stream.h"

#include <string.h>

// nRF SPI driver instantiation
#define SPI_INSTANCE  0
static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);
static volatile bool spi_xfer_done;

#define TEST_STRING "Nordic"
static uint8_t       m_tx_buf[] = TEST_STRING;              /**< TX buffer. */
static uint8_t       m_rx_buf[sizeof(TEST_STRING) + 1];     /**< RX buffer. */
static const uint8_t m_length = sizeof(m_tx_buf);           /**< Transfer length. */

static segger_rtt_output_stream rtt_os;

void spi_event_handler(nrf_drv_spi_evt_t const *p_event, void *p_context)
{
    spi_xfer_done = true;
    logger &logger = logger::instance();

    logger.info("Transfer completed.");
    if (m_rx_buf[0] != 0)
    {
        logger.info("Received:");
        logger.write_data(logger::level::info,
                          m_rx_buf,
                          sizeof(m_rx_buf),
                          true,
                          write_data::data_prefix::address);
    }
}

int main(void)
{
    ret_code_t ret_code = NRF_SUCCESS;
    bsp_board_leds_init();

    logger& logger = logger::instance();
    logger.set_level(logger::level::debug);
    logger.set_output_stream(rtt_os);

    logger.info("SPI example.");

    nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
    spi_config.ss_pin   = SPI_SS_PIN;
    spi_config.miso_pin = SPI_MISO_PIN;
    spi_config.mosi_pin = SPI_MOSI_PIN;
    spi_config.sck_pin  = SPI_SCK_PIN;

    ret_code = nrf_drv_spi_init(&spi, &spi_config, spi_event_handler, NULL);
    if (ret_code != NRF_SUCCESS)
    {
        logger.error("nrf_drv_spi_init() failed: %d\n", ret_code);
    }

    while (1)
    {
        // Reset rx buffer and transfer done flag
        memset(m_rx_buf, 0, m_length);
        spi_xfer_done = false;

        ret_code = nrf_drv_spi_transfer(&spi,
                                        m_tx_buf,
                                        m_length,
                                        m_rx_buf,
                                        m_length);

        while (!spi_xfer_done)
        {
            __WFE();
        }

        logger.flush();

        bsp_board_led_invert(BSP_BOARD_LED_0);
        nrf_delay_ms(200);
    }
}
