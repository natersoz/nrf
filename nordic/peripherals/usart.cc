/**
 * @file usart.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "usart.h"
#include "logger.h"
#include "fixed_allocator.h"
#include "nrf_cmsis.h"
#include "nordic_critical_section.h"
#include "arm_utilities.h"
#include "project_assert.h"

#include <algorithm>
#include <iterator>
#include <limits>
#include <exception>
#include <boost/circular_buffer.hpp>

static constexpr bool const DEBUG_RX = true;

#if defined (NRF52840_XXAA)
static constexpr size_t const max_dma_length = std::numeric_limits<uint16_t>::max();
#else
static constexpr size_t const max_dma_length = std::numeric_limits<uint8_t>::max();
#endif

static constexpr uint32_t const hfclk = 16'000'000u;        // HFCLK in Hz.

using usart_allocator = fixed_allocator<uint8_t>;
using usart_buffer    = boost::circular_buffer<uint8_t, usart_allocator>;

/**
 * Convert from usart_buffer::array_range (first, second)
 * to DMA (PTR, MAXCNT) values.
 * limit the length to the max DMA transfer size.
 */
struct dma_range
{
    ~dma_range()                            = default;
    dma_range()                             = delete;
    dma_range(dma_range const&)             = delete;
    dma_range(dma_range &&)                 = delete;
    dma_range& operator=(dma_range const&)  = delete;
    dma_range& operator=(dma_range&&)       = delete;

    dma_range(usart_buffer::array_range const& range):
        ptr(reinterpret_cast<uint32_t>(range.first)),
        length(std::min(range.second, max_dma_length))
    {
    }

    uint32_t ptr;
    uint32_t length;
};

struct dma_buffer
{
    ~dma_buffer()                             = default;
    dma_buffer(dma_buffer const&)             = delete;
    dma_buffer(dma_buffer &&)                 = delete;
    dma_buffer& operator=(dma_buffer const&)  = delete;
    dma_buffer& operator=(dma_buffer&&)       = delete;

    dma_buffer(): ptr(nullptr), length(0u) {}

    uint8_t*    ptr;
    size_t      length;

    uintptr_t address() const { return reinterpret_cast<uintptr_t>(this->ptr); }
};

static constexpr uint8_t const rx_dma_buffer_count = 2u;

// Double buffering is not used for Tx. It might present a small increase
// in throughput but the complexity is daunting.
static constexpr uint32_t const usart_tx_interrupt_mask =
        UARTE_INTENSET_ENDTX_Msk        |
        UARTE_INTENSET_TXSTOPPED_Msk    |
//      UARTE_INTENSET_TXSTARTED_Msk    |
//      UARTE_INTENSET_TXDRDY_Msk       |
        UARTE_INTENSET_NCTS_Msk         |   // Debug only.
        UARTE_INTENSET_CTS_Msk          |   // Debug only.
        0u;

// Since the USART errors are all related to read operations the ERROR mask
// is included in the Rx interrupts for enablement.
static constexpr uint32_t const usart_rx_interrupt_mask =
        UARTE_INTENSET_RXSTARTED_Msk    |
        UARTE_INTENSET_RXTO_Msk         |
        UARTE_INTENSET_RXDRDY_Msk       |
        UARTE_INTENSET_ENDRX_Msk        |
        UARTE_INTENSET_ERROR_Msk        |
        0u;

/**
 * @struct usart_control_block_t
 * Maintain the state of the USART master device using DMA.
 *
 * @note The register set for the USART Masters:
 * + NRF_UARTE_Type: Uses DMA
 * + NRF_UART_Type : Uses byte-by-byte interrupts
 * are significatly different.
 * This block is specific to NRF_UARTE_Type, using DMA.
 */
struct usart_control_block_t
{
    ~usart_control_block_t()                                        = default;
    usart_control_block_t()                                         = delete;
    usart_control_block_t(usart_control_block_t const&)             = delete;
    usart_control_block_t(usart_control_block_t &&)                 = delete;
    usart_control_block_t& operator=(usart_control_block_t const&)  = delete;
    usart_control_block_t& operator=(usart_control_block_t&&)       = delete;

