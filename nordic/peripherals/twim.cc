/**
 * @file twim.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Perform transfers from the Noric device acting as a TWI master using DMA.
 */

#include "twim.h"
#include "logger.h"
#include "nrf_cmsis.h"
#include "arm_utilities.h"
#include "project_assert.h"

#include <iterator>

// Shortening the IRQ naming for readability. Ignore unused varaiable warnings.
static constexpr IRQn_Type const TWIM0_IRQn = SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQn;   // NOLINT (clang-diagnostic-unused-const-variable)
static constexpr IRQn_Type const TWIM1_IRQn = SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQn;   // NOLINT (clang-diagnostic-unused-const-variable)

/// The default initialization value; which is invalid.
/// To use the TWI, all pins must be set to a valid device pin.
static constexpr gpio_pin_t const twi_pin_uninitialized = -1;

/**
 * @struct twim_control_block_t
 * Maintain the state of the TWI master device using DMA.
 *
 * @note The register set for the TWI Masters:
 * + NRF_TWIM_Type: Uses DMA
 * + NRF_TWI_Type : Uses byte-by-byte interrupts
 * are significatly different.
 * This block is specific to NRF_TWIM_Type, using DMA.
 */
struct twim_control_block_t
{
    ~twim_control_block_t()                                         = default;

    twim_control_block_t()                                          = delete;
    twim_control_block_t(twim_control_block_t const&)               = delete;
    twim_control_block_t(twim_control_block_t &&)                   = delete;
    twim_control_block_t& operator=(twim_control_block_t const&)    = delete;
    twim_control_block_t& operator=(twim_control_block_t&&)         = delete;

    twim_control_block_t(uintptr_t twi_base_addr, IRQn_Type irq):
        twim_registers(reinterpret_cast<NRF_TWIM_Type *>(twi_base_addr)),
        irq_type(irq),
        handler(nullptr),
        context(nullptr),
        rx_busy(false),
        tx_busy(false),
        pin_scl(twi_pin_uninitialized),
        pin_sda(twi_pin_uninitialized)
    {
    }

    /**
     * Pointer to the structure with TWIM peripheral instance registers.
     * This must be one of:
     * {NRF_TWIM0_BASE, NRF_TWIM1_BASE}
     * @note The instance of the registers must match the ISR found in the
     * vector table for this set of registers.
     * @example NRF_TWIM0_BASE must associate with
     *          irq_type SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQn whose ISR
     *          is SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQHandler().
     */
    NRF_TWIM_Type* const twim_registers;

    /**
     * TWI/TWIM peripheral instance IRQ number.
     * This will be one of
     * - SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQn,
     * - SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQn,
     * @see association notes in .twim_registers above.
     */
    IRQn_Type const irq_type;

    /// The user supplied callback function.
    /// When the spi transfer is complete this function is called.
    twim_event_handler_t handler;

    /// The user supplied context.
    /// This is carried by the TWI interface but never modified by the TWI driver.
    void *context;

    /// Set to true when the DMA RXD.PTR, RXD.MAXCNT registeres are set.
    /// Set to false when EVENTS_RXSTARTED has been received.
    volatile bool rx_busy;

    /// Set to true when the DMA TXD.PTR, TXD.MAXCNT registeres are set.
    /// Set to false when EVENTS_tXSTARTED has been received.
    volatile bool tx_busy;

    /** @{ The I2C clock and data registers.
     * @see OPS 1.4, 33.7 Master mode pin configuration page 310
     * The PSEL.SCL and PSEL.SDA registers and their configurations are only
     * used as long as the TWI master is enabled, and retained only as long as
     * the device is in ON mode.
     *
     * When the peripheral is disabled, the pins will behave as regular GPIOs,
     * and use the configuration in their respective OUT bit field and
     * PIN_CNF[n] register.
     *
     * PSEL.SCL, PSEL.SDA must only be configured when the TWI master is disabled.
     * To secure correct signal levels on the pins used by the TWI master when
     * the system is in OFF mode, and when the TWI master is disabled,
     * these pins must be configured in the GPIO peripheral as described
     * in Table 74: GPIO configuration before enabling peripheral on page 310.
     */
    gpio_pin_t pin_scl;
    gpio_pin_t pin_sda;
    /// @}
};

static void irq_handler_twim(struct twim_control_block_t* twim_control);

#if defined TWIM0_ENABLED
static struct twim_control_block_t twim_instance_0(NRF_TWIM0_BASE, TWIM0_IRQn);
static constexpr struct twim_control_block_t* const twim_instance_ptr_0 =
    &twim_instance_0;

