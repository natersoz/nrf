/**
 * @file adc_sensor_service.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include "ble/gatt_service.h"
#include "ble/profile_connectable.h"
#include "ble/gatt_descriptors.h"
#include "ble/gatt_enum_types.h"
#include "ble/service/custom_uuid.h"
#include "logger.h"

#include <array>

namespace ble
{
namespace service
{
namespace custom
{

template<typename sample_type, size_t channel_count>
struct adc_samples_characteristic;

template<typename sample_type>
class adc_sensor_acquisition
{
public:
    virtual ~adc_sensor_acquisition()                                = default;

    adc_sensor_acquisition()                                         = default;
    adc_sensor_acquisition(adc_sensor_acquisition const&)            = delete;
    adc_sensor_acquisition(adc_sensor_acquisition &&)                = delete;
    adc_sensor_acquisition& operator=(adc_sensor_acquisition const&) = delete;
    adc_sensor_acquisition& operator=(adc_sensor_acquisition&&)      = delete;

    virtual void init() = 0;
    virtual void conversion_start() = 0;
    virtual void conversion_stop() = 0;
};

template<typename sample_type, size_t channel_count>
class adc_samples_cccd: public gatt::cccd
{
public:
    adc_samples_cccd(adc_samples_characteristic<sample_type, channel_count>& chr):
        gatt::cccd(chr),
        characteristic_(chr)
    {
    }

    virtual ble::att::length_t write(ble::att::op_code    write_type,
                                     ble::att::length_t   offset,
                                     ble::att::length_t   length,
                                     void const*          data)
    {
        bool const notif_prev   = this->notifications_enabled();
        bool const indic_prev   = this->indications_enabled();
        bool const enabled_prev = notif_prev || indic_prev;

         ble::att::length_t const n_write =
             super::write(write_type, offset, length, data);

        bool const notif_curr   = this->notifications_enabled();
        bool const indic_curr   = this->indications_enabled();
        bool const enabled_curr = notif_curr || indic_curr;

        if (this->characteristic_.adc_sensor_acq())
        {
            if (enabled_prev && not enabled_curr)
            {
                this->characteristic_.adc_sensor_acq()->conversion_stop();
            }
            else if (not enabled_prev && enabled_curr)
            {
                this->characteristic_.adc_sensor_acq()->conversion_start();
            }
            else
            {
                // Enable status unchanged.
            }
        }

        return n_write;
    }

private:
    using super = gatt::cccd;
    adc_samples_characteristic<sample_type, channel_count>& characteristic_;
};

/**
 * @class adc_sample_characteristic
 * A custom characteristic for emitting 16-bit ADC sample data.
 *
 * @note
 * We are relying on the implementation of the ble_gatts_operations::notify()
 * and  ble_gatts_operations::indicate() functions to update the data_ member
 * when notificaitons are made. The Nordic stack takes care of this.
 * This may be an issue for other stack implementations.
 *
 * @tparam sample_type   The data type into which ADC samples are converted.
 *                       These could be: uint8_t, int8_t, uint16_t, int16_t,
 *                       uint32_t, int32_t.
 * @tparam channel_count The number of ADC channels which may be simultaneously
 *                       converted.
 */
template<typename sample_type, size_t channel_count>
struct adc_samples_characteristic: public gatt::characteristic
{
public:
    using value_type = sample_type;

    virtual ~adc_samples_characteristic() override                            = default;

    adc_samples_characteristic(adc_samples_characteristic const&)             = delete;
    adc_samples_characteristic(adc_samples_characteristic &&)                 = delete;
    adc_samples_characteristic& operator=(adc_samples_characteristic const&)  = delete;
    adc_samples_characteristic& operator=(adc_samples_characteristic&&)       = delete;

    adc_samples_characteristic() :
        gatt::characteristic(
            uuid_characteristic(custom::services::adc_sensor,
                                custom::characteristics::adc_samples),
            gatt::properties::read | gatt::properties::notify),
        cccd(*this),
        adc_sensor_acq_(nullptr)
    {
        this->descriptor_add(cccd);
        this->data_.fill(0);
    }

    virtual void const*   data_pointer() const { return this->data_.data(); }
    virtual att::length_t data_length()  const {
        return this->data_.size() * sizeof(sample_type);
    }

