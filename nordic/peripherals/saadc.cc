/**
 * @file saadc.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "saadc.h"
#include "ppi.h"
#include "logger.h"
#include "nrf_cmsis.h"
#include "arm_utilities.h"
#include "project_assert.h"

#include <iterator>

/// The number of SAADC analog input conversion channels.
static uint8_t const SAADC_INPUT_COUNT = 8u;

/// The SAADC time of conversion in microsoeconds.
static uint32_t const t_acq_conv = 2u;

/// Interrupt registers are 32-bits wide.
/// Use this value to clear all interrupts.
static uint32_t const interrupts_clear_all = UINT32_MAX;

/**
 * @struct saadc_control_block_t
 * There is one SAADC on an nrf5x SoC. True for both nrf52832 and nrf52840.
 * SAADC_IRQn is the interrupt line.
 */
struct saadc_control_block_t
{
    /// Pointer to the SAADC peripheral instance registers.
    NRF_SAADC_Type * const saadc_registers;

    /**
     * SAADC peripheral instance IRQ number.
     * Since there is only one SAADC per SoC, this is set to SAADC_IRQn.
     */
    IRQn_Type const irq_type;

    ppi_channel_t ppi_trigger;
    ppi_channel_t ppi_sample;

    /// SAADC conversion completion data destination buffer.
    int16_t* destination_pointer;

    /// SAADC conversion completion data destination buffer length in uint16_t words.
    uint16_t destination_length;

    /// The user supplied callback function.
    /// When the spi transfer is complete this function is called.
    saadc_event_handler_t handler;

    /// The user supplied context.
    /// This is carried by the SAADC interface but never modified by the driver.
    void *context;
};

static void irq_handler_saadc(struct saadc_control_block_t* saadc_control);

static struct saadc_control_block_t saadc_instance_0 =
{
    .saadc_registers        = reinterpret_cast<NRF_SAADC_Type *>(NRF_SAADC_BASE),
    .irq_type               = SAADC_IRQn,
    .ppi_trigger            = ppi_channel_invalid,
    .ppi_sample             = ppi_channel_invalid,
    .destination_pointer    = nullptr,
    .destination_length     = 0u,
    .handler                = nullptr,
    .context                = nullptr
};

extern "C" void SAADC_IRQHandler(void)
{
    irq_handler_saadc(&saadc_instance_0);
}

/**
 * @return bool true if the SAADC.ENABLE register is set to enable.
 *              false if the SAADC.ENABLE register is cleared; not enabled.
 */
static bool saadc_is_enabled(struct saadc_control_block_t const *saadc_control)
{
    return bool(saadc_control->saadc_registers->ENABLE & SAADC_ENABLE_ENABLE_Enabled);
}

/**
 * See nRF52 Section 15.8.1 Peripheral Interface, Interrupt clearing
 * Clearing an interrupt may take 4 cycles. Read the register to insure the
 * interrupt is cleared before exiting the ISR.
 */
static void saadc_clear_event_register(uint32_t volatile* saadc_register)
{
    *saadc_register = 0u;
    volatile uint32_t dummy = *saadc_register;
    (void) dummy;
}

static void saadc_check_sizes(void)
{
    NRF_SAADC_Type saadc_registers_check;
    static_assert(SAADC_INPUT_COUNT == std::size(saadc_registers_check.CH));
}

static uint32_t t_acq_usec(enum saadc_tacq_t t_acq)
{
    switch (t_acq)
    {
    case saadc_tacq__3_usec: return  3u;
    case saadc_tacq__5_usec: return  5u;
    case saadc_tacq_10_usec: return 10u;
    case saadc_tacq_15_usec: return 15u;
    case saadc_tacq_20_usec: return 20u;
    case saadc_tacq_40_usec: return 40u;
    default:
        ASSERT(0);
        return 0u;
    }
}

void saadc_input_configure(
    uint8_t                         input_channel,
    enum saadc_input_drive_t        drive,
    enum saadc_input_select_t       analog_in_positive,
    enum saadc_input_termination_t  termination_positive,
    enum saadc_input_select_t       analog_in_negative,
    enum saadc_input_termination_t  termination_negative,
    enum saadc_gain_t               gain,
    enum saadc_reference_select_t   reference_select,
    enum saadc_tacq_t               t_acq)
{
    ASSERT(input_channel < SAADC_INPUT_COUNT);
    ASSERT(not saadc_conversion_in_progress());