extern "C" void SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQHandler(void)
{
    irq_handler_twim(&twim_instance_0);
}
#else
static struct twim_control_block_t* const twim_instance_ptr_0 = nullptr;
#endif  // TWIM0_ENABLED

#if defined TWIM1_ENABLED
static struct twim_control_block_t twim_instance_1(NRF_TWIM1_BASE, TWIM1_IRQn);
static constexpr struct twim_control_block_t* const twim_instance_ptr_1 =
    &twim_instance_1;

extern "C" void SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQHandler(void)
{
    irq_handler_twim(&twim_instance_1);
}
#else
static struct twim_control_block_t* const twim_instance_ptr_1 = nullptr;
#endif  // TWIM1_ENABLED

static struct twim_control_block_t* const twim_instances[] =
{
    twim_instance_ptr_0,
    twim_instance_ptr_1,
};

/**
 * @return bool true  if the TWIM.ENABLE register set properly for the TWIM operation.
 *              false if the TWIM.ENABLE register is set for any other mode of
 *              operation or if it is disabled.
 */
static bool twim_is_initialized(struct twim_control_block_t const *twim_control)
{
    return twim_control->twim_registers->ENABLE ==
        (TWIM_ENABLE_ENABLE_Enabled << TWIM_ENABLE_ENABLE_Pos);
}

/**
 * @return bool true if the TWIM.ENABLE register is in use by any module
 *              {SPIM, SPIS, TWIM}.
 *              false if the TWIM.ENABLE register is not in use.
 */
static bool twim_regs_in_use(struct twim_control_block_t const *twim_control)
{
    return bool(twim_control->twim_registers->ENABLE & TWIM_ENABLE_ENABLE_Msk);
}

static struct twim_control_block_t* const twim_control_block(twi_port_t twi_port)
{
    if (twi_port < std::size(twim_instances))
    {
        return twim_instances[twi_port];
    }
    else
    {
        return nullptr;
    }
}

/**
 * See nRF52 Section 15.8.1 Peripheral Interface, Interrupt clearing
 * Clearing an interrupt may take 4 cycles. Read the register to insure the
 * interrupt is cleared before exiting the ISR.
 */
static void twim_clear_event_register(uint32_t volatile* twim_register)
{
    *twim_register = 0u;
    volatile uint32_t dummy = *twim_register;
    (void) dummy;
}

enum twi_result_t twim_init(twi_port_t                  twi_port,
                            struct twim_config_t const* twim_config)
{
    struct twim_control_block_t* const twim_control = twim_control_block(twi_port);
    ASSERT(twim_control);
    ASSERT(not twim_regs_in_use(twim_control));

    ASSERT(twim_config);
    ASSERT(interrupt_priority_is_valid(twim_config->irq_priority));

    enum twi_result_t result_code = twi_result_success;

    twim_control->handler = nullptr;
    twim_control->context = nullptr;

    twi_pin_config(&twim_config->pin_scl);
    twi_pin_config(&twim_config->pin_sda);

    twim_control->twim_registers->PSEL.SCL  = twim_config->pin_scl.pin_no;
    twim_control->twim_registers->PSEL.SDA  = twim_config->pin_sda.pin_no;
    twim_control->twim_registers->FREQUENCY = twim_config->clock_freq;
    twim_control->twim_registers->ENABLE    = (TWIM_ENABLE_ENABLE_Enabled << TWIM_ENABLE_ENABLE_Pos);
    twim_control->twim_registers->INTENSET  = 0u;

    NVIC_SetPriority(twim_control->irq_type, twim_config->irq_priority);
    NVIC_ClearPendingIRQ(twim_control->irq_type);

    twim_control->rx_busy = false;
    twim_control->tx_busy = false;

    return result_code;
}

void twim_deinit(twi_port_t twi_port)
{
    struct twim_control_block_t* const twim_control = twim_control_block(twi_port);
    ASSERT(twim_control);
    ASSERT(twim_is_initialized(twim_control));

    twim_abort_transfer(twi_port);
    twim_control->twim_registers->ENABLE = (TWIM_ENABLE_ENABLE_Disabled << TWIM_ENABLE_ENABLE_Pos);
}

static void twim_events_clear_all(struct twim_control_block_t* const twim_control)
{
    twim_clear_event_register(&twim_control->twim_registers->EVENTS_STOPPED);
    twim_clear_event_register(&twim_control->twim_registers->EVENTS_ERROR);
    twim_clear_event_register(&twim_control->twim_registers->EVENTS_SUSPENDED);
    twim_clear_event_register(&twim_control->twim_registers->EVENTS_RXSTARTED);
    twim_clear_event_register(&twim_control->twim_registers->EVENTS_TXSTARTED);
    twim_clear_event_register(&twim_control->twim_registers->EVENTS_LASTRX);
    twim_clear_event_register(&twim_control->twim_registers->EVENTS_LASTTX);
}

