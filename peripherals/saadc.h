/**
 * @file saadc.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

enum saadc_event_t
{
    /// This event is sent in response to the EVENTS_STARTED interrupt.
    /// The RESULT.PTR register is double-buffered can be updated
    /// immediately after the EVENTS_STARTED event is generated.
    saadc_event_conversion_start,
    saadc_event_conversion_stop,
    saadc_event_conversion_complete,
    saadc_event_limit_lower,
    saadc_event_limit_upper,
    saddc_event_calibration_complete,
};

/**
 * @enum saadc_input_mode_t
 * Determine the ADC input drive type: single ended or differential.
 *
 * @note All of these enums are specific to the Nordic SAADC peripheral
 * and map to specific register settings values.
 */
enum saadc_input_drive_t
{
    saadc_input_single_ended            = 0u,
    saadc_input_differential            = 1u,
};

/**
 * @enum saadc_input_termination_t
 * Determine the input resistive termination.
 * Pull 'middle' means bot hpull up and down are enabled.
 * 'None' means not resistive pull up/down is enabled.
 */
enum saadc_input_termination_t
{
    saadc_input_termination_none        = 0u,   ///< No pull up or down enabled.
    saadc_input_termination_pull_down   = 1u,
    saadc_input_termination_pull_up     = 2u,
    saadc_input_termination_pull_middle = 3u    ///< Both pull up and down enabled.
};

enum saadc_input_select_t
{
    /// ADC input not connected.
    saadc_input_select_NC               = 0u,

    /// @note AIN0 : value 1u, an intentional index offset.
    saadc_input_AIN0                    = 1u,
    saadc_input_AIN1                    = 2u,
    saadc_input_AIN2                    = 3u,
    saadc_input_AIN3                    = 4u,
    saadc_input_AIN4                    = 5u,
    saadc_input_AIN5                    = 6u,
    saadc_input_AIN6                    = 7u,
    saadc_input_AIN7                    = 8u,

    /// saadc_input_select_VDD can be appiled as the
    /// saadc_input_pin_config_t::input_pin setting for selecting VDD input
    /// without an external pin.
    saadc_input_select_VDD              = 9u
};

/**
 * @enum saadc_gain_t
 * Select the input gain.
 */
enum saadc_gain_t
{
    saadc_gain_div_6            = 0u,
    saadc_gain_div_5            = 1u,
    saadc_gain_div_4            = 2u,
    saadc_gain_div_3            = 3u,
    saadc_gain_div_2            = 4u,
    saadc_gain_unity            = 5u,
    saadc_gain_mul_2            = 6u,
    saadc_gain_mul_4            = 7u,
};

/**
 * @enum saadc_reference_select_t
 * Set the ADC input reference voltage.
 *
 * The ADC input range is calculated as:
 * Range = Vref / Gain
 *
 * @example For VDD unity gain:
 * Vref = VDD/4
 * Gain = 1/4
 * Range = Vref / Gain = (VDD/4) / (1/4)
 *
 * @example For Vref internal 600 mV:
 * Vref = 600 mV
 * Gain = 1/6
 * Range = 600 mV / (1/6) = 3600 mV
 *
 * Input voltage, at the input pins, must be: VSS <= Vin <= VDD.
 *
 * Conversion Result:
 * result = (Vp - Vn) * (Gain / VRef) * 2 ^ (resoultion - m)
 * Where m is 0:single ended input, 1: differential input
 *
 * @example
 * single ended, 100 mV input signal, VRef internal, Gain: 1/6, res: 12 bits:
 * result = (100 mV - 0mV) * (1/6) / 600 mV * 2^12
 *        = 100 mV * (1/3600 mV) * 4096 = 114
 * Or, 1.14 bits/mV, 0.879 mV / bit.
 */
enum saadc_reference_select_t
{
    saadc_reference_600mV       = 0u,
    saadc_reference_VDD_div_4   = 1u,
};

/**
 * @enum saadc_tacq_t
 * Set the ADC input sample and hold time.
 *
 * @warning Nordic Errata:
 * 3.41 [150] SAADC: EVENT_STARTED does not fire
 * This anomaly applies to IC Rev. Rev 2, build codes QFAA-E00, CIAA-E00, QFAB-E00.
 * It was inherited from the previous IC revision Rev 1.
 * Symptoms:        EVENT_STARTED does not fire.
 * Conditions:      ADC started (TASKS_START) with PPI task.
 *                  Any channel configured to TACQ <= 5 μs.
 * Consequences:    ADC cannot be started (TASKS_START) with PPI if TACQ <= 5 μs.
 *
 * http://infocenter.nordicsemi.com/pdf/nRF52832_Rev_2_Errata_v1.1.pdf
 */