    ASSERT(static_cast<uint8_t>(analog_in_positive)    <= saadc_input_select_VDD);
    ASSERT(static_cast<uint8_t>(analog_in_negative)    <= saadc_input_select_VDD);
    ASSERT(static_cast<uint8_t>(termination_positive)  <= saadc_input_termination_pull_middle);
    ASSERT(static_cast<uint8_t>(termination_negative)  <= saadc_input_termination_pull_middle);
    ASSERT(static_cast<uint8_t>(gain)                  <= saadc_gain_mul_4);
    ASSERT(static_cast<uint8_t>(reference_select)      <= saadc_reference_VDD_div_4);
    ASSERT(static_cast<uint8_t>(t_acq)                 <= saadc_tacq_40_usec);
    ASSERT(static_cast<uint8_t>(drive)                 <= saadc_input_differential);

    NRF_SAADC_Type *saadc_registers = saadc_instance_0.saadc_registers;

    saadc_registers->CH[input_channel].PSELP = analog_in_positive;
    saadc_registers->CH[input_channel].PSELN = analog_in_negative;

    // Note: Burst mode is disabled and oversampling not used.
    uint32_t const channel_config =
        (termination_positive   << SAADC_CH_CONFIG_RESP_Pos)   |
        (termination_negative   << SAADC_CH_CONFIG_RESN_Pos)   |
        (gain                   << SAADC_CH_CONFIG_GAIN_Pos)   |
        (reference_select       << SAADC_CH_CONFIG_REFSEL_Pos) |
        (t_acq                  << SAADC_CH_CONFIG_TACQ_Pos)   |
        (drive                  << SAADC_CH_CONFIG_MODE_Pos)   |
        0;

    saadc_registers->CH[input_channel].CONFIG = channel_config;
    saadc_disable_limit_event(input_channel);
}

void saadc_input_configure_single_ended(
    uint8_t                         input_channel,
    enum saadc_input_select_t       analog_in_positive,
    enum saadc_input_termination_t  termination_positive,
    enum saadc_gain_t               gain,
    enum saadc_reference_select_t   reference_select,
    enum saadc_tacq_t               t_acq)
{
    saadc_input_configure(
        input_channel,
        saadc_input_single_ended,
        analog_in_positive,
        termination_positive,
        saadc_input_select_NC,
        saadc_input_termination_pull_down,
        gain,
        reference_select,
        t_acq
    );
}

void saadc_input_disable(uint8_t input_channel)
{
    // Set registers CH[].PSELP, CH[].PSELN, CH[].CONFIG to defaults
    saadc_input_configure(
        input_channel,
        saadc_input_single_ended,
        saadc_input_select_NC,
        saadc_input_termination_none,
        saadc_input_select_NC,
        saadc_input_termination_none,
        saadc_gain_div_6,
        saadc_reference_600mV,
        saadc_tacq_10_usec
    );
}

bool saadc_input_is_enabled(uint8_t input_channel)
{
    NRF_SAADC_Type *saadc_registers = saadc_instance_0.saadc_registers;
    return (saadc_registers->CH[input_channel].PSELP != saadc_input_select_NC);
}

void saadc_init(enum saadc_conversion_resolution_t  resolution,
                void*                               context,
                uint8_t                             irq_priority)
{
    ASSERT(not saadc_is_enabled(&saadc_instance_0));
    ASSERT(interrupt_priority_is_valid(irq_priority));

    saadc_check_sizes();

    // Short hand for readability.
    NRF_SAADC_Type* const saadc_registers = saadc_instance_0.saadc_registers;

    saadc_instance_0.destination_pointer    = nullptr;
    saadc_instance_0.destination_length     = 0u;
    saadc_instance_0.handler                = nullptr;
    saadc_instance_0.context                = nullptr;

    // During the life time of this driver - until deinit() is called -
    // the SAADC EVENTS_STARTED event will trigger the SAADC TASKS_SAMPLE
    // task to start taking samples using PPI. Otherwise this would have to be
    // done in the ISR and introduce software latency as sample jitter.
    saadc_instance_0.ppi_sample = ppi_channel_allocate(&saadc_registers->TASKS_SAMPLE,
                                                       &saadc_registers->EVENTS_STARTED,
                                                       nullptr);
    ppi_channel_enable(saadc_instance_0.ppi_sample);
    logger::instance().debug("ppi sample channel: %u", saadc_instance_0.ppi_sample);

    // Defer allocation of the saadc_instance_0.ppi_trigger until a client
    // requests it via the call to saadc_conversion_start_on_trigger().

    saadc_registers->INTEN          = 0u;
    saadc_registers->ENABLE         = 0u;

    saadc_registers->INTENCLR       = interrupts_clear_all; // Clear all SAADC interrupts.
    saadc_registers->RESOLUTION     = saadc_conversion_resolution_12_bit;
    saadc_registers->OVERSAMPLE     = 0u;                   // disable oversampling.
    saadc_registers->SAMPLERATE     = 0u;                   // Use task to trigger SAADC conversions.

    saadc_registers->RESULT.PTR     = 0u;                   // The conversion result:
    saadc_registers->RESULT.MAXCNT  = 0u;                   // init to nullptr, zero length.

    for (uint8_t input_channel = 0u; input_channel < SAADC_INPUT_COUNT; ++input_channel)
    {
        saadc_input_disable(input_channel);
    }

    NVIC_SetPriority(saadc_instance_0.irq_type, irq_priority);
    NVIC_ClearPendingIRQ(saadc_instance_0.irq_type);
}