/**
 * TWIM Shortcuts:
 * TWIM_SHORTS_LASTRX_STOP_Msk      LASTRX event and STOP task
 * TWIM_SHORTS_LASTRX_STARTTX_Msk   LASTRX event and STARTTX task
 * TWIM_SHORTS_LASTTX_STOP_Msk      LASTTX event and STOP task
 * TWIM_SHORTS_LASTTX_SUSPEND_Msk   LASTTX event and SUSPEND task
 * TWIM_SHORTS_LASTTX_STARTRX_Msk   LASTTX event and STARTRX task
 *
 * TWIN Interrupts:
 * TWIM_INTEN_LASTTX_Msk        TWIM_INTENSET_LASTTX_Msk        TWIM_INTENCLR_LASTTX_Msk
 * TWIM_INTEN_LASTRX_Msk        TWIM_INTENSET_LASTRX_Msk        TWIM_INTENCLR_LASTRX_Msk
 * TWIM_INTEN_TXSTARTED_Msk     TWIM_INTENSET_TXSTARTED_Msk     TWIM_INTENCLR_TXSTARTED_Msk
 * TWIM_INTEN_RXSTARTED_Msk     TWIM_INTENSET_RXSTARTED_Msk     TWIM_INTENCLR_RXSTARTED_Msk
 * TWIM_INTEN_SUSPENDED_Msk     TWIM_INTENSET_SUSPENDED_Msk     TWIM_INTENCLR_SUSPENDED_Msk
 * TWIM_INTEN_ERROR_Msk         TWIM_INTENSET_ERROR_Msk         TWIM_INTENCLR_ERROR_Msk
 * TWIM_INTEN_STOPPED_Msk       TWIM_INTENSET_STOPPED_Msk       TWIM_INTENCLR_STOPPED_Msk
 *
 * OPS 1.4, 33.2 EasyDMA
 * The .PTR and .MAXCNT registers are double-buffered.
 * They can be updated and prepared for the next RX/ TX transmission immediately
 * after having received the RXSTARTED/TXSTARTED event.
 *
 * The STOPPED event indicates that EasyDMA has finished accessing the buffer in RAM.
 */

/**
 * @see OPS 1.4 33.3 Master write sequence
 * A TWI master write sequence is started by triggering the STARTTX task.
 *
 * After the STARTTX task has been triggered, the TWI master will generate
 * a start condition on the TWI bus, followed by clocking out the address
 * and the READ/WRITE bit set to 0 (WRITE=0, READ=1).
 *
 * The Slave ACKs or NACKs the address. The slave NACK-ing the address will
 * result in the error 'ANACK' and indicates a rejection of the write by the
 * slave. The master should respond to a NACK with a STOP.
 *
 * After receiving the ACK bit, the TWI master will clock out the data bytes
 * found in the transmit buffer TXD.PTR register.
 *
 * The slave must ACK/NACK each byte. If the slave NACKs a data byte the error
 * DNACK is is generated. The master must terminate transmission on NACK.
 *
 * On clock stretching by the I2C slave the SUSPENDED event is triggered.
 *
 * The TWI master is stopped by triggering the STOP task,
 * this task should be triggered during the transmission of the last byte to
 * so that the TWI will stop after sending the last byte.
 * Use the shortcut between LASTTX and STOP to accomplish this.
 */