    usart_control_block_t(uint32_t  base_address,
                          IRQn_Type irq_no)
    :   usart_registers(reinterpret_cast<NRF_UARTE_Type *>(base_address)),
        irq_type(irq_no),
        handler(nullptr),
        context(nullptr),
        tx_dma_in_progress(false),
        rx_allocator(),
        tx_allocator(),
        rx_buffer(rx_allocator),
        tx_buffer(tx_allocator),
        rx_bytes_ready(0u),
        rx_dma_index(0u),
        rx_dma_state(0u)
    {
    }

    /**
     * Pointer to the structure with USART/USARTM peripheral instance registers.
     * This must be one of: {NRF_UARTE0_BASE, NRF_UARTE1_BASE}
     * Only nRF5240 has the UART1/UARTE1 peripheral
     *
     * @note The instance of the registers must match the ISR found in the
     * vector table for this set of registers.
     * @example NRF_UARTE0_BASE must associate with irq_type UARTE0_UART0_IRQn
     *          whose ISR is UARTE0_UART0_IRQHandler().
     */
    NRF_UARTE_Type* const usart_registers;

    /**
     * USART/USARTM peripheral instance IRQ number.
     * This will be one of {UARTE0_UART0_IRQn, UARTE1_IRQn}
     * @see association notes in .usart_registers above.
     */
    IRQn_Type const irq_type;

    /// The user supplied callback function.
    /// When the spi transfer is complete this function is called.
    usart_event_handler_t handler;

    /// The user supplied context.
    /// This is carried by the USART interface but never modified by the USART driver.
    void const* context;

    /// Set to true when the DMA TXD.PTR, TXD.MAXCNT registeres are set.
    /// Set to false when EVENTS_tXSTARTED has been received.
    volatile bool tx_dma_in_progress;

    usart_allocator rx_allocator;
    usart_allocator tx_allocator;

    usart_buffer rx_buffer;
    usart_buffer tx_buffer;

    /// Increment each time an EVENTS_RXDRDY is received.
    /// Clear when the Rx DMA FIFO is cleared.
    /// Tracks whether there is Rx data available for reading.
    size_t rx_bytes_ready;

    dma_buffer rx_dma_buffer[rx_dma_buffer_count];

    /// The buffer index points to the rx_dma_buffer[] node which will set as
    /// the next Rx DMA target. @see usart_queue_rx_dma_buffer().
    /// The usage and simplicity is based on:
    /// 1. rx_dma_buffer_count is two. Any other value will not work.
    /// 2. The RXSTARTED event always happens before ENDRX.
    uint8_t rx_dma_index;

    /// State 0: No     DMA buffer queued.
    /// State 1: First  DMA buffer queued.
    /// State 2: Second DMA buffer queued.
    /// @note This is used for sanity checking. It could be removed with any
    /// conditions ignored (ignore test, do what is inside the curlies)
    /// and operation should work fine.
    uint8_t rx_dma_state;
};

static void irq_handler_usart(struct usart_control_block_t* usart_control);
static struct usart_control_block_t usart_instance_0(NRF_UARTE0_BASE, UARTE0_UART0_IRQn);
static struct usart_control_block_t* const usart_instance_ptr_0 = &usart_instance_0;
extern "C" void UARTE0_UART0_IRQHandler(void) { irq_handler_usart(&usart_instance_0); }

#if defined (NRF52840_XXAA)
static struct usart_control_block_t usart_instance_1(NRF_UARTE1_BASE, UARTE1_IRQn);
static struct usart_control_block_t* const usart_instance_ptr_1 = &usart_instance_1;
extern "C" void UARTE1_IRQHandler(void) { irq_handler_usart(&usart_instance_1); }
#else
static struct usart_control_block_t* const usart_instance_ptr_1 = nullptr;
#endif  // NRF52840_XXAA