void saadc_deinit(void)
{
    if (not saadc_is_enabled(&saadc_instance_0))
    {
        logger::instance().warn("saadc_deinit(): SAADC not enabled");
    }

    NRF_SAADC_Type *saadc_registers = saadc_instance_0.saadc_registers;

    /// Release our PPI channels.
    ppi_channel_release(saadc_instance_0.ppi_trigger);
    ppi_channel_release(saadc_instance_0.ppi_sample);

    saadc_instance_0.ppi_trigger = ppi_channel_invalid;
    saadc_instance_0.ppi_sample  = ppi_channel_invalid;

    saadc_registers->INTEN  = 0u;   // Disable all interrupts
    saadc_registers->ENABLE = 0u;   // Disable SAADC operation

    // Clear all SAADC events.
    saadc_clear_event_register(&saadc_registers->EVENTS_STARTED);
    saadc_clear_event_register(&saadc_registers->EVENTS_END);
    saadc_clear_event_register(&saadc_registers->EVENTS_DONE);
    saadc_clear_event_register(&saadc_registers->EVENTS_RESULTDONE);
    saadc_clear_event_register(&saadc_registers->EVENTS_CALIBRATEDONE);
    saadc_clear_event_register(&saadc_registers->EVENTS_STOPPED);

    for (uint8_t input_channel = 0u; input_channel < SAADC_INPUT_COUNT; ++input_channel)
    {
        saadc_clear_event_register(&saadc_registers->EVENTS_CH[input_channel].LIMITH);
        saadc_clear_event_register(&saadc_registers->EVENTS_CH[input_channel].LIMITL);
    }
}

void saadc_conversion_start(int16_t*              destination_pointer,
                            uint16_t              destination_length,
                            saadc_event_handler_t saadc_handler,
                            uint32_t volatile*    event_register)
{
    ASSERT(not saadc_conversion_in_progress());
    ASSERT(destination_pointer);

    NRF_SAADC_Type *saadc_registers = saadc_instance_0.saadc_registers;

    struct saadc_conversion_info_t const channel_conversion =
        saadc_conversion_info();
    ASSERT(channel_conversion.channel_count <= destination_length);

    saadc_registers->ENABLE = 1u;

    // Enable the STARTED and END events.
    // Event END generates an interrupt when the DMA has filled the
    // destination buffer with converted data.
    saadc_registers->INTENSET =
        SAADC_INTEN_STARTED_Msk     |
        SAADC_INTEN_END_Msk         |
//      SAADC_INTEN_DONE_Msk        |       // debug only.
//      SAADC_INTEN_RESULTDONE_Msk  |       // debug only.
        SAADC_INTEN_STOPPED_Msk     ;

    saadc_instance_0.destination_length     = destination_length;
    saadc_instance_0.destination_pointer    = destination_pointer;
    saadc_instance_0.handler                = saadc_handler;

    saadc_registers->RESULT.MAXCNT = destination_length;
    saadc_registers->RESULT.PTR    = reinterpret_cast<uintptr_t>(destination_pointer);

    NVIC_ClearPendingIRQ(saadc_instance_0.irq_type);
    NVIC_EnableIRQ(saadc_instance_0.irq_type);

    if (event_register == nullptr)
    {
        // The PPI channel for triggering the SAADC was previously allocated.
        // Insure that it is disabled since the client is specifying that the
        // conversion start with this function call. Don't have a second or
        // interfering trigger start the conversion.
        if (saadc_instance_0.ppi_trigger != ppi_channel_invalid)
        {
            ppi_channel_disable(saadc_instance_0.ppi_trigger);
        }
        saadc_registers->TASKS_START = 1u;
    }
    else
    {
        // The client is requesting that the SAADC begin sampling based on
        // a peripheral generated event.
        // If the channel has not been allocated, allocate and associate it.
        if (saadc_instance_0.ppi_trigger == ppi_channel_invalid)
        {
            saadc_instance_0.ppi_trigger = ppi_channel_allocate(
                &saadc_registers->TASKS_START, event_register, nullptr);

            logger::instance().debug("ppi trigger channel: %u", saadc_instance_0.ppi_trigger);
        }
        else
        {
            // Event if already allocated, binding the event and task does no
            // harm and insures the association is correct.
            ppi_channel_bind_task(saadc_instance_0.ppi_trigger, &saadc_registers->TASKS_START);
            ppi_channel_bind_event(saadc_instance_0.ppi_trigger, event_register);
        }

        ppi_channel_enable(saadc_instance_0.ppi_trigger);
    }
}

