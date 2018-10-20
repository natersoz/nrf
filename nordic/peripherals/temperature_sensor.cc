/**
 * @file temperature_sensor.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "temperature_sensor.h"
#include "logger.h"
#include "nrf_cmsis.h"
#include "arm_utilities.h"
#include "project_assert.h"

#include <iterator>

/**
 * @struct temp_control_block_t
 * There is one TEMP on an nrf5x SoC. True for both nrf52832 and nrf52840.
 * TEMP_IRQn is the interrupt line.
 */
struct temp_control_block_t
{
    /// Pointer to the TEMP peripheral instance registers.
    NRF_TEMP_Type * const temp_registers;

    /**
     * SAADC peripheral instance IRQ number.
     * Since there is only one SAADC per SoC, this is set to SAADC_IRQn.
     */
    IRQn_Type const irq_type;

    /// The user supplied callback function.
    /// When the spi transfer is complete this function is called.
    /// When the handler is null it indicates that a measurement is not in
    /// progress. When set, a new measurement cannot start until the pending
    /// measurement completes.
    temperature_sensor_event_handler_t handler;

    /// The user supplied context.
    /// This is carried by the SAADC interface but never modified by the driver.
    void *context;
};

static void irq_handler_temp(struct temp_control_block_t* temp_control);

static struct temp_control_block_t temp_instance_0 =
{
    .temp_registers = reinterpret_cast<NRF_TEMP_Type *>(NRF_TEMP_BASE),
    .irq_type       = TEMP_IRQn,
    .handler        = nullptr,
    .context        = nullptr
};

extern "C" void TEMP_IRQHandler(void)
{
    irq_handler_temp(&temp_instance_0);
}

static void temp_clear_event_register(uint32_t volatile* temperature_register)
{
    *temperature_register = 0u;
    volatile uint32_t dummy = *temperature_register;
    (void) dummy;
}

bool temperature_sensor_take_measurement(
    temperature_sensor_event_handler_t  handler,
    void*                               context)
{
    // The handler parameter is required.
    // What good is a measurement without a result?
    if (not handler) { return false; }

    // Temperature measurement already in progress.
    if (temp_instance_0.handler) { return false; }

    uint8_t const irq_priority = 7u;
    NVIC_SetPriority(temp_instance_0.irq_type, irq_priority);
    NVIC_ClearPendingIRQ(temp_instance_0.irq_type);
    NVIC_EnableIRQ(temp_instance_0.irq_type);

    temp_instance_0.handler                     = handler;
    temp_instance_0.temp_registers->INTENSET    = TEMP_INTENSET_DATARDY_Msk;
    temp_instance_0.temp_registers->TASKS_START = 1u;

    return true;
}

static void irq_handler_temp(struct temp_control_block_t* temp_control)
{
    logger& logger = logger::instance();

    if (temp_control->temp_registers->EVENTS_DATARDY)
    {
        logger.debug("IRQ TEMP: EVENTS_DATARDY");
        temp_clear_event_register(&temp_control->temp_registers->EVENTS_DATARDY);

        int32_t const temp_Cx4 = temp_control->temp_registers->TEMP;

        // STOP must come after the reading of the TEMP register.
        // Errata in nRF5 series.
        temp_control->temp_registers->TASKS_STOP = 1u;

        if (temp_control->handler)
        {
            temp_control->handler(temp_Cx4, temp_control->context);
        }

        temp_control->handler = nullptr;
    }
}