static struct usart_control_block_t* const usart_instances[] =
{
    usart_instance_ptr_0,
    usart_instance_ptr_1,
};

/**
 * @return bool true  if the USARTE.ENABLE register set properly for the USARTE operation.
 *              false if the USARTE.ENABLE register is set for any other mode of
 *              operation or if it is disabled.
 */
static bool usart_is_initialized(struct usart_control_block_t const *usart_control)
{
    return usart_control->usart_registers->ENABLE ==
        (UARTE_ENABLE_ENABLE_Enabled << UARTE_ENABLE_ENABLE_Pos);
}

/**
 * @return bool true if the USARTE.ENABLE register is in use by any module
 *              {USARTE, USARTS, TWIM}.
 *              false if the USARTE.ENABLE register is not in use.
 */
static bool usart_regs_in_use(struct usart_control_block_t const *usart_control)
{
    return bool(usart_control->usart_registers->ENABLE & UARTE_ENABLE_ENABLE_Msk);
}

static struct usart_control_block_t* const usart_control_block(usart_port_t usart_port)
{
    if (usart_port < std::size(usart_instances))
    {
        return usart_instances[usart_port];
    }
    else
    {
        return nullptr;
    }
}

static void usart_clear_event_register(uint32_t volatile* usart_event_register)
{
    *usart_event_register = 0u;
    volatile uint32_t dummy = *usart_event_register;
    (void) dummy;
}

static void usart_wait_for_event_register(uint32_t volatile* usart_event_register)
{
    while (not usart_event_register)
    {
    }
    usart_clear_event_register(usart_event_register);
}

// Set the pin as an output, with the initial value high.
static void usart_pin_config_output(gpio_pin_t pin)
{
    gpio_pin_set(pin);
    gpio_configure_output(pin, gpio_pull_none, gpio_drive_s1s0);
}

static void usart_pin_config_input(gpio_pin_t pin)
{
    gpio_pin_clear(pin);
    gpio_configure_input(pin, gpio_pull_none, gpio_sense_disable);
}

/* Note: The lookup table method, which is what PS 1.4 Section 35.10.11
 * says to use, is more robust with respect to Rx framing errors than the
 * calculation method. The baud rate calculation method:
 *   uint64_t baud_reg_64 = baud_rate;
 *   baud_reg_64 <<= 32u;
 *   baud_reg_64 /= hfclk;
 *   return static_cast<uint32_t>(baud_reg_64);
 * baud rate calculated value for 115200: 0x01d7'dbf5
 */
static uint32_t usart_baud_rate_to_reg(uint32_t baud_rate)
{
    switch (baud_rate)
    {
    case     1'200u:    return 0x0004'F000u;
    case     2'400u:    return 0x0009'D000u;
    case     4'800u:    return 0x0013'B000u;
    case     9'600u:    return 0x0027'5000u;
    case    14'400u:    return 0x003A'F000u;
    case    19'200u:    return 0x004E'A000u;
    case    28'800u:    return 0x0075'C000u;
    case    38'400u:    return 0x009D'0000u;
    case    57'600u:    return 0x00EB'0000u;
    case    76'800u:    return 0x013A'9000u;
    case   115'200u:    return 0x01D6'0000u;
    case   230'400u:    return 0x03B0'0000u;
    case   250'000u:    return 0x0400'0000u;
    case   460'800u:    return 0x0740'0000u;
    case   921'600u:    return 0x0F00'0000u;
    case 1'000'000u:    return 0x1000'0000u;

    default:
        logger::instance().error("invalid baud rate: %u", baud_rate);
        ASSERT(0);
        return 0u;
    }
}

static uint32_t usart_baud_rate_from_reg(uint32_t baud_reg)
{
    uint64_t baud_rate = baud_reg;
    baud_rate *= hfclk;
    baud_rate >>= 32u;
    return static_cast<uint32_t>(baud_rate);
}

