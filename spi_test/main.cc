/**
 * @file spi_master/main.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "spim.h"
#include "spis.h"
#include "rtc.h"
#include "timer_observer.h"
#include "leds.h"
#include "clocks.h"

#include "nrf_gpio.h"
#include "nrf_delay.h"

#include "logger.h"
#include "segger_rtt_output_stream.h"
#include "project_assert.h"

#include <string.h>

class timer_spis_prepare: public timer_observer
{
public:
    timer_spis_prepare(expiration_type type, uint32_t expiry_ticks) :
        timer_observer(type, expiry_ticks) {}

    void expiration_notify() override;
};

class timer_spim_send: public timer_observer
{
public:
    timer_spim_send(expiration_type type, uint32_t expiry_ticks) :
        timer_observer(type, expiry_ticks) {}

    void expiration_notify() override;
};

static dma_size_t const spim_tx_length = 8u;
static uint8_t spim_tx_buffer[spim_tx_length];

static dma_size_t const spim_rx_length = 8u;
static uint8_t spim_rx_buffer[spim_rx_length];

static dma_size_t const spis_tx_length = 8u;
static uint8_t spis_tx_buffer[spis_tx_length];

static dma_size_t const spis_rx_length = 8u;
static uint8_t spis_rx_buffer[spis_rx_length];

static segger_rtt_output_stream rtt_os;

static bool volatile spim_xfer_done = false;
static bool volatile spis_xfer_done = false;

static uint8_t ramp_start_value = 0u;

static spi_port_t const spim_port = 0u;
static spi_port_t const spis_port = 1u;

static timer_observable timer_test_observable(1u);
static timer_spis_prepare timer_spis(timer_observer::expiration_type::continuous,
                                     timer_test_observable.ticks_per_second());

static timer_spim_send timer_spim(timer_observer::expiration_type::one_shot,
                                  timer_test_observable.msec_to_ticks(2u));

static void mem_fill_ramp(void *buffer,
                          uint8_t init_value,
                          uint8_t step_value,
                          size_t buffer_length)
{
    uint8_t *ptr = reinterpret_cast<uint8_t *>(buffer);
    uint8_t value = init_value;
    for ( ; buffer_length > 0; --buffer_length)
    {
        *ptr++ = value;
        value += step_value;
    }
}

void spim_event_handler(void* context)
{
    led_state_toggle(2u);
    logger &logger = logger::instance();

    logger.info("SPIM transfer completed.");
    spim_xfer_done = true;
    if (spim_rx_buffer[0u] != 0u)
    {
        logger.info("SPIM received:");
        logger.write_data(logger::level::info,
                          spim_rx_buffer,
                          spim_rx_length,
                          true,
                          write_data::data_prefix::address);
    }
}

void spis_event_handler(void* context, struct spis_event_t const *event)
{
    led_state_toggle(1u);

    logger &logger = logger::instance();

    logger.info("SPIS transfer completed.");
    spis_xfer_done = true;

    logger.info("SPIS received:");
    logger.write_data(logger::level::info,
                      spis_rx_buffer,
                      event->rx_amount,
                      true,
                      write_data::data_prefix::address);

    memcpy(spis_tx_buffer, spis_rx_buffer, event->tx_amount);
}

void timer_spis_prepare::expiration_notify()
{
    led_state_toggle(3u);
    logger &logger = logger::instance();
    logger.info("timer_spis: spis_enable_transfer()");

    spim_xfer_done = false;

    spis_enable_transfer(spis_port,
                         spis_tx_buffer,     spis_tx_length,
                         spis_rx_buffer,     spis_rx_length,
                         spis_event_handler, nullptr);

    mem_fill_ramp(spim_tx_buffer, ramp_start_value, 1u, spim_tx_length);
    ramp_start_value += spim_tx_length;

    timer_test_observable.attach(timer_spim);
}

void timer_spim_send::expiration_notify()
{
    timer_test_observable.detach(timer_spim);

    logger &logger = logger::instance();
    logger.info("timer_spim: spim_transfer()");

    enum spi_result_t spim_result = spim_transfer(
        spim_port,
        spim_tx_buffer, spim_tx_length,
        spim_rx_buffer, spim_rx_length,
        spim_event_handler,
        nullptr,
        0u);

    ASSERT(spim_result == SPI_RESULT_SUCCESS);
}

// RTC: 1024 ticks / second
static rtc rtc_1(1u, 32u);

int main()
{
    lfclk_enable(LFCLK_SOURCE_XO);
    rtc_1.start();
    leds_board_init();

    logger& logger = logger::instance();
    logger.set_level(logger::level::info);
    logger.set_output_stream(rtt_os);
    logger.set_rtc(rtc_1);

    logger.info("SPIM init");
    logger.info("timer spim: %8u ticks", timer_spim.expiration_get_ticks());
    logger.info("timer spis: %8u ticks", timer_spis.expiration_get_ticks());

    struct spi_config_t const spim_config = {
        .sck_pin        =  11u,
        .mosi_pin       =  12u,
        .miso_pin       =  13u,
        .ss_pin         =  14u,
        .irq_priority   =   7u,
        .orc            = 0xFFu,            // over-run char value.
        .output_drive   = NRF_GPIO_PIN_S0S1,
        .input_pull     = NRF_GPIO_PIN_NOPULL,
        .frequency      = SPI_FREQUENCY_FREQUENCY_M4,
        .mode           = SPI_MODE_0,
        .shift_order    = SPI_SHIFT_ORDER_MSB_FIRST
    };

    struct spi_config_t const spis_config = {
        .sck_pin        =   6u,
        .mosi_pin       =   7u,
        .miso_pin       =   8u,
        .ss_pin         =   5u,
        .irq_priority   =   7u,
        .orc            = 0xFFu,            // over-run char value.
        .output_drive   = NRF_GPIO_PIN_S0S1,
        .input_pull     = NRF_GPIO_PIN_NOPULL,
        .frequency      = 0u,
        .mode           = SPI_MODE_0,
        .shift_order    = SPI_SHIFT_ORDER_MSB_FIRST
    };

    timer_test_observable.attach(timer_spis);

    enum spi_result_t spim_result = spim_init(spim_port, &spim_config);
    enum spi_result_t spis_result = spis_init(spis_port, &spis_config);

    if (spim_result != SPI_RESULT_SUCCESS)
    {
        logger.error("spim_init() failed: %d\n", spim_result);
    }

    if (spis_result != SPI_RESULT_SUCCESS)
    {
        logger.error("spis_init() failed: %d\n", spis_result);
    }

    while (true)
    {
        __WFE();

        logger.flush();
        led_state_toggle(0u);
    }
}
