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
#include "gpio_te.h"
#include "nrf_cmsis.h"

#include "logger.h"
#include "segger_rtt_output_stream.h"
#include "project_assert.h"

#include <cstring>

class spi_test_timer: public timer_observer
{
public:
    spi_test_timer(expiration_type type, uint32_t expiry_ticks) :
        timer_observer(type, expiry_ticks) {}

    void expiration_notify() override;
};

static dma_size_t const spim_tx_length = 64u;
static uint8_t spim_tx_buffer[spim_tx_length];

static dma_size_t const spim_rx_length = 64u;
static uint8_t spim_rx_buffer[spim_rx_length];

static dma_size_t const spis_tx_length = 64u;
static uint8_t spis_tx_buffer[spis_tx_length];

static dma_size_t const spis_rx_length = 64u;
static uint8_t spis_rx_buffer[spis_rx_length];

static segger_rtt_output_stream rtt_os;

static uint32_t volatile spim_transfer_count = 0u;
static uint32_t volatile spis_transfer_count = 0u;

static uint8_t ramp_start_value = 0u;

static spi_port_t const spim_port = 0u;
static spi_port_t const spis_port = 1u;

static timer_observable<> timer_test_observable(1u);
static spi_test_timer spi_timer(timer_observer::expiration_type::continuous,
                                timer_test_observable.msec_to_ticks(10u));

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

static void spim_event_handler(void* context)
{
    led_state_set(2u, false);

    logger &logger = logger::instance();

    spim_transfer_count += 1u;
    logger.info("SPIM [%u] Rx:", spim_transfer_count);
    logger.write_data(logger::level::debug,
                      spim_rx_buffer,
                      spim_rx_length,
                      false,
                      write_data::data_prefix::address);

    if (spim_transfer_count > 1u)
    {
        // Check that the received data is what we expect.
        // The first transfer will have uninitialized data since the SPIS
        // has not received anything yet (should be all zeroes).
        ASSERT(memcmp(spis_tx_buffer, spim_rx_buffer, sizeof(spim_rx_buffer)) == 0);
    }
}

static void spim_start_transfer()
{
    led_state_set(2u, true);

    logger &logger = logger::instance();
    logger.debug("SPIM: start");

    enum spi_result_t spim_result = spim_transfer(
        spim_port,
        spim_tx_buffer, spim_tx_length,
        spim_rx_buffer, spim_rx_length,
        spim_event_handler,
        nullptr,
        0u);

    ASSERT(spim_result == spi_result_success);
}

static void spis_event_handler(void* context, struct spis_event_t const *event)
{
    logger &logger = logger::instance();
    led_state_set(1u, false);

    switch (event->type)
    {
    case spis_event_data_ready:
        logger.debug("SPIS: data ready (ignored)");
        // This is just the SPIS driver being impatient.
        // The SPIS will get prepared when transfer complete event gets hit
        // below. It can happen that the semaphore gets transfered back to the
        // SPIS driver before the transfer complete notification is sent.
        break;

    case spis_event_transfer_complete:
        spis_transfer_count += 1u;
        logger.info("SPIS [%u] Rx:", spis_transfer_count);
        logger.write_data(logger::level::debug,
                          spis_rx_buffer,
                          event->rx_length,
                          false,
                          write_data::data_prefix::address);

        // Check that the received data is what we expect.
        ASSERT(memcmp(spis_rx_buffer, spim_tx_buffer, event->rx_length) == 0);

        // What the SPIS received will be sent back to SPIM the next time
        // the transfer occurs.
        memcpy(spis_tx_buffer, spis_rx_buffer, event->rx_length);

        // Enable the SPIS to receive a new transfer.
        spis_enable_transfer(spis_port,
                             spis_tx_buffer, spis_tx_length,
                             spis_rx_buffer, spis_rx_length);
        break;

    default:
        ASSERT(0);
        break;
    }
}

void spi_test_timer::expiration_notify()
{
    led_state_set(1u, true);

    logger &logger = logger::instance();
    logger.info("SPIS [%u] enable:", spis_transfer_count);

    // Fill the SPIM tx buffer with increasing ramps of data.
    mem_fill_ramp(spim_tx_buffer, ramp_start_value, 1u, spim_tx_length);
    ramp_start_value += spim_tx_length;

    spim_start_transfer();
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

    logger.info("SPIM, SPIS test");
    logger.info("spi timer: %8u ticks", spi_timer.expiration_get_ticks());

    struct spi_config_t const spim_config = {
        .sck_pin        =  11u,
        .mosi_pin       =  12u,
        .miso_pin       =  13u,
        .ss_pin         =  14u,
        .irq_priority   =   7u,
        .orc            = 0xFFu,            // over-run char value.
        .output_drive   = gpio_drive_s1s0,
        .input_pull     = gpio_pull_none,
        .frequency      = SPI_FREQUENCY_FREQUENCY_M4,
        .mode           = spi_mode_0,
        .shift_order    = spi_shift_order_msb_first
    };

    struct spi_config_t const spis_config = {
        .sck_pin        =   6u,
        .mosi_pin       =   7u,
        .miso_pin       =   8u,
        .ss_pin         =   5u,
        .irq_priority   =   7u,
        .orc            = 0xFFu,            // over-run char value.
        .output_drive   = gpio_drive_s1s0,
        .input_pull     = gpio_pull_none,
        .frequency      = 0u,
        .mode           = spi_mode_0,
        .shift_order    = spi_shift_order_msb_first
    };

    uint8_t const gpio_te_irq_priority = 7u;
    gpio_te_init(gpio_te_irq_priority);

    enum spi_result_t spim_result = spim_init(spim_port, &spim_config);
    enum spi_result_t spis_result = spis_init(spis_port,
                                              &spis_config,
                                              spis_event_handler,
                                              nullptr);

    if (spim_result != spi_result_success)
    {
        logger.error("spim_init() failed: %d\n", spim_result);
    }

    if (spis_result != spi_result_success)
    {
        logger.error("spis_init() failed: %d\n", spis_result);
    }

    spis_enable_transfer(spis_port,
                         spis_tx_buffer, spis_tx_length,
                         spis_rx_buffer, spis_rx_length);

    // Don't start the spi test timer until the SPIS is ready.
    timer_test_observable.attach(spi_timer);

    while (true)
    {
        led_state_set(0u, false);
        __WFE();
        led_state_set(0u, true);

        logger.flush();
    }
}
