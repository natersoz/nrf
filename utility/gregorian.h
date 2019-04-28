/**
 * @file gregorian.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once
#include <cstdint>
#include <cstddef>

#include <ostream>
#include <boost/operators.hpp>

namespace utility
{

/**
 * @struct gregorian
 * A calendar date/time C++ structure using an epoch of Jan 1, 1601.
 * This struct is designed to be used within the BLE current time service.
 * All fields are comformant except for the year field which here starts
 * at 1601 and the BLE uses 1582.
 *
 * Inheriting from boost less_than_comparable<> brings operators <=, >, >=,
 * along for the ride when < and == are defined.
 */
struct gregorian : public boost::less_than_comparable<gregorian>
{
    enum month : uint8_t
    {
        January = 1u,
        February,
        March,
        April,
        May,
        June,
        July,
        August,
        September,
        October,
        November,
        December
    };

    /**
     * The day of the week as defined by the BLE current_time_service.
     * monday is '1'.
     */
    enum day_of_week: uint8_t
    {
        dow_invalid = 0u,
        monday,
        tuesday,
        wednesday,
        thursday,
        friday,
        saturday,
        sunday,
        days_per_week
    };

    static constexpr uint16_t const epoch_year              = 1601u;
    static constexpr uint8_t  const months_per_year         =   12u;
    static constexpr uint16_t const days_per_non_leap_year  =  365u;

    static constexpr uint8_t  const hours_per_day           =   24u;
    static constexpr uint8_t  const minutes_per_hour        =   60u;
    static constexpr uint8_t  const seconds_per_minute      =   60u;

    /// Jan 1, 1601 Gregorian day of the week is a Monday.
    static constexpr day_of_week const epoch_day_of_week    = monday;

    /// To contain the date/time format "2007-04-05T14:30:00"
    /// requires 20 characters, including the null terminator.
    static constexpr std::size_t const char_buffer_size     = 20u;

    ~gregorian() = default;

    gregorian(gregorian const&)             = default;
    gregorian(gregorian &&)                 = default;
    gregorian& operator=(gregorian const&)  = default;
    gregorian& operator=(gregorian&&)       = default;

    /**
     * The default ctor contains values which are completely invalid.
     * Attempting to convert this to seconds since epoch will throw an
     * exception.
     */
    gregorian()
    : year(0u), month(0u), day_of_month(0u),
      hours(0u), minutes(0u), seconds(0u)
    {
    }

    /**
     * Create a gregorian date/time entry based on calendar date.
     * The default time values are for 00:00:00 the start of the
     * date specified.
     */
    gregorian(uint16_t _year,
              uint8_t  _month,
              uint8_t  _day_of_month,
              uint8_t  _hours   = 0u,
              uint8_t  _minutes = 0u,
              uint8_t  _seconds = 0u)
    : year(_year),
      month(_month),
      day_of_month(_day_of_month),
      hours(_hours),
      minutes(_minutes),
      seconds(_seconds)
    {
    }

    /**
     * Create a gregorian date/time entry based the number of seconds since
     * the gregorian::epoch.
     */
    explicit gregorian(uint64_t seconds_since_epoch)
    {
        *this = to_calendar(seconds_since_epoch);
    }

    uint16_t year;              ///< [1601:UINT16_MAX]
    uint8_t  month;             ///< [1:12],
    uint8_t  day_of_month;      ///< [1:31],
    uint8_t  hours;             ///< [0:23]
    uint8_t  minutes;           ///< [0:59]
    uint8_t  seconds;           ///< [0:59]

    /**
     * Convert from the number of seconds since the epoch to the gregorian
     * calendar date/time struct.
     *
     * @param seconds_since_epoch The number of seconds since gregorian::epoch.
     * @return gregorian          The gregorian date/time struct.
     */
    static gregorian to_calendar(uint64_t seconds_since_epoch);