    /**
     * Associate an implementation of the adc_sensor_acquisition class wit this
     * characteristic. This is not applied within the ctor due to mutual
     * co-dependecies: adc_samples_characteristic and the adc_sensor_acquisition
     * implementation. So the association is made once both are constructed.
     *
     * @param adc_senor_acq The adc_sensor_acquisition implementation class.
     * This class ties the ADC sampling operation together with this BLE
     * characteristic and its cccd.
     */
    void set_adc_sensor_acq(adc_sensor_acquisition<sample_type> &adc_senor_acq)
    {
        this->adc_sensor_acq_ = &adc_senor_acq;
    }

    adc_sensor_acquisition<sample_type> *adc_sensor_acq() {
        return this->adc_sensor_acq_;
    }

    adc_sensor_acquisition<sample_type> const* adc_sensor_acq() const {
        return this->adc_sensor_acq_;
    }

    void sample_conversion_complete(sample_type const*  adc_samples,
                                    uint16_t            adc_samples_length)
    {
        ble::gatt::service* service = this->service();
        if (service)
        {
            ble::profile::connectable* connectable = service->connectable();
            if (connectable)
            {
                if (this->cccd.notifications_enabled())
                {
                    logger::instance().debug(
                        "notify: c: 0x%04x, h: 0x%04x, data: 0x%p, len: %u",
                        connectable->connection().get_connection_handle(),
                        this->value_handle,
                        adc_samples,
                        adc_samples_length * sizeof(sample_type));

                    att::length_t const length = connectable->gatts()->notify(
                        connectable->connection().get_connection_handle(),
                        this->value_handle,
                        0u,
                        adc_samples_length * sizeof(sample_type),
                        adc_samples);

                    logger::instance().debug("notified length: %u", length);
                }
            }
        }
        /// @todo When the notification data format is complete the RTC would
        /// provide a decent timestamp. It can be read asynchronously.
        /// (The TIMER cannot be read asynchronously).
        /// uint32_t const ticks = rtc_1.get_count_extend_32();
        /// uint32_t const conversion_usec = (conversion_ticks * 1000000u) / rtc_1.ticks_per_second();
    }

    adc_samples_cccd<sample_type, channel_count> cccd;

private:
    adc_sensor_acquisition<sample_type>*    adc_sensor_acq_;
    std::array<sample_type, channel_count>  data_;
};

/**
 * @class adc_enable_characteristic
 * A custom characteristic for enabling/disabling ADC sample data.
 *
 * @tparam channel_count The maximum number of channels of the ADC can acquire.
 * This value should match the value used in adc_samples_characteristic.
 */
template<size_t channel_count>
struct adc_enable_characteristic: public gatt::characteristic
{
public:
    virtual ~adc_enable_characteristic() override                           = default;

    adc_enable_characteristic(adc_enable_characteristic const&)             = delete;
    adc_enable_characteristic(adc_enable_characteristic &&)                 = delete;
    adc_enable_characteristic& operator=(adc_enable_characteristic const&)  = delete;
    adc_enable_characteristic& operator=(adc_enable_characteristic&&)       = delete;

    adc_enable_characteristic():
        gatt::characteristic(uuid_characteristic(custom::services::adc_sensor,
                                                 custom::characteristics::adc_enable),
            gatt::properties::read_write)
    {
        for (auto &iter : this->sample_enable) { iter = false; }
    }

    virtual void const* data_pointer() const override {
        return this->sample_enable.data();
    }

    virtual att::length_t data_length() const override {
        return channel_count * sizeof(bool);
    }

    std::array<bool, channel_count> sample_enable;
};

class adc_sensor_service: public gatt::service
{
public:
    virtual ~adc_sensor_service() override                      = default;

    adc_sensor_service(adc_sensor_service const&)               = delete;
    adc_sensor_service(adc_sensor_service &&)                   = delete;
    adc_sensor_service& operator=(adc_sensor_service const&)    = delete;
    adc_sensor_service& operator=(adc_sensor_service&&)         = delete;

    /** Construct a ADC Sensor Service as a primary service. */
    adc_sensor_service():
        gatt::service(uuid_service(custom::services::adc_sensor),
                      gatt::attribute_type::primary_service)
    {
    }

    /**
     * Construct an ADC Sensor Service as a primary or secondary service.
     *
     * @param attr_type Specify whether the ADC Sensor Service is a
     * primary or secondary service.
     */
    adc_sensor_service(gatt::attribute_type attr_type):
        service(custom::uuid_service(custom::services::adc_sensor), attr_type)
    {
    }
};

} // namespace custom
} // namespace service
} // namespace ble

