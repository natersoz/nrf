/**
 * @file current_time_service.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * @see https://www.bluetooth.com/specifications/gatt
 */

#pragma once

#include "ble/gatt_service.h"
#include "ble/gatt_descriptors.h"
#include "ble/gatt_uuids.h"

namespace ble
{
namespace service
{

struct gregorian
{
    static constexpr uint8_t const month_unknown = 0u;
    static constexpr uint8_t const day_unknown   = 0u;

    ~gregorian() = default;
    gregorian(): year(0u), month(0u), day(0u), hours(0u), minutes(0u), seconds(0u)
    {
    }

    gregorian(uint16_t _year,
              uint8_t  _month,
              uint8_t  _day,
              uint8_t  _hours,
              uint8_t  _minutes,
              uint8_t  _seconds):
        year(_year),   month(_month),     day(_day),
        hours(_hours), minutes(_minutes), seconds(_seconds)
    {
    }

    uint16_t year;              ///< [1582:9999]
    uint8_t  month;             ///< [1:12], 0: unknown
    uint8_t  day;               ///< [1:31], 0: unknown
    uint8_t  hours;             ///< [0:23]
    uint8_t  minutes;           ///< [0:59]
    uint8_t  seconds;           ///< [0:59]
};

enum class day_of_week: uint8_t
{
    unknown     = 0,
    monday      = 1,
    tuesday     = 2,
    wednesday   = 3,
    thursday    = 4,
    friday      = 5,
    saturday    = 6,
    sunday      = 7,
};

/**
 * @class date_time
 * @see https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.date_time.xml
 */
struct date_time: public gatt::characteristic
{
    virtual ~date_time() override             = default;

    date_time(date_time const&)               = delete;
    date_time(date_time &&)                   = delete;
    date_time& operator=(date_time const&)    = delete;
    date_time& operator=(date_time&&)         = delete;

    /**
     * @note The write permission is optional. For now turn it on for testing.
     */
    date_time() :
        gatt::characteristic(gatt::characteristics::date_time,
                             gatt::properties::read  |
                             gatt::properties::write |
                             gatt::properties::notify),
        cccd(*this)
    {
        this->descriptor_add(this->cccd);
    }

    virtual void const* data_pointer() const override {
        return &this->greg_date;
    }

    virtual att::length_t data_length() const override {
        return sizeof(this->greg_date);
    }

    gatt::client_characteristic_configuration_descriptor cccd;

    gregorian greg_date;
};

/**
 * @struct day_date_time
 * https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.day_of_week.xml
 */
struct day_date_time: public gatt::characteristic
{
    virtual ~day_date_time() override               = default;

    day_date_time(day_date_time const&)             = delete;
    day_date_time(day_date_time &&)                 = delete;
    day_date_time& operator=(day_date_time const&)  = delete;
    day_date_time& operator=(day_date_time&&)       = delete;

    day_date_time() :
        gatt::characteristic(gatt::characteristics::date_time,
                             gatt::properties::read  |
                             gatt::properties::write |
                             gatt::properties::notify),
        cccd(*this),
        week_day(day_of_week::unknown)
    {
        this->descriptor_add(this->cccd);
    }

    virtual void const* data_pointer() const override {
        return &this->greg_date;
    }

    virtual att::length_t data_length() const override {
        return sizeof(this->greg_date) + sizeof(this->week_day);
    }

    gatt::client_characteristic_configuration_descriptor cccd;

    gregorian   greg_date;
    day_of_week week_day;
};

struct exact_time_256: public gatt::characteristic
{
    virtual ~exact_time_256() override                = default;

    exact_time_256(exact_time_256 const&)             = delete;
    exact_time_256(exact_time_256 &&)                 = delete;
    exact_time_256& operator=(exact_time_256 const&)  = delete;
    exact_time_256& operator=(exact_time_256&&)       = delete;

    exact_time_256() :
        gatt::characteristic(gatt::characteristics::date_time,
                             gatt::properties::read  |
                             gatt::properties::write |
                             gatt::properties::notify),
        cccd(*this),
        week_day(day_of_week::unknown),
        seconds_fraction_256(0u)
   {
       this->descriptor_add(this->cccd);
   }

    virtual void const* data_pointer() const override {
        return &this->greg_date;
    }

    virtual att::length_t data_length() const override {
        return sizeof(this->greg_date) +
               sizeof(this->week_day) +
               sizeof(this->seconds_fraction_256);
    }

    gatt::client_characteristic_configuration_descriptor cccd;

    gregorian   greg_date;
    day_of_week week_day;
    uint8_t     seconds_fraction_256;
};

/**
 * @struct current_time
 * https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.current_time.xml
 */
struct current_time: public gatt::characteristic
{
    enum class adjust_reason: uint8_t
    {
        none        = 0u,
        manual      = (1u << 0u),
        external    = (1u << 1u),
        time_zone   = (1u << 2u),
        dst         = (1u << 3u),
    };

    virtual ~current_time() override               = default;

    current_time(current_time const&)             = delete;
    current_time(current_time &&)                 = delete;
    current_time& operator=(current_time const&)  = delete;
    current_time& operator=(current_time&&)       = delete;

    /**
     * @note The write permission is optional. For now turn it on for testing.
     */
    current_time() :
        gatt::characteristic(gatt::characteristics::date_time,
                             gatt::properties::read  |
                             gatt::properties::write |
                             gatt::properties::notify),
        cccd(*this),
        week_day(day_of_week::unknown),
        seconds_fraction_256(0u),
        adjust_reason(adjust_reason::none)
     {
         this->descriptor_add(this->cccd);
     }