static void usart_dma_queue_rx_buffer(struct usart_control_block_t* usart_control)
{
    ASSERT(usart_control->rx_dma_state < rx_dma_buffer_count);
    usart_control->rx_dma_state += 1u;

    dma_buffer& rx_dma_buffer =
        usart_control->rx_dma_buffer[usart_control->rx_dma_index];

    usart_control->usart_registers->RXD.PTR    = rx_dma_buffer.address();
    usart_control->usart_registers->RXD.MAXCNT = rx_dma_buffer.length;
    usart_control->rx_dma_index ^= 1u;
}

static size_t usart_dma_dequeue_rx_buffer(struct usart_control_block_t* usart_control)
{
    ASSERT(usart_control->rx_dma_state > 0u);

    uint8_t const* rx_ptr = usart_control->rx_dma_buffer[usart_control->rx_dma_index].ptr;
    size_t  const  rx_len = usart_control->usart_registers->RXD.AMOUNT;

    usart_control->rx_buffer.insert(usart_control->rx_buffer.end(), rx_ptr, rx_ptr + rx_len);
    usart_control->rx_dma_state -= 1u;

    return rx_len;
}

void usart_init(usart_port_t                    usart_port,
                struct usart_config_t const*    usart_config,
                usart_event_handler_t           usart_event_handler,
                void*                           tx_buffer,
                size_t                          tx_length,
                void*                           rx_buffer,
                size_t                          rx_length,
                void*                           usart_context)
{
    struct usart_control_block_t* const usart_control = usart_control_block(usart_port);
    ASSERT(usart_control);
    ASSERT(not usart_regs_in_use(usart_control));

    ASSERT(usart_config);
    ASSERT(interrupt_priority_is_valid(usart_config->irq_priority));

    ASSERT(is_valid_ram(tx_buffer, tx_length));
    ASSERT(is_valid_ram(rx_buffer, rx_length));

    usart_control->handler = usart_event_handler;
    usart_control->context = usart_context;
    usart_control->tx_dma_in_progress = false;

    usart_control->rx_allocator.assign(reinterpret_cast<uint8_t*>(rx_buffer), rx_length);
    usart_control->tx_allocator.assign(reinterpret_cast<uint8_t*>(tx_buffer), tx_length);

    usart_control->rx_buffer.get_allocator() = usart_control->rx_allocator;
    usart_control->tx_buffer.get_allocator() = usart_control->tx_allocator;

    usart_control->rx_buffer.set_capacity(rx_length);
    usart_control->tx_buffer.set_capacity(tx_length);

    if (usart_config->tx_pin != usart_pin_not_used)
    {
        usart_pin_config_output(usart_config->tx_pin);
        usart_control->usart_registers->PSEL.TXD = usart_config->tx_pin;
    }

    if (usart_config->cts_pin != usart_pin_not_used)
    {
        usart_pin_config_output(usart_config->cts_pin);
        usart_control->usart_registers->PSEL.CTS = usart_config->cts_pin;
    }

    if (usart_config->rx_pin != usart_pin_not_used)
    {
        usart_pin_config_input(usart_config->rx_pin);
        usart_control->usart_registers->PSEL.RXD = usart_config->rx_pin;
    }

    if (usart_config->rts_pin != usart_pin_not_used)
    {
        usart_pin_config_input(usart_config->rts_pin);
        usart_control->usart_registers->PSEL.RTS = usart_config->rx_pin;
    }

    // Init will start with everything disabled.
    // The user must call usart_read_start() to enable the Rx data flow.
    usart_control->usart_registers->BAUDRATE = usart_baud_rate_to_reg(usart_config->baud_rate);
    usart_control->usart_registers->ENABLE   = (UARTE_ENABLE_ENABLE_Enabled << UARTE_ENABLE_ENABLE_Pos);
    usart_control->usart_registers->SHORTS   = 0u;
    usart_control->usart_registers->INTENCLR = std::numeric_limits<uint32_t>::max();

    // Set up the Rx DMA buffers, but don't queue anything. Rx is not yet started.
    size_t rx_chunk = std::min(rx_length / 2u, max_dma_length / 2u);
    uint8_t* rx_buffer_8 = reinterpret_cast<uint8_t*>(rx_buffer);

    usart_control->rx_dma_buffer[0u].ptr       = rx_buffer_8;
    usart_control->rx_dma_buffer[0u].length    = rx_chunk;

    usart_control->rx_dma_buffer[1u].ptr       = rx_buffer_8 + rx_chunk;
    usart_control->rx_dma_buffer[1u].length    = rx_chunk;

    NVIC_SetPriority(usart_control->irq_type, usart_config->irq_priority);
    NVIC_ClearPendingIRQ(usart_control->irq_type);
    NVIC_EnableIRQ(usart_control->irq_type);
}