enum twi_result_t twim_write(twi_port_t             twi_port,
                             twi_addr_t             address,
                             void const*            tx_buffer,
                             dma_size_t             tx_length,
                             twim_event_handler_t   handler,
                             void*                  context)
{
    struct twim_control_block_t* const twim_control = twim_control_block(twi_port);

    ASSERT(twim_control);
    ASSERT(twim_is_initialized(twim_control));

    ASSERT(tx_buffer != nullptr);
    ASSERT(is_valid_ram(tx_buffer, tx_length));
    ASSERT(handler != nullptr);                 // Polling mode not supported.

    enum twi_result_t result = twi_result_success;

    if (twim_control->tx_busy)
    {
        result = twi_result_tx_busy;
        logger &logger = logger.instance();
        logger.error("%s, error: %d", __func__, result);
        return result;
    }

    twim_control->tx_busy = true;
    twim_control->handler = handler;
    twim_control->context = context;

    // Disable all TWIM interrupts
    twim_control->twim_registers->INTEN = 0u;

    twim_events_clear_all(twim_control);

    twim_control->twim_registers->ADDRESS    = (address >> 1u);
    twim_control->twim_registers->TXD.PTR    = reinterpret_cast<uintptr_t>(tx_buffer);
    twim_control->twim_registers->TXD.MAXCNT = tx_length;
    twim_control->twim_registers->SHORTS     = TWIM_SHORTS_LASTTX_STOP_Msk;
    twim_control->twim_registers->INTENSET   = TWIM_INTENSET_ERROR_Msk     |
                                               TWIM_INTENSET_TXSTARTED_Msk |
                                               TWIM_INTENSET_LASTTX_Msk    |
                                               TWIM_INTENSET_SUSPENDED_Msk |
                                               TWIM_INTENSET_STOPPED_Msk   ;

    NVIC_ClearPendingIRQ(twim_control->irq_type);
    NVIC_EnableIRQ(twim_control->irq_type);

    twim_control->twim_registers->TASKS_STARTTX = 1u;

    return result;
}

/**
 * @see OPS 1.4 33.4 Master read sequence
 * A TWI master read sequence is started by triggering the STARTRX task.
 * After the STARTRX task has been triggered the TWI master will generate a
 * start condition on the TWI bus, followed by clocking out the address and
 * the READ/WRITE bit set to 1 (WRITE = 0, READ = 1).
 *
 * The READ/WRITE bit is followed by an ACK/NACK bit (ACK=0 or NACK = 1)
 * generated by the slave. After having sent the ACK bit the TWI slave will send
 * data to the master using the clock generated by the master.
 *
 * Data received will be stored in RAM at the address specified in the RXD.PTR.
 * The TWI master will generate an ACK after all but the last byte received from
 * the slave.
 * The TWI master will generate a NACK after the last byte received to indicate
 * that the read sequence shall stop.
 *
 * Setting the suspend task will result in clock stretching by the master when
 * receiving data from the slave. A SUSPENDED event indicates that the SUSPEND
 * task has taken effect.
 *
 * The TWI master will generate a LASTRX event when it is ready to receive the
 * last byte.
 * If RXD.MAXCNT > 1 the LASTRX event is generated after sending the ACK of the
 * previously received byte.
 * If RXD.MAXCNT = 1 the LASTRX event is generated after receiving the ACK
 * following the address and READ bit.
 *
 * The TWI master is stopped by triggering the STOP task,
 * The STOP task must be set before the NACK bit is supposed to be transmitted.
 * The STOP task can be triggered at during the reception of the last byte.
 * Use the shortcut between LASTRX and STOP to accomplish this.
 */
enum twi_result_t twim_read(twi_port_t              twi_port,
                            twi_addr_t              address,
                            void*                   rx_buffer,
                            dma_size_t              rx_length,
                            twim_event_handler_t    handler,
                            void*                   context)
{
    struct twim_control_block_t* const twim_control = twim_control_block(twi_port);

    ASSERT(twim_control);
    ASSERT(twim_is_initialized(twim_control));

    ASSERT(rx_buffer != nullptr);
    ASSERT(is_valid_ram(rx_buffer, rx_length));
    ASSERT(handler != nullptr);                 // Polling mode not supported.

    enum twi_result_t result = twi_result_success;

    if (twim_control->rx_busy)
    {
        result = twi_result_rx_busy;
        logger &logger = logger.instance();
        logger.error("%s, error: %d", __func__, result);
        return result;
    }

    twim_control->rx_busy = true;
    twim_control->handler = handler;
    twim_control->context = context;

    // Disable all TWIM interrupts
    twim_control->twim_registers->INTEN = 0u;

    twim_events_clear_all(twim_control);

    twim_control->twim_registers->RXD.PTR    = reinterpret_cast<uintptr_t>(rx_buffer);
    twim_control->twim_registers->RXD.MAXCNT = rx_length;

    twim_control->twim_registers->INTENSET = TWIM_INTENSET_ERROR_Msk     |
                                             TWIM_INTENSET_RXSTARTED_Msk |
                                             TWIM_INTENSET_LASTRX_Msk    |
                                             TWIM_INTENSET_SUSPENDED_Msk |
                                             TWIM_INTENSET_STOPPED_Msk   ;

    twim_control->twim_registers->SHORTS  = TWIM_SHORTS_LASTRX_STOP_Msk;
    twim_control->twim_registers->ADDRESS = (address >> 1u);

    NVIC_ClearPendingIRQ(twim_control->irq_type);
    NVIC_EnableIRQ(twim_control->irq_type);

    twim_control->twim_registers->TASKS_STARTRX = 1u;

    return result;
}

