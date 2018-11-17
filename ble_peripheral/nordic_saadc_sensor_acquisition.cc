/**
 * @file nordic_saadc_sensor_acquisition.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "nordic_saadc_sensor_acquisition.h"
#include "ble/service/adc_sensor_service.h"
#include "nordic/peripherals/saadc.h"

#include "timer.h"
#include "timer_observer.h"

#include "logger.h"
#include "project_assert.h"

namespace nordic
{

void saadc_sensor_acquisition::init()
{
    // Use a higher than normal priority here. This is might
    // be a high sample rate and we don't want to drop samples.
    uint8_t const irq_priority = 5u;
    ::saadc_init(saadc_conversion_resolution_12_bit,
                 saadc_event_handler,
                 this,
                 irq_priority);

    ::saadc_input_configure_single_ended(0u,
                                         saadc_input_AIN0,
                                         saadc_input_termination_none,
                                         saadc_gain_div_6,
                                         saadc_reference_600mV,
                                         saadc_tacq_40_usec);

    ::saadc_input_configure_single_ended(1u,
                                         saadc_input_AIN1,
                                         saadc_input_termination_none,
                                         saadc_gain_div_6,
                                         saadc_reference_600mV,
                                         saadc_tacq_40_usec);

    // Attach exclusively so that the events triggered by the timer comparator
    // (CC) register are only used by the saadc_sensor_acquisition class.
    // This reserves the cc_index exclusively for our use until
    // detach_exclusive() is called. We can still call detach() and keep the
    // channel exclusively owned.
    timer_observable<>::cc_index_t const cc_index =
        this->timer_observable_.attach_exclusive(this->saadc_sample_timer_);

    // Fail hard if we could not reserved a timer index.
    ASSERT(cc_index != timer_observable<>::cc_index_unassigned);

    // Initially detach so that timer interrupts are disabled.
    // The channel index is still reserved for us.
    this->timer_observable_.detach(this->saadc_sample_timer_);

    this->saadc_trigger_event_ = this->timer_observable_.cc_get_event(cc_index);
}

void saadc_sensor_acquisition::conversion_start()
{
    logger& logger = logger::instance();

    // The saadc_sample_timer_ is not expected to be attached, but check it.
    if (not this->saadc_sample_timer_.is_attached())
    {
        this->timer_observable_.attach(this->saadc_sample_timer_);
    }
    else
    {
        logger.warn("conversion_start: saadc_sample_timer_ already attached");
    }

    sample_buffer& sample_buffer = this->next_sample_buffer();
    logger.debug("conversion_start: buffer: 0x%p, index: %u",
                 sample_buffer.data(), this->sample_buffer_bank_index);

    ::saadc_conversion_start(sample_buffer.data(),
                             sample_buffer.size(),
                             this->saadc_trigger_event_);
}

void saadc_sensor_acquisition::conversion_stop()
{
    if (this->saadc_sample_timer_.is_attached())
    {
        this->timer_observable_.detach(this->saadc_sample_timer_);
    }
    else
    {
        logger::instance().warn("conversion_stop: saadc_sample_timer_ not attached");
    }

    ::saadc_conversion_stop();
}

void saadc_sensor_acquisition::saadc_conversion_started()
{
    logger& logger = logger::instance();

    // Queue the next buffer for SAADC conversions.
    sample_buffer& sample_buffer = this->next_sample_buffer();

    logger.debug("saadc_conversion_started: buffer: 0x%p, len: %u, index: %u",
                 sample_buffer.data(), sample_buffer.size(),
                 this->sample_buffer_bank_index);

    ::saadc_queue_conversion_buffer(sample_buffer.data(), sample_buffer.size());
}

void saadc_sensor_acquisition::saadc_conversion_complete(int16_t const* sample_data,
                                                         uint16_t       sample_count)
{
    logger& logger = logger::instance();

    logger.debug("SAADC event: conversion complete: 0x%p, %d samples",
                 sample_data, sample_count);

    this->adc_samples_characterisitc_.sample_conversion_complete(sample_data, sample_count);
}

size_t saadc_sensor_acquisition::sample_bank_increment(size_t index) const
{
    index += 1u;
    return (index >= sample_buffer_depth) ? 0u : index;
}

saadc_sensor_acquisition::sample_buffer& saadc_sensor_acquisition::next_sample_buffer()
{
    this->sample_buffer_bank_index = this->sample_bank_increment(this->sample_buffer_bank_index);
    return sample_buffer_banks[this->sample_buffer_bank_index];
}

void saadc_sensor_acquisition::saadc_event_handler(
    saadc_event_type_t              event_type,
    union saadc_event_info_t const* event_info,
    void*                           context)
{
    logger &logger = logger::instance();

    saadc_sensor_acquisition* saadc_sensor_acq =
        reinterpret_cast<saadc_sensor_acquisition *>(context);

    switch (event_type)
    {
    case saadc_event_conversion_started:
        saadc_sensor_acq->saadc_conversion_started();
        break;
    case saadc_event_conversion_stop:
        logger.debug("SAADC event: conversion stop: 0x%p, %d samples",
                     event_info->conversion.data, event_info->conversion.length);
        break;
    case saadc_event_conversion_complete:
        {
            ASSERT(saadc_sensor_acq);
            saadc_sensor_acq->saadc_conversion_complete(
                event_info->conversion.data, event_info->conversion.length);
        }
        break;
    case saadc_event_limit_lower:
        {
            struct saadc_limits_t const limits =
                saadc_get_channel_limits(event_info->limits_exceeded.input_channel);
            logger.info("SAADC event: chan: %d, lower limit %u 0x%x exceeded",
                        event_info->limits_exceeded.input_channel, limits.lower, limits.lower);
        }
        break;
    case saadc_event_limit_upper:
        {
            struct saadc_limits_t const limits =
                saadc_get_channel_limits(event_info->limits_exceeded.input_channel);
            logger.info("SAADC event: chan: %d, upper limit %u 0x%x exceeded",
                        event_info->limits_exceeded.input_channel, limits.upper, limits.upper);
        }
        break;
    case saddc_event_calibration_complete:
        logger.info("SAADC event: calibration complete");
        break;
    default:
        ASSERT(0);
        break;
    }

}

void saadc_sample_timer::expiration_notify()
{
    struct saadc_conversion_info_t const conversion = saadc_conversion_info();

    logger &logger = logger::instance();
    logger.debug("SAADC start: channel_count: %u, time: %u usec",
                 conversion.channel_count, conversion.time_usec);
}

} // namespace nordic