uint32_t usart_get_baud_rate(usart_port_t usart_port)
{
    struct usart_control_block_t* const usart_control = usart_control_block(usart_port);
    ASSERT(usart_control);
    return usart_baud_rate_from_reg(usart_control->usart_registers->BAUDRATE);
}

void usart_deinit(usart_port_t usart_port)
{
    struct usart_control_block_t* const usart_control = usart_control_block(usart_port);
    ASSERT(usart_control);
    ASSERT(usart_is_initialized(usart_control));

    usart_write_stop(usart_port);
    usart_read_stop(usart_port);

    usart_control->usart_registers->ENABLE = (UARTE_ENABLE_ENABLE_Disabled << UARTE_ENABLE_ENABLE_Pos);
}

static void usart_start_tx_dma(struct usart_control_block_t*    usart_control,
                               dma_range const&                 dma_tx)
{
    usart_control->tx_dma_in_progress             = true;
    usart_control->usart_registers->TXD.PTR       = dma_tx.ptr;
    usart_control->usart_registers->TXD.MAXCNT    = dma_tx.length;
    usart_control->usart_registers->INTENSET      = usart_tx_interrupt_mask;
    usart_control->usart_registers->TASKS_STARTTX = 1u;
}

size_t usart_write(usart_port_t usart_port, void const* tx_buffer, size_t tx_length)
{
    struct usart_control_block_t* const usart_control = usart_control_block(usart_port);
    ASSERT(tx_buffer);
    ASSERT(usart_control);
    ASSERT(usart_is_initialized(usart_control));

    if (tx_length > 0u)
    {
        nordic::auto_critical_section cs;

        tx_length = std::min(tx_length, usart_control->tx_buffer.reserve());
        uint8_t const* tx_begin = reinterpret_cast<uint8_t const*>(tx_buffer);
        uint8_t const* tx_end   = tx_begin + tx_length;
        usart_control->tx_buffer.insert(usart_control->tx_buffer.end(), tx_begin, tx_end);

        if (not usart_control->tx_dma_in_progress)
        {
            dma_range const dma_tx = usart_control->tx_buffer.array_one();
            usart_start_tx_dma(usart_control, dma_tx);
        }
    }

    return tx_length;
}

size_t usart_write_pending(usart_port_t usart_port)
{
    struct usart_control_block_t* const usart_control = usart_control_block(usart_port);
    ASSERT(usart_control);
    ASSERT(usart_is_initialized(usart_control));

    nordic::auto_critical_section cs;
    return usart_control->tx_buffer.size();
}

size_t usart_write_avail(usart_port_t usart_port)
{
    struct usart_control_block_t* const usart_control = usart_control_block(usart_port);
    ASSERT(usart_control);
    ASSERT(usart_is_initialized(usart_control));

    nordic::auto_critical_section cs;
    return usart_control->tx_buffer.reserve();
}

void usart_write_flush(usart_port_t usart_port)
{
    ASSERT(not interrupt_context_check());
    while (usart_write_pending(usart_port))
    {
    }
}

