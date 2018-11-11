/**
 * @file nordic_saadc_sensor_acquisition.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "ble/service/adc_sensor_service.h"
#include "ble/service/nordic_saadc_sensor_service.h"
#include "nordic/peripherals/saadc.h"

#include "timer.h"
#include "timer_observer.h"

#include <array>

namespace nordic
{

class saadc_sample_timer: public timer_observer
{
public:
    saadc_sample_timer(uint32_t expiration_ticks) :
        timer_observer(timer_observer::expiration_type::continuous, expiration_ticks)
    {
    }

private:
    void expiration_notify() override;
};

/**
 * @todo At present we are not utilizing the depth of the
 * nordic::saadc_samples_characteristic
 * adc_samples_characteristic::sample_data[] allocation.
 */
class saadc_sensor_acquisition: public nordic::adc_sensor_acquisition
{
public:
    using value_type = nordic::saadc_samples_characteristic::value_type;

    /// Two buffers are allocated for SAADC double buffering.
    static constexpr size_t const sample_buffer_depth = 2u;

    virtual ~saadc_sensor_acquisition()                                  = default;

    saadc_sensor_acquisition()                                           = delete;
    saadc_sensor_acquisition(saadc_sensor_acquisition const&)            = delete;
    saadc_sensor_acquisition(saadc_sensor_acquisition &&)                = delete;
    saadc_sensor_acquisition& operator=(saadc_sensor_acquisition const&) = delete;
    saadc_sensor_acquisition& operator=(saadc_sensor_acquisition&&)      = delete;

    saadc_sensor_acquisition(
        nordic::saadc_samples_characteristic&   adc_samples_char,
        timer_observable<>&                     timer_observable)
        : adc_samples_characterisitc_(adc_samples_char),
          timer_observable_(timer_observable),
          saadc_sample_timer_(timer_observable.msec_to_ticks(1000u)),
          saadc_trigger_event_(nullptr)
    {
        for (sample_buffer& buffer : sample_buffer_banks) { buffer.fill(0); }
    }

    virtual void init() override;
    virtual void conversion_start() override;
    virtual void conversion_stop() override;

private:
    nordic::saadc_samples_characteristic& adc_samples_characterisitc_;

    timer_observable<>&     timer_observable_;
    saadc_sample_timer      saadc_sample_timer_;
    uint32_t volatile*      saadc_trigger_event_;

    using sample_buffer = std::array<value_type, saadc_input_channel_count>;

    std::array<sample_buffer, sample_buffer_depth> sample_buffer_banks;
    size_t                                         sample_buffer_bank_index;

    /**
     * The Nordic EVENTS_STARTED triggers this call,
     * indicating that the next conversion buffer can be queued.
     */
    void saadc_conversion_started();

    /**
     * The Nordic event EVENTS_END triggers this call,
     * indicating that the SAADC has completed converting samples.
     *
     * @param sample_data  A pointer to the data samples converted.
     * @param sample_count The number of samples converted.
     */
    void saadc_conversion_complete(int16_t const *sample_data, uint16_t sample_count);

    /// Increment the sample buffer index within the sample_buffer_banks[].
    size_t sample_bank_increment(size_t index) const;

    /// Obtain the next sample buffer for conversion.
    sample_buffer& next_sample_buffer();

    /**
     * This is the callback function which gets registered into the SAADC
     * device driver. Device driver event notifications are demultiplexed here
     * and delivered to the member class functions.
     *
     * @param event_type    @see saadc_event_type_t
     * @param event_info    @see union saadc_event_info_t
     * @param context       Casts back to this class instance pointer.
     */
    static void saadc_event_handler(saadc_event_type_t              event_type,
                                    union saadc_event_info_t const* event_info,
                                    void*                           context);
};

} // namespace nordic