    /**
     * Create a character string with format: "2007-04-05T14:30:00".
     * The correct size of the conversion buffer is char_buffer_size.
     */
    static void to_chars(char *first, char *last, gregorian const& greg);

    /**
     * Convert from the gregorian date/time struct to the number of days
     * since the gregorian::epoch.
     *
     * @param greg      The gregorian date/time struct.
     * @return uint64_t The number of seconds since the gregorian::epoch.
     *                  The date 1601-01-01 returns zero since it is the epoch.
     */
    static uint32_t days_since_epoch(gregorian const &greg);

    /**
     * Convert from the gregorian date/time struct to the number of seconds
     * since the gregorian::epoch.
     *
     * @param greg      The gregorian date/time struct.
     * @return uint64_t The number of seconds since the gregorian::epoch.
     */
    static uint64_t seconds_since_epoch(gregorian const &greg);

    /**
     * Determine whether a year is a leap year.
     *
     * @param year  The year in question.
     * @return bool true if the year is a leap year; false if it is not.
     */
    static bool is_leap_year(uint16_t year);

    /**
     * Determine the number of days in a month.
     *
     * @param month The month in question [0:11] or enum gregorian::month.
     * @param year  The year; required to determine February's leap day.
     * @return uint8_t The number of days in the month.
     */
    static uint8_t days_in_month(uint8_t month, uint16_t year);

    /**
     * Determine the number of leap years since the gregorian::epoch.
     *
     * @param year      The year in question.
     * @return uint16_t The number of leap years since the gregorian::epoch.
     */
    static uint16_t leap_years_since_epoch(uint16_t year);

    /**
     * Given a year, month, day_of_month calculate the day within the year.
     * @return uint8_t The day within the year. Jan 1 has value zero.
     */
    static uint16_t calc_day_of_year(gregorian const &greg);

    /// @return The day of the week based on the gregorian calendar date.
    static day_of_week calc_day_of_week(gregorian const &greg);

    /**
     * @retval true  The fields within the gregorian date/time struct
     *               are within the acceptable range limits.
     * @retval false The fields within the gregorian date/time struct are
     *               out of bounds.
     */
    static bool is_valid(gregorian const &greg);

private:
    static constexpr uint32_t const seconds_per_hour = minutes_per_hour * seconds_per_minute;
    static constexpr uint32_t const seconds_per_day  = hours_per_day    * seconds_per_hour;

    /// There are 97 leap years in each span of 400 years:
    /// + 100 years are divisible by 4:        +100
    /// + but 4 of those are divisible by 100:   -4
    /// + and one of them is divisible by 400:   +1
    static constexpr uint16_t const leaps_per_400_years = (400u / 4u) - (400u / 100u) + 1u;
    static_assert(leaps_per_400_years == 97u);

    /// In any 100 year span there are 24 leap years.
    static constexpr uint16_t const leaps_per_100_years = (100u / 4u) - 1u;
    static_assert(leaps_per_100_years == 24u);

    static constexpr uint64_t const days_per_400_years    = 400u * days_per_non_leap_year + leaps_per_400_years;
    static constexpr uint64_t const seconds_per_400_years = days_per_400_years * seconds_per_day;

    static constexpr uint64_t const days_per_100_years    = 100u * days_per_non_leap_year + leaps_per_100_years;
    static constexpr uint64_t const seconds_per_100_years = days_per_100_years * seconds_per_day;

    static constexpr uint64_t const days_per_4_years      = 4u * days_per_non_leap_year + 1u;
    static constexpr uint64_t const seconds_per_4_years   = days_per_4_years * seconds_per_day;
    static constexpr uint64_t const seconds_per_1_years   = days_per_non_leap_year * seconds_per_day;
};

bool operator == (gregorian const& greg_1, gregorian const& greg_2);
bool operator != (gregorian const& greg_1, gregorian const& greg_2);
bool operator <  (gregorian const& greg_1, gregorian const& greg_2);
std::ostream& operator << (std::ostream& os, gregorian const& greg);

} // namespace utility
