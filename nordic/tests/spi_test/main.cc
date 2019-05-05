/**
 * @file spi_test/main.cc
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
#include "rtt_output_stream.h"
#include "segger_rtt.h"
#include "project_assert.h"

#include <array>
#include <iterator>
#include <utility>

// For debug printing of SPI data I/O.
static constexpr bool const print_data = false;

class spi_test_timer: public timer_observer
{
public:
    using timer_observer::timer_observer;

    spi_test_timer(expiration_type type, uint32_t expiry_ticks) :
        timer_observer(type, expiry_ticks) {}

    void expiration_notify() override;
};

constexpr size_t const spi_buffer_length = 64u;
using spi_buffer = std::array<uint8_t, spi_buffer_length>;

struct spi_double_buffer
{
    ~spi_double_buffer()                                   = default;
    spi_double_buffer(spi_double_buffer const&)            = delete;
    spi_double_buffer(spi_double_buffer &&)                = delete;
    spi_double_buffer& operator=(spi_double_buffer const&) = delete;
    spi_double_buffer& operator=(spi_double_buffer&&)      = delete;

    spi_double_buffer() : index_to_queue(0u), index_enqueued(0u) {}

    size_t index_increment(size_t index) const {
        index += 1u;
        return (index >= this->buffers.size())? 0u : index;
    }

    spi_buffer const& to_queue() const {
        ASSERT(this->index_to_queue < this->buffers.size());
        return this->buffers[this->index_to_queue];
    }

    spi_buffer& to_queue() {
        return const_cast<spi_buffer&>(std::as_const(*this).to_queue());
    }

    spi_buffer const& enqueued() const {
        ASSERT(this->index_enqueued < this->buffers.size());
        return this->buffers[this->index_enqueued];
    }

    spi_buffer& enqueued() {
        return const_cast<spi_buffer&>(std::as_const(*this).enqueued());
    }

    size_t                      index_to_queue;
    size_t                      index_enqueued;
    std::array<spi_buffer, 2u>  buffers;
};

static spi_buffer spim_mosi_buffer;
static spi_buffer spim_miso_buffer;

static spi_double_buffer spis_miso_buffer;
static spi_double_buffer spis_mosi_buffer;

static uint32_t volatile spim_transfer_count = 0u;
static uint32_t volatile spis_transfer_count = 0u;

static uint8_t ramp_start_value = 0u;

static spi_port_t const spim_port = 0u;
static spi_port_t const spis_port = 1u;

static timer_observable<> timer_test_observable(1u);
static spi_test_timer spi_timer(timer_observer::expiration_type::continuous,
                                timer_test_observable.msec_to_ticks(10u));

static void mem_fill_ramp(uint8_t*  ptr,
                          uint8_t   init_value,
                          uint8_t   step_value,
                          size_t    buffer_length)
{
    uint8_t value = init_value;
    for ( ; buffer_length > 0; --buffer_length)
    {
        *ptr++ = value;
        value += step_value;
    }
}

static void spim_event_handler(struct spi_event_t const* event, void* context)
{
    led_state_set(2u, false);

    logger &logger = logger::instance();

    switch (event->type)
    {
    case spi_event_data_ready:
        logger.debug("SPIM [%u] ready: "
                     "mosi:(0x%p, %04x), miso:(0x%p, %04x) -- ignored",
                     spim_transfer_count,
                     event->mosi_pointer, event->mosi_length,
                     event->miso_pointer, event->miso_length);
        break;

    case spi_event_transfer_complete:
        spim_transfer_count += 1u;
        logger.debug("SPIM [%u] xfer:  "
                     "mosi:(0x%p, %04x), miso:(0x%p, %04x)",
                     spim_transfer_count,
                     event->mosi_pointer, event->mosi_length,
                     event->miso_pointer, event->miso_length);

        if (print_data)
        {
            logger.write_data(logger::level::debug,
                              event->miso_pointer,
                              event->miso_length,
                              false, io::data_prefix::address);
        }

        if (spim_transfer_count > 1u)
        {
            ASSERT(spis_miso_buffer.enqueued() == spim_miso_buffer);
        }
        break;

    default:
        ASSERT(0);
        break;
    }
}

static void spim_start_transfer()
{
    led_state_set(2u, true);

    logger &logger = logger::instance();
    logger.debug("SPIM: start");

    enum spi_result_t spim_result = spim_transfer(
        spim_port,
        spim_mosi_buffer.data(), spim_mosi_buffer.size(),
        spim_miso_buffer.data(), spim_miso_buffer.size(),
        spim_event_handler,
        nullptr,
        0u);

    ASSERT(spim_result == spi_result_success);
}

static void spis_event_handler(struct spi_event_t const *event, void* context)
{
    logger &logger = logger::instance();
    led_state_set(1u, false);

    switch (event->type)
    {
    case spi_event_data_ready:
        // The SPI slave is ready to queue another buffer.
        logger.debug("SPIS [%u] ready: "
                     "mosi:(0x%p, %04x), miso:(0x%p, %04x)",
                     spim_transfer_count,
                     event->mosi_pointer, event->mosi_length,
                     event->miso_pointer, event->miso_length);

        spis_enable_transfer(spis_port,
                             spis_miso_buffer.to_queue().data(),
                             spis_miso_buffer.to_queue().size(),
                             spis_mosi_buffer.to_queue().data(),
                             spis_mosi_buffer.to_queue().size());

        // The SPIS will get prepared when transfer complete event gets hit
        // below. It can happen that the semaphore gets transfered back to the
        // SPIS driver before the transfer complete notification is sent.
        break;

    case spi_event_transfer_complete:
        {
            spis_transfer_count += 1u;
            logger.debug("SPIS [%u] xfer:   "
                         "mosi:(0x%p, %04x), miso:(0x%p, %04x)",
                         spim_transfer_count,
                         event->mosi_pointer, event->mosi_length,
                         event->miso_pointer, event->miso_length);

            if (print_data)
            {
                logger.write_data(logger::level::debug,
                                  event->mosi_pointer,
                                  event->mosi_length,
                                  false, io::data_prefix::address);
            }

            // Check that the received data is what we expect.
            ASSERT(spis_mosi_buffer.enqueued() == spim_mosi_buffer);
            ASSERT(spis_mosi_buffer.enqueued().size() == event->mosi_length);

            // Copy what was recieved by the SPIS on the mosi line into
            // the next SPIS miso transaction.
            std::copy(spis_mosi_buffer.enqueued().begin(),
                      spis_mosi_buffer.enqueued().begin() + event->mosi_length,
                      spis_miso_buffer.to_queue().begin());

            spis_mosi_buffer.index_enqueued =
                spis_mosi_buffer.index_increment(spis_mosi_buffer.index_enqueued);
            spis_miso_buffer.index_enqueued =
                spis_miso_buffer.index_increment(spis_miso_buffer.index_enqueued);

            spis_mosi_buffer.index_to_queue =
                spis_mosi_buffer.index_increment(spis_mosi_buffer.index_to_queue);
            spis_miso_buffer.index_to_queue =
                spis_miso_buffer.index_increment(spis_miso_buffer.index_to_queue);

            // Enable the SPIS to receive a new transfer.
            spis_enable_transfer(spis_port,
                                 spis_miso_buffer.to_queue().data(),
                                 spis_miso_buffer.to_queue().size(),
                                 spis_mosi_buffer.to_queue().data(),
                                 spis_mosi_buffer.to_queue().size());
        }
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
    logger.debug("SPIS [%u] enable:", spis_transfer_count);

    // Fill the SPIM tx buffer with increasing ramps of data.
    mem_fill_ramp(spim_mosi_buffer.data(),
                  ramp_start_value,
                  1u,
                  spim_mosi_buffer.size());
    ramp_start_value += spim_mosi_buffer.size();

    spim_start_transfer();
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

    logger.info("----- SPIM, SPIS test -----");
    if (logger.get_level() <= logger::level::info)
    {
        logger.info("Only errors will be reported");
    }

    logger.debug("spi timer: %8u ticks", spi_timer.expiration_get_ticks());

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
                         spis_miso_buffer.to_queue().data(),
                         spis_miso_buffer.to_queue().size(),
                         spis_mosi_buffer.to_queue().data(),
                         spis_mosi_buffer.to_queue().size());

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