void saadc_conversion_stop(void)
{
    if (not saadc_is_enabled(&saadc_instance_0))
    {
        // Warn that the SAADC is not enabled.
        // Continue generating the STOP event; the client may be relying on it.
        logger::instance().warn("saadc_disable(): SAADC not enabled");
    }

    NRF_SAADC_Type *saadc_registers = saadc_instance_0.saadc_registers;

    saadc_registers->INTENCLR   = interrupts_clear_all;
    saadc_registers->INTENSET   = SAADC_INTEN_STOPPED_Msk;
    saadc_registers->TASKS_STOP = 1u;
}

struct saadc_conversion_info_t saadc_conversion_info(void)
{
    saadc_conversion_info_t channel_conversion = {
        .time_usec     = 0u,
        .channel_count = 0u
    };

    for (uint8_t input_channel = 0u; input_channel < SAADC_INPUT_COUNT; ++input_channel)
    {
        if (saadc_input_is_enabled(input_channel))
        {
            channel_conversion.channel_count += 1u;

            NRF_SAADC_Type *saadc_registers = saadc_instance_0.saadc_registers;
            uint32_t const config = saadc_registers->CH[input_channel].CONFIG;

            enum saadc_tacq_t const t_acq = static_cast<enum saadc_tacq_t>(
                (config & SAADC_CH_CONFIG_TACQ_Msk) >> SAADC_CH_CONFIG_TACQ_Pos);

            channel_conversion.time_usec += t_acq_usec(t_acq) + t_acq_conv;
        }
    }

    return channel_conversion;
}

static uint32_t saadc_make_limits(int16_t limit_lower, int16_t limit_upper)
{
    uint32_t limits = static_cast<uint16_t>(limit_upper);
    limits <<= 16u;
    limits |= static_cast<uint16_t>(limit_lower);
    return limits;
}

void saadc_enable_limits_event(uint8_t input_channel,
                               int16_t limit_lower,
                               int16_t limit_upper)
{
    ASSERT(input_channel < SAADC_INPUT_COUNT);

    uint32_t const limits             = saadc_make_limits(limit_upper, limit_lower);
    uint32_t const limit_lower_enable = (input_channel * 2u) << SAADC_INTENSET_CH0LIMITL_Pos;
    uint32_t const limit_upper_enable = (input_channel * 2u) << SAADC_INTENSET_CH0LIMITH_Pos;

    NRF_SAADC_Type *saadc_registers = saadc_instance_0.saadc_registers;

    saadc_registers->INTENCLR |= (limit_lower_enable | limit_upper_enable);
    saadc_registers->CH[input_channel].LIMIT = limits;

    if (limit_lower == INT16_MIN)           // INT16_MIN: -32768, 0x8000
    {
        saadc_registers->INTENSET |= limit_upper_enable;
    }
    else if (limit_upper == INT16_MAX)      // INT16_MAX:  32767, 0x7FFF
    {
        saadc_registers->INTENSET |= limit_lower_enable;
    }
    else
    {
        saadc_registers->INTENSET |= (limit_lower_enable | limit_upper_enable);
    }
}

void saadc_enable_lower_limit_event(uint8_t input_channel, uint16_t limit_lower)
{
    saadc_enable_limits_event(input_channel, limit_lower, INT16_MAX);
}

void saadc_enable_upper_limit_event(uint8_t input_channel, int16_t limit_upper)
{
    saadc_enable_limits_event(input_channel, INT16_MIN, limit_upper);
}