void usart_write_stop(usart_port_t usart_port)
{
    ASSERT(not interrupt_context_check());

    struct usart_control_block_t* const usart_control = usart_control_block(usart_port);
    ASSERT(usart_control);

    nordic::auto_critical_section cs;
    usart_control->usart_registers->INTENCLR = usart_tx_interrupt_mask;

    if (usart_control->tx_dma_in_progress)
    {
        usart_control->usart_registers->TASKS_STOPTX = 1u;
        usart_wait_for_event_register(&usart_control->usart_registers->EVENTS_TXSTOPPED);

        /// @todo clear events NCTS, CTS?
        usart_clear_event_register(&usart_control->usart_registers->EVENTS_ENDTX);
        usart_control->tx_dma_in_progress = false;
    }
}

size_t usart_read(usart_port_t usart_port, void* rx_buffer, size_t rx_length)
{
    struct usart_control_block_t* const usart_control = usart_control_block(usart_port);
    ASSERT(rx_buffer);
    ASSERT(usart_control);
    ASSERT(usart_is_initialized(usart_control));

    nordic::auto_critical_section cs;

    dma_range const dma_rx = usart_control->rx_buffer.array_one();
    rx_length = std::min<size_t>(rx_length, dma_rx.length);
    usart_buffer::iterator const first = usart_control->rx_buffer.begin();
    usart_buffer::iterator const last  = first + rx_length;
    std::copy(first, last, reinterpret_cast<uint8_t*>(rx_buffer));
    usart_control->rx_buffer.erase(first, last);

    return rx_length;
}

size_t usart_read_pending(usart_port_t usart_port)
{
    struct usart_control_block_t* const usart_control = usart_control_block(usart_port);
    ASSERT(usart_control);
    ASSERT(usart_is_initialized(usart_control));

    nordic::auto_critical_section cs;
    return usart_control->rx_buffer.size() + usart_control->rx_bytes_ready;
}

size_t usart_read_avail(usart_port_t usart_port)
{
    struct usart_control_block_t* const usart_control = usart_control_block(usart_port);
    ASSERT(usart_control);
    ASSERT(usart_is_initialized(usart_control));

    nordic::auto_critical_section cs;
    return usart_control->rx_buffer.reserve();
}

void usart_read_fill(usart_port_t usart_port)
{
    struct usart_control_block_t* const usart_control = usart_control_block(usart_port);
    ASSERT(usart_control);
    ASSERT(usart_is_initialized(usart_control));

    nordic::auto_critical_section cs;
    if (usart_control->rx_dma_state > 0u)
    {
        // Setting STOPRX will cause an ENDRX event to occur and the data.
        // Leave the SHORTS set to UARTE_SHORTS_ENDRX_STARTRX_Msk so that Rx
        // processing continues.
        usart_control->usart_registers->TASKS_STOPRX = 1u;
    }
}

void usart_read_start(usart_port_t usart_port)
{
    struct usart_control_block_t* const usart_control = usart_control_block(usart_port);
    ASSERT(usart_control);
    ASSERT(usart_is_initialized(usart_control));

    nordic::auto_critical_section cs;
    ASSERT(usart_control->rx_dma_state == 0u);

    // Enable the SHORT ENDRX_STARTRX so that when the RX DMA gets filled,
    // The UARTE DMA keeps running. This will minimize the chance for dropped
    // data. The RX DMA double buffer is used to keep the Rx DMA fed.
    // Note: UARTE_SHORTS_ENDRX_STOPRX_Msk is not used.
    usart_control->rx_dma_index              = 0u;
    usart_control->usart_registers->SHORTS   = UARTE_SHORTS_ENDRX_STARTRX_Msk;
    usart_control->usart_registers->INTENSET = usart_rx_interrupt_mask;

    usart_dma_queue_rx_buffer(usart_control);
    usart_control->usart_registers->TASKS_STARTRX = 1u;
}

