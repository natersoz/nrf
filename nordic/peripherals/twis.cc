/**
 * @file twis.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Perform transfers from the Noric device acting as a TWI master using DMA.
 */

#include "twis.h"
#include "gpio.h"
#include "logger.h"
#include "nrf_cmsis.h"
#include "arm_utilities.h"
#include "project_assert.h"

#include <iterator>

enum class transfer_state
{
    ready,
    rx_busy,
    tx_busy
};

/**
 * @struct twis_control_block_t
 * Maintain the state of the TWI master device using DMA.
 *
 * @note The register set for the TWI Masters:
 * + NRF_TWIS_Type: Uses DMA
 * + NRF_TWI_Type : Uses byte-by-byte interrupts
 * are significatly different.
 * This block is specific to NRF_TWIS_Type, using DMA.
 */
struct twis_control_block_t
{
    /**
     * Pointer to the structure with TWIS peripheral instance registers.
     * This must be one of:
     * {NRF_TWIS0_BASE, NRF_TWIS1_BASE}
     * @note The instance of the registers must match the ISR found in the
     * vector table for this set of registers.
     * @example NRF_TWIS0_BASE must associate with
     *          irq_type SPIM0_SPIS0_TWIS0_TWIS0_SPI0_TWI0_IRQn whose ISR
     *          is SPIM0_SPIS0_TWIS0_TWIS0_SPI0_TWI0_IRQHandler().
     */
    NRF_TWIS_Type* const twis_registers;

    /**
     * TWI/TWIS peripheral instance IRQ number.
     * This will be one of
     * - SPIM0_SPIS0_TWIS0_TWIS0_SPI0_TWI0_IRQn,
     * - SPIM1_SPIS1_TWIS1_TWIS1_SPI1_TWI1_IRQn,
     * @see association notes in .twis_registers above.
     */
    IRQn_Type const irq_type;

    /// The user supplied callback function.
    /// When the spi transfer is complete this function is called.
    twis_event_handler_t handler;

    /// The user supplied context.
    /// This is carried by the TWI interface but never modified by the TWI driver.
    void *context;