void saadc_disable_limit_event(uint8_t input_channel)
{
    ASSERT(input_channel < SAADC_INPUT_COUNT);
    uint32_t const limits             = saadc_make_limits(INT16_MIN, INT16_MAX);
    uint32_t const limit_lower_enable = (input_channel * 2u) << SAADC_INTENSET_CH0LIMITL_Pos;
    uint32_t const limit_upper_enable = (input_channel * 2u) << SAADC_INTENSET_CH0LIMITH_Pos;

    NRF_SAADC_Type *saadc_registers = saadc_instance_0.saadc_registers;

    saadc_registers->INTENCLR |= (limit_lower_enable | limit_upper_enable);
    saadc_registers->CH[input_channel].LIMIT = limits;
}

struct saadc_limits_t saadc_get_channel_limits(uint8_t input_channel)
{
    NRF_SAADC_Type *saadc_registers = saadc_instance_0.saadc_registers;

    uint32_t const limits = saadc_registers->CH[input_channel].LIMIT;

    struct saadc_limits_t const limit_pair = {
        .lower = static_cast<int16_t>(limits >>  0u),
        .upper = static_cast<int16_t>(limits >> 16u),
    };

    return limit_pair;
}

bool saadc_conversion_in_progress(void)
{
    NRF_SAADC_Type *saadc_registers = saadc_instance_0.saadc_registers;
    return bool(saadc_registers->STATUS & SAADC_STATUS_STATUS_Busy);
}

static void irq_handler_saadc(struct saadc_control_block_t* saadc_control)
{
    NRF_SAADC_Type *saadc_registers = saadc_control->saadc_registers;
    int16_t const event_value_ignore = -1;
    logger& logger = logger::instance();

    if (saadc_registers->EVENTS_STARTED)
    {
        logger.debug("IRQ: EVENTS_STARTED");
        // If the PPI channel saadc_instance_0.ppi_sample  were not used
        // saadc_registers->TASKS_SAMPLE = 1u; would be required here.
        saadc_clear_event_register(&saadc_registers->EVENTS_STARTED);
        saadc_control->handler(saadc_event_conversion_start,
                               event_value_ignore,
                               saadc_control->context);
    }

    if (saadc_registers->EVENTS_END)
    {
        logger.debug("IRQ: EVENTS_END");
        saadc_clear_event_register(&saadc_registers->EVENTS_END);
        saadc_control->handler(saadc_event_conversion_complete,
                               saadc_registers->RESULT.AMOUNT,
                               saadc_control->context);
    }

    if (saadc_registers->EVENTS_DONE)
    {
        logger.debug("IRQ: EVENTS_DONE");
        saadc_clear_event_register(&saadc_registers->EVENTS_DONE);
    }

    if (saadc_registers->EVENTS_RESULTDONE)
    {
        logger.debug("IRQ: EVENTS_RESULTDONE");
        saadc_clear_event_register(&saadc_registers->EVENTS_RESULTDONE);
    }

    if (saadc_registers->EVENTS_CALIBRATEDONE)
    {
        logger.debug("IRQ: EVENTS_CALIBRATEDONE");
        saadc_clear_event_register(&saadc_registers->EVENTS_CALIBRATEDONE);
        saadc_control->handler(saddc_event_calibration_complete,
                               event_value_ignore,
                               saadc_control->context);
    }

    if (saadc_registers->EVENTS_STOPPED)
    {
        logger.debug("IRQ: EVENTS_STOPPED");
        saadc_clear_event_register(&saadc_registers->EVENTS_STOPPED);
        saadc_control->handler(saadc_event_conversion_stop,
                               saadc_registers->RESULT.AMOUNT,
                               saadc_control->context);
    }

    for (uint8_t input_channel = 0u; input_channel < SAADC_INPUT_COUNT; ++input_channel)
    {
        if (saadc_registers->EVENTS_CH[input_channel].LIMITL)
        {
            logger.debug("IRQ: LIMITL[%u]: 0x%08x", input_channel, saadc_registers->CH[input_channel].LIMIT);
            saadc_clear_event_register(&saadc_registers->EVENTS_CH[input_channel].LIMITL);
            saadc_control->handler(saadc_event_limit_lower,
                                   input_channel,
                                   saadc_control->context);
        }

        if (saadc_registers->EVENTS_CH[input_channel].LIMITH)
        {
            logger.debug("IRQ: LIMITH[%u]: 0x%08x", input_channel, saadc_registers->CH[input_channel].LIMIT);
            saadc_clear_event_register(&saadc_registers->EVENTS_CH[input_channel].LIMITH);
            saadc_control->handler(saadc_event_limit_upper,
                                   input_channel,
                                   saadc_control->context);
        }
    }
}