enum saadc_tacq_t
{
    saadc_tacq__3_usec          = 0u,
    saadc_tacq__5_usec          = 1u,
    saadc_tacq_10_usec          = 2u,
    saadc_tacq_15_usec          = 3u,
    saadc_tacq_20_usec          = 4u,
    saadc_tacq_40_usec          = 5u,
};

/// Select the SAADC conversion resolution.
enum saadc_conversion_resolution_t
{
    saadc_conversion_resolution__8_bit = 0u,
    saadc_conversion_resolution_10_bit = 1u,
    saadc_conversion_resolution_12_bit = 2u,
    saadc_conversion_resolution_14_bit = 3u,
};

/**
 * @brief SAADC event handler type.
 * @param event The reason that the event handler was called
 * @param event_value
 * saadc_event_limits_exceeded:     The input channel associated with a the event type.
 * saadc_event_conversion_complete: The number of int16_t conversion values delivered.
 * saadc_event_conversion_stop:     The number of int16_t conversion values delivered.
 * All others:                      -1
 * @param context The user supplied context.
 */
typedef void (* saadc_event_handler_t) (saadc_event_t   event,
                                        int16_t         event_value,
                                        void*           context);

/**
 * General function for configuring an SAADC analog input for conversion.
 * Configuring an SAADC conversion channel for differential inputs use this function.
 *
 * @param input_channel         The channel to configure: [0:7].
 * @param drive                 The drive type: differential or single ended.
 * @param analog_in_positive    The AIN[index] signal to allocate for the + input.
 * @param termination_positive  How to terminate the + input.
 * @param analog_in_negative    The AIN[index] signal to allocate for the - input
 * @param termination_negative  How to terminate the - input.
 * @param gain                  @see enum saadc_gain_t
 * @param reference_select      @see enum saadc_reference_select_t
 * @param t_acq                 @see enum saadc_tacq_t
 */
void saadc_input_configure(
    uint8_t                         input_channel,
    enum saadc_input_drive_t        drive,
    enum saadc_input_select_t       analog_in_positive,
    enum saadc_input_termination_t  termination_positive,
    enum saadc_input_select_t       analog_in_negative,
    enum saadc_input_termination_t  termination_negative,
    enum saadc_gain_t               gain,
    enum saadc_reference_select_t   reference_select,
    enum saadc_tacq_t               t_acq);

/**
 * Short-hand function for configuring an SAADC analog input for single-ended conversion.
 *
 * @param input_channel         The channel to configure: [0:7].
 * @param analog_in_positive    The AIN[index] signal to allocate for the + input.
 * @param termination_positive  How to terminate the + input.
 * @param gain                  @see enum saadc_gain_t
 * @param reference_select      @see enum saadc_reference_select_t
 * @param t_acq                 @see enum saadc_tacq_t
 */
void saadc_input_configure_single_ended(
    uint8_t                         input_channel,
    enum saadc_input_select_t       analog_in_positive,
    enum saadc_input_termination_t  termination_positive,
    enum saadc_gain_t               gain,
    enum saadc_reference_select_t   reference_select,
    enum saadc_tacq_t               t_acq);

/**
 * Disable the SAADC channel conversion.
 * The next triggered conversion will not include this channel in the conversion
 * process nor in the results buffer.
 *
 * @param input_channel The analog input channel to disable.
 */
void saadc_input_disable(uint8_t input_channel);

/**
 * Determine whether an input channel is enabled.
 *
 * @param input_channel The input channel in question.
 *
 * @return bool true if the SAADC input channe is enabled; false if not.
 */
bool saadc_input_is_enabled(uint8_t input_channel);

/**
 * Initialize the SAADC device driver.
 * @note Placement of resolution and priority parameters could have
 * been deferred until saadc_enable() is called. This would be more versatile.
 * However, from a use-case perspective, this seems simpler.
 *
 * @note oversampling and burst mode not supported.
 *
 * @param resolution   The resolution of the SAADC conversions.
 * @param context      A user suppplied object that is unmodified by the SAADC
 *                     driver and is supplied as part of the event handler callback.
 * @param irq_priority The interrupt priority of the conversion.
 *                     Typically the value 7 is used in nRF5x designs.
 *                     Values 0, 1 and 4 are reserved for the softdevice.
 */