void usart_read_stop(usart_port_t usart_port)
{
    ASSERT(not interrupt_context_check());

    struct usart_control_block_t* const usart_control = usart_control_block(usart_port);
    ASSERT(usart_control);

    nordic::auto_critical_section cs;
    usart_control->usart_registers->INTENCLR = usart_rx_interrupt_mask;
    usart_control->usart_registers->SHORTS   = 0u;

    // Ignore any errors, we are shutting down.
    usart_clear_event_register(&usart_control->usart_registers->EVENTS_ERROR);
    usart_clear_event_register(&usart_control->usart_registers->EVENTS_TXDRDY);

    if (usart_control->rx_dma_state > 0u)
    {
        /* See OPS 1.4, Section 35.3 Transmission
         * Figure 96: UARTE reception with forced stop via STOPRX on page 336.
         * Initiate the sequence:
         *     event: ENDRX
         *     event: RXTO
         *     task:  FLUSHRX
         *     event: ENDRX
         * Disable the UARTE_SHORTS_ENDRX_STARTRX_Msk so that Rx does not
         * restart automatically.
         */
        usart_control->usart_registers->SHORTS = 0u;
        usart_control->usart_registers->TASKS_STOPRX = 1u;
        usart_wait_for_event_register(&usart_control->usart_registers->EVENTS_RXTO);

        usart_control->usart_registers->TASKS_FLUSHRX = 1u;
        usart_wait_for_event_register(&usart_control->usart_registers->EVENTS_ENDRX);

        usart_control->rx_dma_state = 0u;
        usart_control->rx_dma_index = 0u;
    }

    usart_control->rx_bytes_ready = 0u;
}

