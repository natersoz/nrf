/**
 * @file twi_test/main.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "twim.h"
#include "twis.h"
#include "rtc.h"
#include "timer_observer.h"
#include "leds.h"
#include "clocks.h"
#include "gpio_te.h"
#include "nrf_cmsis.h"

#include "logger.h"
#include "rtt_output_stream.h"
#include "segger_rtt.h"
#include "project_assert.h"

#include <iterator>

enum class twim_direction
{
    uninitialized,
    write,
    read,
};

class twi_test_timer: public timer_observer
{
public:
    twi_test_timer(expiration_type type, uint32_t expiry_ticks) :
        timer_observer(type, expiry_ticks) {}

    void expiration_notify() override;
};

static dma_size_t const twim_tx_length = 64u;
static uint8_t twim_tx_buffer[twim_tx_length];

static dma_size_t const twim_rx_length = 64u;
static uint8_t twim_rx_buffer[twim_rx_length];

static dma_size_t const twis_tx_length = 64u;
static uint8_t twis_tx_buffer[twis_tx_length];

static dma_size_t const twis_rx_length = 64u;
static uint8_t twis_rx_buffer[twis_rx_length];

static uint32_t volatile twim_write_count = 0u;
static uint32_t volatile twim_read_count = 0u;

static uint8_t ramp_start_value = 0u;

static twi_port_t const twim_port = 0u;
static twi_port_t const twis_port = 1u;

static timer_observable<> timer_test_observable(1u);
static twi_test_timer twi_timer(timer_observer::expiration_type::continuous,
                                timer_test_observable.msec_to_ticks(50u));

static uint8_t const i2c_addr = 0xA0u;
static_assert((i2c_addr & 0x01) == 0u);         // LSBit R/W must be zero.

static twim_direction twim_direction = twim_direction::uninitialized;

static void twim_event_handler(struct twim_event_t const* event, void* context);
static void twis_event_handler(struct twis_event_t const* event, void* context);

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

static void twis_event_handler(struct twis_event_t const *event, void* context)
{
    logger &logger = logger::instance();
    uint32_t type_bits = event->type;

    if (event->type & twi_event_tx_started)
    {
        logger.debug("twi S: Tx started");
        type_bits &= ~twi_event_tx_started;
    }

    if (event->type & twi_event_rx_started)
    {
        logger.debug("twi S: Rx started");
        type_bits &= ~twi_event_rx_started;
    }

    if (event->type & twis_event_write_cmd)
    {
        logger.debug("twi S: Write");
        type_bits &= ~twis_event_write_cmd;
    }

    if (event->type & twis_event_read_cmd)
    {
        logger.debug("twi S: Read");
        type_bits &= ~twis_event_read_cmd;
    }

    if (event->type & twi_event_tx_overrun)
    {
        led_state_set(2u, false);
        logger.debug("twi S: Tx Overrun");
        type_bits &= ~twi_event_tx_overrun;
    }

    if (event->type & twi_event_rx_overrun)
    {
        led_state_set(2u, false);
        logger.debug("twi S: Rx Overrun");
        type_bits &= ~twi_event_rx_overrun;
    }

    if (event->type & twi_event_stopped)
    {
        led_state_set(2u, false);
        logger.debug("twi S: Stopped");

        if (twim_direction == twim_direction::read)
        {
            // Check that the received data is what was sent by the master.
            size_t transfer_size = std::min(sizeof(twis_rx_buffer),
                                            sizeof(twis_tx_buffer));
            transfer_size = std::min(transfer_size, event->xfer.tx_bytes);
            ASSERT(std::equal(twis_tx_buffer,
                              twis_tx_buffer + transfer_size,
                              twim_rx_buffer));
        }
        else if (twim_direction == twim_direction::write)
        {
            size_t transfer_size = std::min(sizeof(twis_rx_buffer),
                                            sizeof(twis_tx_buffer));
            transfer_size = std::min(transfer_size, event->xfer.rx_bytes);

            // Check that the received data is what was sent by the master.
            ASSERT(std::equal(twis_rx_buffer,
                              twis_rx_buffer + transfer_size,
                              twim_tx_buffer));

            // What the twiS received will be sent back to twiM when the read
            // operation is performed.
            std::copy(twis_rx_buffer,
                      twis_rx_buffer + transfer_size,
                      twis_tx_buffer);
        }
        else
        {
            ASSERT(0);
        }
        type_bits &= ~twi_event_stopped;
    }

    if (type_bits)
    {
        logger.warn("twi S: unhandled bits: 0x%04x", type_bits);
    }
}

static void twim_start_write()
{
    logger &logger = logger::instance();
    logger.debug("twi M: start write");

    enum twi_result_t twim_result = twim_write(twim_port,
                                               i2c_addr,
                                               twim_tx_buffer,
                                               twim_tx_length,
                                               twim_event_handler,
                                               nullptr);

    ASSERT(twim_result == twi_result_success);
}

static void twim_start_read()
{
    logger &logger = logger::instance();
    logger.debug("twi M: start read");

    enum twi_result_t twim_result = twim_read(twim_port,
                                              i2c_addr,
                                              twim_rx_buffer,
                                              twim_rx_length,
                                              twim_event_handler,
                                              nullptr);

    ASSERT(twim_result == twi_result_success);
}

static void twim_event_handler(struct twim_event_t const* event, void* context)
{
    logger &logger = logger::instance();
    uint32_t type_bits = event->type;

    if (event->type & twi_event_tx_started)
    {
        logger.debug("twi M: Tx started");
        twim_direction = twim_direction::write;
        type_bits &= ~twi_event_tx_started;
    }

    if (event->type & twi_event_rx_started)
    {
        logger.debug("twi M: Rx started");
        twim_direction = twim_direction::read;
        type_bits &= ~twi_event_rx_started;
    }

    if (event->type & twim_event_suspended)
    {
        logger.debug("twi M: Suspend");
        type_bits &= ~twim_event_suspended;
    }

    if (event->type & twi_event_stopped)
    {
        led_state_set(1u, false);
        if (twim_direction == twim_direction::read)
        {
            twim_read_count += 1u;
            logger.debug("twi M [%u] Read Complete:", twim_read_count);
            logger.write_data(logger::level::debug,
                              twim_rx_buffer,
                              event->xfer.rx_bytes,
                              false,
                              io::data_prefix::address);
        }
        else if (twim_direction == twim_direction::write)
        {
            twim_write_count += 1u;
            logger.debug("twi M [%u] Write Complete:", twim_write_count);
            logger.write_data(logger::level::debug,
                              twim_tx_buffer,
                              event->xfer.rx_bytes,
                              false,
                              io::data_prefix::address);
        }
        else
        {
            ASSERT(0);
        }
        type_bits &= ~twi_event_stopped;
    }

    if (event->type & twim_event_addr_nack)
    {
        led_state_set(1u, false);
        logger.warn("twi M: Addr NACK");
        type_bits &= ~twim_event_addr_nack;
    }

    if (event->type & twi_event_data_nack)
    {
        led_state_set(1u, false);
        logger.warn("twi M: Addr NACK");
        type_bits &= ~twi_event_data_nack;
    }

    if (event->type & twi_event_rx_overrun)
    {
        led_state_set(1u, false);
        logger.warn("twi M: Rx Overrun");
        type_bits &= ~twi_event_rx_overrun;
    }

    if (type_bits)
    {
        logger.warn("twi M: unhandled bits: 0x%04x", type_bits);
    }
}

void twi_test_timer::expiration_notify()
{
    led_state_set(1u, true);
    led_state_set(2u, true);

    logger &logger = logger::instance();

    // Fill the twiM tx buffer with increasing ramps of data.
    mem_fill_ramp(twim_tx_buffer, ramp_start_value, 1u, twim_tx_length);
    ramp_start_value += twim_tx_length;

    if (twim_direction != twim_direction::write)
    {
        logger.info("twi[%6u] write: M -> S:", twim_write_count);
        twis_enable_write(twis_port, twis_rx_buffer, twis_rx_length);
        twim_start_write();
    }
    else
    {
        logger.info("twi[%6u] read:  M <- S:", twim_read_count);
        twis_enable_read(twis_port, twis_rx_buffer, twis_rx_length);
        twim_start_read();
    }
}

// RTC: 1024 ticks / second
static rtc  rtc_1(1u, 32u);
static char rtt_os_buffer[4096u];

int main()
{
    lfclk_enable(LFCLK_SOURCE_XO);
    rtc_1.start();
    leds_board_init();

    rtt_output_stream rtt_os(rtt_os_buffer, sizeof(rtt_os_buffer));
    logger& logger = logger::instance();
    logger.set_level(logger::level::info);
    logger.set_output_stream(rtt_os);
    logger.set_rtc(rtc_1);
    segger_rtt_enable();

    logger.info("----- twiM, twiS test -----");
    logger.info("twi timer: %8u ticks", twi_timer.expiration_get_ticks());

    /* Intent is to be compatible with the SPI test setup.
     * The SPI Master assigns pins thusly:
     *     .sck_pin        =  11u       <-- Use as TWIM SCL
     *     .mosi_pin       =  12u       <-- Use as TWIM SDA
     *     .miso_pin       =  13u
     *     .ss_pin         =  14u
     * Use the built in pull ups; approx 13 K ohms.
     */
    struct twim_config_t const twim_config = {
        .pin_scl    = {
            .pin_no = 11u,
            .pull   = gpio_pull_up,
            .drive  = gpio_drive_d1s0,
        },
        .pin_sda    = {
            .pin_no = 12u,
            .pull   = gpio_pull_up,
            .drive  = gpio_drive_d1s0,
        },
        .clock_freq     = twim_clk_freq_400k,
        .irq_priority   = 7u,
    };

    /* Intent is to be compatible with the SPI test setup.
     * The SPI Slave assigns pins thusly:
     *     .sck_pin        =   6u,      <-- Use as TWIS SCL
     *     .mosi_pin       =   7u,      <-- Use as TWIS SDA
     *     .miso_pin       =   8u,
     *     .ss_pin         =   5u,
     * Use the built in pull ups; approx 13 K ohms.
     * Note that this is now 1/2 the value (6.5 K) since they are in parallel.
     * We can use the standard drive output gpio_drive_d1h0 since the pull-ups
     * are less than 10 K.
     */
    struct twis_config_t const twis_config = {
        .pin_scl        = {
            .pin_no     = 6u,
            .pull       = gpio_pull_up,
            .drive      = gpio_drive_d1s0,
        },
        .pin_sda        = {
            .pin_no     = 7u,
            .pull       = gpio_pull_up,
            .drive      = gpio_drive_d1s0,
        },
        .slave_addr     = { 0xA0, twi_addr_invalid },
        .irq_priority   = 7u,
        .orc            = 0xFFu,            // over-run char value.
    };

    enum twi_result_t twim_result = twim_init(twim_port, &twim_config);
    enum twi_result_t twis_result = twis_init(twis_port, &twis_config,
                                              twis_event_handler,
                                              nullptr);

    if (twim_result != twi_result_success)
    {
        logger.error("twim_init() failed: %d\n", twim_result);
    }

    if (twis_result != twi_result_success)
    {
        logger.error("twis_init() failed: %d\n", twis_result);
    }

    // Don't start the twi test timer until the twiS is ready.
    timer_test_observable.attach(twi_timer);

    while (true)
    {
        led_state_set(0u, false);
        __WFE();
        led_state_set(0u, true);

        logger.flush();
    }
}