void saadc_init(enum saadc_conversion_resolution_t  resolution,
                void*                               context,
                uint8_t                             irq_priority);

/**
 * Stop any pending conversions or conversions in progress and release the
 * resources associated with the SAADC driver.
 */
void saadc_deinit(void);

/**
 * Start a SAADC analog to digital conversion.
 * The conversion starts when this function is called.
 *
 * @param destination_pointer The destination buffer of the converted ADC samples.
 *                            Since the SAADC converts on 16-bit boundaries, using
 *                            int16_t will guarantee correct data alignment and
 *                            be in agreement with the sign-extended data produced
 *                            by the SAADC conversion.
 * @param desitnation_length  The number of int16_t values that are allocated for
 *                            conversion. This value must be greater than or equal
 *                            to the number of channels being converted.
 * @param saadc_handler       The user supplied callback completion handler.
 *                            This handler gets called when the SAADC conversion
 *                            is complete and the samples transfered into the user
 *                            supplied memory locations.
 * @param event_register_pointer
 *                            The Nordic peripheral based event which will trigger
 *                            the conversion via a PPI connection.
 *                            If this value is null then the conversion starts
 *                            immediately.
 *
 * @warning Nordic Errata:    3.41 [150] SAADC: EVENT_STARTED does not fire.
 *                            If a SAADC is configured with an acquisition time
 *                            <= 5 usec and the trigger mode is PPI
 *                            (event_register_pointer != null) then the trigger
 *                            will be missed. Error checking guarding against
 *                            this condition is not provided in the driver.
 */
void saadc_conversion_start(int16_t*                destination_pointer,
                            uint16_t                desitnation_length,
                            saadc_event_handler_t   saadc_handler,
                            uint32_t volatile*      event_register_pointer);

/**
 * Stop any pending and current SAADC conversions.
 * A saadc_event_conversion_stop is generated.
 */
void saadc_conversion_stop(void);

/// Used for returning values from saadc_conversion_info()
struct saadc_conversion_info_t
{
    /// The time required for the SAADC conversion based on the
    /// number of configured channels, their sample and hold times and
    /// the time require for the SAADC to complete its conversion.
    uint16_t time_usec;

    /// The number of channels enabled for SAADC conversion.
    uint8_t  channel_count;
};

/**
 * Determine the number of channels configured for SAADC conversion and the
 * total amount of time, in microseconds, that the aggregate conversion
 * will take. Attempting to perform groups of conversions faster than this
 * time will cause aborted conversions.
 *
 * @return struct saadc_conversion_info_t The SAADC channel count and
 *                                        conversion time.
 */
struct saadc_conversion_info_t saadc_conversion_info(void);

/**
 * Enable callbacks onto the event handler based on the ADC exceeding specified
 * limits. When an ADC conversion exceeds the (lower:upper) limit bound the
 * event handler supplied into the saadc_enable() function will be called.
 * @todo Need to specify an event type for this situation.
 *
 * @param input_channel The analog channel to check for limits events.
 * @param limit_lower   The lower limit.
 * @param limit_upper   The upper limit.
 */
void saadc_enable_limits_event(uint8_t  input_channel,
                               int16_t limit_lower,
                               int16_t limit_upper);
/// Enable callbacks based on the ADC exceeding specified lower limit.
void saadc_enable_lower_limit_event(uint8_t input_channel, int16_t limit_lower);

/// Enable callbacks based on the ADC exceeding specified upper limit.
void saadc_enable_upper_limit_event(uint8_t input_channel, int16_t limit_upper);

/// Disable callbacks based on the ADC exceeding specified limits.
void saadc_disable_limit_event(uint8_t input_channel);

struct saadc_limits_t
{
    int16_t lower;
    int16_t upper;
};

/**
 * Determine the input channel limits.
 * This function is useful when processing the saadc_event_limits_exceeded event.
 *
 * @param input_channel          The input channel in question.
 * @return struct saadc_limits_t The lower, upper limits associated with the
 *                               input channel.
 */
struct saadc_limits_t saadc_get_channel_limits(uint8_t input_channel);

/**
 * Determine whether an SAADC conversion is in progress.
 * @return bool true if a conversion is in progress; false if not.
 */
bool saadc_conversion_in_progress(void);

#ifdef __cplusplus
}
#endif