static void irq_handler_usart(struct usart_control_block_t* const usart_control)
{
    void* usart_context = const_cast<void*>(usart_control->context);

    // A auto critical section is not used here.
    // When the USART evetn handler is called the critical section is exited.
    // When the handler returns re-acquire the critical section.
    nordic::critical_section cs;
    cs.enter();

    // Error detected.
    if (usart_control->usart_registers->EVENTS_ERROR)
    {
        usart_clear_event_register(&usart_control->usart_registers->EVENTS_ERROR);
        uint32_t const error_source = usart_control->usart_registers->ERRORSRC;

        if (usart_control->handler)
        {
            cs.exit();

            if (error_source & UARTE_ERRORSRC_BREAK_Msk)
            {
                struct usart_event_t const usart_event = {
                    .type  = usart_rx_error_break,
                    .value = 0u
                };
                usart_control->handler(&usart_event, usart_context);
            }

            if (error_source & UARTE_ERRORSRC_FRAMING_Msk)
            {
                struct usart_event_t const usart_event = {
                    .type  = usart_rx_error_framing,
                    .value = 0u
                };
                usart_control->handler(&usart_event, usart_context);
            }

            if (error_source & UARTE_ERRORSRC_PARITY_Msk)
            {
                struct usart_event_t const usart_event = {
                    .type  = usart_rx_error_parity,
                    .value = 0u
                };
                usart_control->handler(&usart_event, usart_context);
            }

            if (error_source & UARTE_ERRORSRC_OVERRUN_Msk)
            {
                struct usart_event_t const usart_event = {
                    .type  = usart_rx_error_overrun,
                    .value = 0u
                };
                usart_control->handler(&usart_event, usart_context);
            }

            cs.enter();
        }

    }

    // Clear To Send: CTS is asserted (CTS pin low).
    // If flow control is enabled, a transmission will be automatically
    // suspended when CTS is deasserted and resumed when CTS is reasserted.
    // A byte that is in transmission when CTS is deasserted will be fully
    // transmitted before the transmission is suspended.
    // See OPS 1.4: Figure 94: UARTE transmission on page 334.
    if (usart_control->usart_registers->EVENTS_CTS)
    {
        usart_clear_event_register(&usart_control->usart_registers->EVENTS_CTS);
    }

    // Not Clear To Send: CTS is deasserted (CTS pin high).
    if (usart_control->usart_registers->EVENTS_NCTS)
    {
        usart_clear_event_register(&usart_control->usart_registers->EVENTS_NCTS);
    }

    // Data sent from TXD. After each byte has been sent over the TXD line,
    // a TXDRDY event is generated.
    if (usart_control->usart_registers->EVENTS_TXDRDY)
    {
        usart_clear_event_register(&usart_control->usart_registers->EVENTS_TXDRDY);
    }

    // UART transmitter has started.
    if (usart_control->usart_registers->EVENTS_TXSTARTED)
    {
        usart_clear_event_register(&usart_control->usart_registers->EVENTS_TXSTARTED);
    }

    // Transmitter stopped.
    if (usart_control->usart_registers->EVENTS_TXSTOPPED)
    {
        usart_clear_event_register(&usart_control->usart_registers->EVENTS_TXSTOPPED);
    }

    // Last TX byte transmitted.
    // The ENDTX event indicates that DMA has finished reading the Tx buffer.
    // If the ENDTX event has not already been generated when the UARTE
    // transmitter has come to a stop, the ENDTX event will trigger even though
    // all bytes in the TXD buffer, as specified in the TXD.MAXCNT register,
    // have not been transmitted.
    if (usart_control->usart_registers->EVENTS_ENDTX)
    {
        usart_clear_event_register(&usart_control->usart_registers->EVENTS_ENDTX);
        usart_control->tx_dma_in_progress = false;

        // Remove the data which was transferred by the DMA from the buffer.
        size_t const tx_length = usart_control->usart_registers->TXD.AMOUNT;
        usart_buffer::iterator const first = usart_control->tx_buffer.begin();
        usart_buffer::iterator const last  = first + tx_length;
        usart_control->tx_buffer.erase(first, last);

        // If there is more data in the Tx buffer, send it.
        dma_range const dma_tx = usart_control->tx_buffer.array_one();
        if (dma_tx.length > 0u)
        {
            usart_start_tx_dma(usart_control, dma_tx);
        }

        if (usart_control->handler)
        {
            cs.exit();
            struct usart_event_t const usart_event = {
                .type  = usart_tx_complete,
                .value = tx_length
            };
            usart_control->handler(&usart_event, usart_context);
            cs.enter();
        }
    }

    // Data received in RXD (but likely not yet transferred to RAM).
    if (usart_control->usart_registers->EVENTS_RXDRDY)
    {
        usart_clear_event_register(&usart_control->usart_registers->EVENTS_RXDRDY);
        if (usart_control->rx_bytes_ready < std::numeric_limits<uint32_t>::max())
        {
            usart_control->rx_bytes_ready += 1u;
        }
    }

    // Receive buffer is filled up.
    // The ENDRX event indicates that DMA has finished writing the Rx buffer.
    if (usart_control->usart_registers->EVENTS_ENDRX)
    {
        usart_clear_event_register(&usart_control->usart_registers->EVENTS_ENDRX);
        ASSERT(usart_control->rx_dma_state > 0u);

        usart_control->rx_bytes_ready = 0u;
        size_t const rx_len = usart_dma_dequeue_rx_buffer(usart_control);

        // Only send an Rx notification if there is data for the client to read.
        if (usart_control->handler && not usart_control->rx_buffer.empty())
        {
            cs.exit();
            struct usart_event_t const usart_event = {
                .type  = usart_rx_complete,
                .value = rx_len
            };
            usart_control->handler(&usart_event, usart_context);
            cs.enter();
        }
    }

    // Receiver timeout.
    // This event is triggered after a TASKS_STOPRX is set.
    if (usart_control->usart_registers->EVENTS_RXTO)
    {
        usart_clear_event_register(&usart_control->usart_registers->EVENTS_RXTO);
        usart_control->usart_registers->TASKS_FLUSHRX;
    }

    // UART receiver has started.
    if (usart_control->usart_registers->EVENTS_RXSTARTED)
    {
        usart_clear_event_register(&usart_control->usart_registers->EVENTS_RXSTARTED);
        if (usart_control->rx_dma_state < rx_dma_buffer_count)
        {
            usart_dma_queue_rx_buffer(usart_control);
        }
    }

    cs.exit();
}