/** @todo Untested. Needs work. */
void twim_abort_transfer(twi_port_t twi_port)
{
    ASSERT(not interrupt_context_check());

    struct twim_control_block_t* const twim_control = twim_control_block(twi_port);
    ASSERT(twim_control);

    NVIC_DisableIRQ(twim_control->irq_type);

    uint32_t const disable_all = UINT32_MAX;
    twim_control->twim_registers->INTENCLR = disable_all;

    if (twim_control->rx_busy)
    {
        /// @todo need to handle the case when TWIM is suspended.
        twim_control->twim_registers->TASKS_STOP = 1u;

        while (not twim_control->twim_registers->EVENTS_STOPPED)
        {
            // Block while pending TWI transactions complete.
        }
        twim_control->rx_busy = false;
    }

    if (twim_control->tx_busy)
    {
        /// @todo need to handle the case when TWIM is suspended.
        twim_control->twim_registers->TASKS_STOP = 1u;

        while (not twim_control->twim_registers->EVENTS_STOPPED)
        {
            // Block while pending TWI transactions complete.
        }
        twim_control->tx_busy = false;
    }

    twim_events_clear_all(twim_control);
}

static void irq_handler_twim(struct twim_control_block_t* twim_control)
{
    logger &logger = logger.instance();
    logger.debug("+++ %s", __func__);

    struct twim_event_t event = {
        .type = twi_event_none,
        .xfer = {
            .tx_bytes = 0u,
            .rx_bytes = 0u
        }
    };

    if (twim_control->twim_registers->EVENTS_RXSTARTED)
    {
        twim_control->rx_busy = false;
        event.type |= twi_event_rx_started;
        twim_clear_event_register(&twim_control->twim_registers->EVENTS_RXSTARTED);
    }

    if (twim_control->twim_registers->EVENTS_TXSTARTED)
    {
        twim_control->tx_busy = false;
        event.type |= twi_event_tx_started;
        twim_clear_event_register(&twim_control->twim_registers->EVENTS_TXSTARTED);
    }

    if (twim_control->twim_registers->EVENTS_LASTRX)
    {
        twim_clear_event_register(&twim_control->twim_registers->EVENTS_LASTRX);
    }

    if (twim_control->twim_registers->EVENTS_LASTTX)
    {
        twim_clear_event_register(&twim_control->twim_registers->EVENTS_LASTTX);
    }

    if (twim_control->twim_registers->EVENTS_STOPPED)
    {
        twim_control->rx_busy = false;
        twim_control->tx_busy = false;

        event.type         |= twi_event_stopped;
        event.xfer.tx_bytes = twim_control->twim_registers->TXD.AMOUNT;
        event.xfer.rx_bytes = twim_control->twim_registers->RXD.AMOUNT;
        twim_clear_event_register(&twim_control->twim_registers->EVENTS_STOPPED);
    }

    if (twim_control->twim_registers->EVENTS_SUSPENDED)
    {
        event.type         |= twim_event_suspended;
        event.xfer.tx_bytes = twim_control->twim_registers->TXD.AMOUNT;
        event.xfer.rx_bytes = twim_control->twim_registers->RXD.AMOUNT;
        twim_clear_event_register(&twim_control->twim_registers->EVENTS_SUSPENDED);
    }

    if (twim_control->twim_registers->EVENTS_ERROR)
    {
        twim_control->rx_busy = false;
        twim_control->tx_busy = false;

        uint32_t const error_source = twim_control->twim_registers->ERRORSRC;

        event.type |= (error_source & TWI_ERRORSRC_ANACK_Msk) ?
            twim_event_addr_nack : 0u;
        event.type |= (error_source & TWI_ERRORSRC_DNACK_Msk) ?
            twi_event_data_nack : 0u;
        event.type |= (error_source & TWI_ERRORSRC_OVERRUN_Msk) ?
            twi_event_rx_overrun : 0u;

        event.xfer.tx_bytes = twim_control->twim_registers->TXD.AMOUNT;
        event.xfer.rx_bytes = twim_control->twim_registers->RXD.AMOUNT;

        twim_clear_event_register(&twim_control->twim_registers->EVENTS_ERROR);
    }

    // Notify the client of triggered events.
    if (event.type != 0u)
    {
        twim_control->handler(&event, twim_control->context);
    }
    logger.debug("--- %s: 0x%04x", __func__, event.type);
}