    /// Transfer in progress flag.
    /// This volatile flag allows for interrupt/task resource arbitration.
    volatile enum transfer_state transfer_state;

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

/// The default initialization value; which is invalid.
/// To use the TWI, all pins must be set to a valid device pin.
static constexpr gpio_pin_t const twi_pin_uninitialized = -1;

static void irq_handler_twis(struct twis_control_block_t* twis_control);

#if defined TWIS0_ENABLED
static struct twis_control_block_t twis_instance_0 =
{
    .twis_registers         = reinterpret_cast<NRF_TWIS_Type *>(NRF_TWIS0_BASE),
    .irq_type               = SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQn,
    .handler                = nullptr,
    .context                = nullptr,
    .transfer_state         = transfer_state::ready,
    .pin_scl                = twi_pin_not_used,
    .pin_sda                = twi_pin_not_used,
};
static struct twis_control_block_t* const twis_instance_ptr_0 = &twis_instance_0;

extern "C" void SPIM0_SPIS0_TWIS0_TWIS0_SPI0_TWI0_IRQHandler(void)
{
    irq_handler_twis(&twis_instance_0);
}
#else
static struct twis_control_block_t* const twis_instance_ptr_0 = nullptr;
#endif  // TWIS0_ENABLED

#if defined TWIS1_ENABLED
static struct twis_control_block_t twis_instance_1 =
{
    .twis_registers         = reinterpret_cast<NRF_TWIS_Type *>(NRF_TWIS1_BASE),
    .irq_type               = SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQn,
    .handler                = nullptr,
    .context                = nullptr,
    .transfer_state         = transfer_state::ready,
    .pin_scl                = twi_pin_uninitialized,
    .pin_sda                = twi_pin_uninitialized,
};
static struct twis_control_block_t* const twis_instance_ptr_1 = &twis_instance_1;

extern "C" void SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQHandler(void)
{
    irq_handler_twis(&twis_instance_1);
}
#else
static struct twis_control_block_t* const twis_instance_ptr_1 = nullptr;
#endif  // TWIS1_ENABLED

static struct twis_control_block_t* const twis_instances[] =
{
    twis_instance_ptr_0,
    twis_instance_ptr_1,
};

/**
 * @return bool true  if the TWIS.ENABLE register set properly for the TWIS operation.
 *              false if the TWIS.ENABLE register is set for any other mode of
 *              operation or if it is disabled.
 */
static bool twis_is_initialized(struct twis_control_block_t const *twis_control)
{
    return twis_control->twis_registers->ENABLE ==
        (TWIS_ENABLE_ENABLE_Enabled << TWIS_ENABLE_ENABLE_Pos);
}

/**
 * @return bool true if the TWIS.ENABLE register is in use by any module
 *              {SPIM, SPIS, TWIS}.
 *              false if the TWIS.ENABLE register is not in use.
 */
static bool twis_regs_in_use(struct twis_control_block_t const *twis_control)
{
    return bool(twis_control->twis_registers->ENABLE & TWIS_ENABLE_ENABLE_Msk);
}

static struct twis_control_block_t* const twis_control_block(twi_port_t twi_port)
{
    if (twi_port < std::size(twis_instances))
    {
        return twis_instances[twi_port];
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
static void twis_clear_event_register(uint32_t volatile* twis_register)
{
    *twis_register = 0u;
    volatile uint32_t dummy = *twis_register;
    (void) dummy;
}

enum twi_result_t twis_init(twi_port_t                  twi_port,
                            struct twis_config_t const* twis_config,
                            twis_event_handler_t        twis_handler,
                            void*                       twis_context)
{
    struct twis_control_block_t* const twis_control = twis_control_block(twi_port);
    ASSERT(twis_control);
    ASSERT(not twis_regs_in_use(twis_control));

    ASSERT(twis_config);
    ASSERT(interrupt_priority_is_valid(twis_config->irq_priority));

    enum twi_result_t result_code = twi_result_success;

    twis_control->handler = twis_handler;
    twis_control->context = twis_context;

    twi_pin_config(&twis_config->pin_scl);
    twi_pin_config(&twis_config->pin_sda);

    uint32_t addr_enabled  = 0u;
    addr_enabled |= (twis_config->slave_addr[0u] == twi_addr_invalid) ? 0u : (1u << 0u);
    addr_enabled |= (twis_config->slave_addr[1u] == twi_addr_invalid) ? 0u : (1u << 1u);

    twis_control->twis_registers->PSEL.SCL   = twis_config->pin_scl.pin_no;
    twis_control->twis_registers->PSEL.SDA   = twis_config->pin_sda.pin_no;
    twis_control->twis_registers->ADDRESS[0] = twis_config->slave_addr[0] >> 1u;
    twis_control->twis_registers->ADDRESS[1] = twis_config->slave_addr[1] >> 1u;
    twis_control->twis_registers->CONFIG     = addr_enabled;
    twis_control->twis_registers->ORC        = twis_config->orc;
    twis_control->twis_registers->ENABLE     = (TWIS_ENABLE_ENABLE_Enabled << TWIS_ENABLE_ENABLE_Pos);
    twis_control->twis_registers->INTENSET   = 0u;

    NVIC_SetPriority(twis_control->irq_type, twis_config->irq_priority);
    NVIC_ClearPendingIRQ(twis_control->irq_type);

    twis_control->transfer_state = transfer_state::ready;

    return result_code;
}

void twis_deinit(twi_port_t twi_port)
{
    struct twis_control_block_t* const twis_control = twis_control_block(twi_port);
    ASSERT(twis_control);
    ASSERT(twis_is_initialized(twis_control));

    twis_abort_transfer(twi_port);
    twis_control->twis_registers->ENABLE = (TWIS_ENABLE_ENABLE_Disabled << TWIS_ENABLE_ENABLE_Pos);
}

static void twis_events_clear_all(struct twis_control_block_t* const twis_control)
{
    twis_clear_event_register(&twis_control->twis_registers->EVENTS_STOPPED);
    twis_clear_event_register(&twis_control->twis_registers->EVENTS_ERROR);
    twis_clear_event_register(&twis_control->twis_registers->EVENTS_RXSTARTED);
    twis_clear_event_register(&twis_control->twis_registers->EVENTS_TXSTARTED);
    twis_clear_event_register(&twis_control->twis_registers->EVENTS_WRITE);
    twis_clear_event_register(&twis_control->twis_registers->EVENTS_READ);
}

/**
 * TWIS Shortcuts:
 * TWIS_SHORTS_LASTRX_STOP_Msk      LASTRX event and STOP task
 * TWIS_SHORTS_LASTRX_STARTTX_Msk   LASTRX event and STARTTX task
 * TWIS_SHORTS_LASTTX_STOP_Msk      LASTTX event and STOP task
 * TWIS_SHORTS_LASTTX_SUSPEND_Msk   LASTTX event and SUSPEND task
 * TWIS_SHORTS_LASTTX_STARTRX_Msk   LASTTX event and STARTRX task
 *
 * TWIN Interrupts:
 * TWIS_INTEN_LASTTX_Msk        TWIS_INTENSET_LASTTX_Msk        TWIS_INTENCLR_LASTTX_Msk
 * TWIS_INTEN_LASTRX_Msk        TWIS_INTENSET_LASTRX_Msk        TWIS_INTENCLR_LASTRX_Msk
 * TWIS_INTEN_TXSTARTED_Msk     TWIS_INTENSET_TXSTARTED_Msk     TWIS_INTENCLR_TXSTARTED_Msk
 * TWIS_INTEN_RXSTARTED_Msk     TWIS_INTENSET_RXSTARTED_Msk     TWIS_INTENCLR_RXSTARTED_Msk
 * TWIS_INTEN_SUSPENDED_Msk     TWIS_INTENSET_SUSPENDED_Msk     TWIS_INTENCLR_SUSPENDED_Msk
 * TWIS_INTEN_ERROR_Msk         TWIS_INTENSET_ERROR_Msk         TWIS_INTENCLR_ERROR_Msk
 * TWIS_INTEN_STOPPED_Msk       TWIS_INTENSET_STOPPED_Msk       TWIS_INTENCLR_STOPPED_Msk
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
 */


enum twi_result_t twis_enable_read(twi_port_t   twi_port,
                                   void const*  tx_buffer,
                                   dma_size_t   tx_buffer_length)
{
    struct twis_control_block_t* const twis_control = twis_control_block(twi_port);

    ASSERT(twis_control);
    ASSERT(twis_is_initialized(twis_control));

    ASSERT(tx_buffer != nullptr);
    ASSERT(is_valid_ram(tx_buffer, tx_buffer_length));

    enum twi_result_t result = twi_result_success;

    if (twis_control->transfer_state != transfer_state::ready)
    {
        result = (twis_control->transfer_state == transfer_state::rx_busy)
            ? twi_result_rx_busy : twi_result_tx_busy;
        logger &logger = logger.instance();
        logger.error("%s, error: %d", __func__, result);
        return result;
    }

    twis_control->transfer_state = transfer_state::tx_busy;

    // Disable all TWIS interrupts
    twis_control->twis_registers->INTEN = 0u;

    twis_events_clear_all(twis_control);

    twis_control->twis_registers->TXD.PTR    = reinterpret_cast<uint32_t>(tx_buffer);
    twis_control->twis_registers->TXD.MAXCNT = tx_buffer_length;

    if (twis_control->handler)
    {
        twis_control->twis_registers->INTENSET = TWIS_INTENSET_READ_Msk      |
                                                 TWIS_INTENSET_WRITE_Msk     |
                                                 TWIS_INTENSET_TXSTARTED_Msk |
                                                 TWIS_INTENSET_RXSTARTED_Msk |
                                                 TWIS_INTENSET_ERROR_Msk     |
                                                 TWIS_INTENSET_STOPPED_Msk   ;
    }

    NVIC_ClearPendingIRQ(twis_control->irq_type);
    NVIC_EnableIRQ(twis_control->irq_type);

    twis_control->twis_registers->TASKS_PREPARETX = 1u;

    return result;
}

enum twi_result_t twis_enable_write(twi_port_t  twi_port,
                                    void*       rx_buffer,
                                    dma_size_t  rx_buffer_length)
{
    struct twis_control_block_t* const twis_control = twis_control_block(twi_port);

    ASSERT(twis_control);
    ASSERT(twis_is_initialized(twis_control));

    ASSERT(rx_buffer != nullptr);
    ASSERT(is_valid_ram(rx_buffer, rx_buffer_length));

    enum twi_result_t result = twi_result_success;

    if (twis_control->transfer_state != transfer_state::ready)
    {
        result = (twis_control->transfer_state == transfer_state::rx_busy)
            ? twi_result_rx_busy : twi_result_tx_busy;
        logger &logger = logger.instance();
        logger.error("%s, error: %d", __func__, result);
        return result;
    }

    twis_control->transfer_state = transfer_state::rx_busy;

    // Disable all TWIS interrupts
    twis_control->twis_registers->INTEN = 0u;

    twis_events_clear_all(twis_control);

    twis_control->twis_registers->RXD.PTR    = reinterpret_cast<uint32_t>(rx_buffer);
    twis_control->twis_registers->RXD.MAXCNT = rx_buffer_length;

    if (twis_control->handler)
    {
        twis_control->twis_registers->INTENSET = TWIS_INTENSET_READ_Msk      |
                                                 TWIS_INTENSET_WRITE_Msk     |
                                                 TWIS_INTENSET_TXSTARTED_Msk |
                                                 TWIS_INTENSET_RXSTARTED_Msk |
                                                 TWIS_INTENSET_ERROR_Msk     |
                                                 TWIS_INTENSET_STOPPED_Msk   ;
    }

    NVIC_ClearPendingIRQ(twis_control->irq_type);
    NVIC_EnableIRQ(twis_control->irq_type);

    twis_control->twis_registers->TASKS_PREPARERX = 1u;

    return result;
}

/** @todo untested, needs work. */
void twis_abort_transfer(twi_port_t twi_port)
{
    ASSERT(not interrupt_context_check());

    struct twis_control_block_t* const twis_control = twis_control_block(twi_port);
    ASSERT(twis_control);

    NVIC_DisableIRQ(twis_control->irq_type);

    uint32_t const disable_all = UINT32_MAX;
    twis_control->twis_registers->INTENCLR = disable_all;

    if (twis_control->transfer_state != transfer_state::ready)
    {
        twis_control->twis_registers->TASKS_STOP = 1u;

        while (not twis_control->twis_registers->EVENTS_STOPPED)
        {
            // Block while pending TWI transactions complete.
        }
        twis_control->transfer_state = transfer_state::ready;
    }

    twis_events_clear_all(twis_control);
}

static void irq_handler_twis(struct twis_control_block_t* twis_control)
{
    logger &logger = logger.instance();
    logger.debug("+++ %s", __func__);

    struct twis_event_t event = {
        .type = twi_event_none,
        .xfer = {
            .tx_bytes = 0u,
            .rx_bytes = 0u
        },
        .addr_index = static_cast<uint8_t>(twis_control->twis_registers->MATCH)
    };

    if (twis_control->twis_registers->EVENTS_STOPPED)
    {
        twis_control->transfer_state = transfer_state::ready;

        event.type         |= twi_event_stopped;
        event.xfer.tx_bytes = twis_control->twis_registers->TXD.AMOUNT;
        event.xfer.rx_bytes = twis_control->twis_registers->RXD.AMOUNT;
        twis_clear_event_register(&twis_control->twis_registers->EVENTS_STOPPED);
    }

    if (twis_control->twis_registers->EVENTS_ERROR)
    {
        twis_control->transfer_state = transfer_state::ready;

        uint32_t const error_source = twis_control->twis_registers->ERRORSRC;

        event.type |= (error_source & TWI_ERRORSRC_OVERRUN_Msk) ?
            twi_event_rx_overrun : 0u;
        event.type |= (error_source & TWI_ERRORSRC_DNACK_Msk) ?
            twi_event_data_nack : 0u;
        event.type |= (error_source & TWIS_ERRORSRC_OVERREAD_Msk) ?
            twi_event_tx_overrun : 0u;

        event.xfer.tx_bytes = twis_control->twis_registers->TXD.AMOUNT;
        event.xfer.rx_bytes = twis_control->twis_registers->RXD.AMOUNT;

        twis_clear_event_register(&twis_control->twis_registers->EVENTS_ERROR);
    }

    if (twis_control->twis_registers->EVENTS_RXSTARTED)
    {
        twis_control->transfer_state = transfer_state::ready;
        event.type |= twi_event_rx_started;
        twis_clear_event_register(&twis_control->twis_registers->EVENTS_RXSTARTED);
    }

    if (twis_control->twis_registers->EVENTS_TXSTARTED)
    {
        twis_control->transfer_state = transfer_state::ready;
        event.type |= twi_event_tx_started;
        twis_clear_event_register(&twis_control->twis_registers->EVENTS_TXSTARTED);
    }

    if (twis_control->twis_registers->EVENTS_WRITE)
    {
        event.type |= twis_event_write_cmd;
        twis_clear_event_register(&twis_control->twis_registers->EVENTS_WRITE);
    }

    if (twis_control->twis_registers->EVENTS_READ)
    {
        event.type |= twis_event_read_cmd;
        twis_clear_event_register(&twis_control->twis_registers->EVENTS_READ);
    }

    twis_control->handler(&event, twis_control->context);
    logger.debug("--- %s", __func__);
}