    virtual void const* data_pointer() const override {
        return &this->greg_date;
    }

    virtual att::length_t data_length() const override {
        return sizeof(this->greg_date) +
               sizeof(this->week_day) +
               sizeof(this->seconds_fraction_256) +
               sizeof(this->adjust_reason);
    }

    gatt::client_characteristic_configuration_descriptor cccd;

    gregorian       greg_date;
    day_of_week     week_day;
    uint8_t         seconds_fraction_256;
    adjust_reason   adjust_reason;
};

/**
 * @struct time_zone
 * https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.time_zone.xml
 */
struct time_zone: public gatt::characteristic
{
    virtual ~time_zone() override             = default;

    time_zone(time_zone const&)               = delete;
    time_zone(time_zone &&)                   = delete;
    time_zone& operator=(time_zone const&)    = delete;
    time_zone& operator=(time_zone&&)         = delete;

    /**
     * @note The write permission is optional. For now turn it on for testing.
     */
    time_zone() :
        gatt::characteristic(gatt::characteristics::time_zone,
                             gatt::properties::read  |
                             gatt::properties::write |
                             gatt::properties::notify),
        tz_offset(0)
    {
    }

    virtual void const* data_pointer() const override {
        return &this->tz_offset;
    }

    virtual att::length_t data_length() const override {
        return sizeof(this->tz_offset);
    }

    int8_t tz_offset;
};

/**
 * @struct dst_offset
 * https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.dst_offset.xml
 */
struct dst_offset: public gatt::characteristic
{
    enum class offset: uint8_t
    {
        standard_time           = 0,
        daylight_time_half      = 2,
        daylight_time           = 4,
        daylight_time_double    = 8,
    };

    virtual ~dst_offset() override              = default;

    dst_offset(dst_offset const&)               = delete;
    dst_offset(dst_offset &&)                   = delete;
    dst_offset& operator=(dst_offset const&)    = delete;
    dst_offset& operator=(dst_offset&&)         = delete;

    /**
     * @note The write permission is optional. For now turn it on for testing.
     */
    dst_offset(offset _offset = offset::standard_time) :
        gatt::characteristic(gatt::characteristics::dst_offset,
                             gatt::properties::read  |
                             gatt::properties::write |
                             gatt::properties::notify),
        offset(_offset)
    {
    }

    virtual void const* data_pointer() const override { return &this->offset; }

    virtual att::length_t data_length() const override {
        return sizeof(this->offset);
    }

    offset offset;
};

struct time_source: public gatt::characteristic
{
    enum class source: uint8_t
    {
        unknown     = 0u,
        ntp         = 1u,
        gps         = 2u,
        radio       = 3u,
        manual      = 4u,
        atomic      = 5u,
        cell        = 6u,
    };

    virtual ~time_source() override               = default;

    time_source(time_source const&)               = delete;
    time_source(time_source &&)                   = delete;
    time_source& operator=(time_source const&)    = delete;
    time_source& operator=(time_source&&)         = delete;

    /**
     * @note The write permission is optional. For now turn it on for testing.
     */
    time_source() :
        gatt::characteristic(gatt::characteristics::time_source,
                             gatt::properties::read  |
                             gatt::properties::write |
                             gatt::properties::notify),
        source(source::unknown)
    {
    }

    virtual void const* data_pointer() const override { return &this->source; }

    virtual att::length_t data_length() const override {
        return sizeof(this->source);
    }

    source source;
};

/**
 *
 * https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.characteristic.reference_time_information.xml
 */
struct time_accuracy: public gatt::characteristic
{
    static constexpr uint8_t const out_of_range = 254u;
    static constexpr uint8_t const unknown      = 255u;

    virtual ~time_accuracy() override                 = default;

    time_accuracy(time_accuracy const&)               = delete;
    time_accuracy(time_accuracy &&)                   = delete;
    time_accuracy& operator=(time_accuracy const&)    = delete;
    time_accuracy& operator=(time_accuracy&&)         = delete;

    /**
     * @note The write permission is optional. For now turn it on for testing.
     */
    time_accuracy() :
        gatt::characteristic(gatt::characteristics::time_accuracy,
                             gatt::properties::read  |
                             gatt::properties::write |
                             gatt::properties::notify),
        accuracy(unknown)
    {
    }

    virtual void const* data_pointer() const override {
        return &this->accuracy;
    }

    virtual att::length_t data_length() const override {
        return sizeof(this->accuracy);
    }

    uint8_t accuracy;
};

/**
 * @class current_time_service
 * https://www.bluetooth.org/docman/handlers/downloaddoc.ashx?doc_id=292957
 * https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.service.current_time.xml
 */
class current_time_service: public gatt::service
{
public:
    virtual ~current_time_service() override                      = default;

    current_time_service(current_time_service const&)             = delete;
    current_time_service(current_time_service &&)                 = delete;
    current_time_service& operator=(current_time_service const&)  = delete;
    current_time_service& operator=(current_time_service&&)       = delete;

    /**
     * Construct a Current Time Service as a primary service.
     *
     * @param attr_type Specify whether the battery service
     * is a primary or secondary service.
     */
    current_time_service():
        service(gatt::services::current_time_service,
                gatt::attribute_type::primary_service)
    {
        this->characteristic_add(this->current_time);
    }

    /**
     * Construct a Current Time Service object.
     *
     * @param attr_type Specify whether the service
     * is a primary or secondary service.
     */
    current_time_service(gatt::attribute_type attr_type):
        service(gatt::services::current_time_service, attr_type)
    {
        this->characteristic_add(this->current_time);
    }

    struct current_time current_time;
};

} // namespace service
} // namespace ble

