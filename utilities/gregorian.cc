/**
 * @file gregorian.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "gregorian.h"
#include "logger.h"
#include "project_assert.h"

#include <stdexcept>

namespace utility
{

// Since months start counting with January = 1, due to human interpretation
// considerations, many indicies will be one higher than perhaps expected.
constexpr uint8_t const days_per_month[gregorian::months_per_year + 1u] =
{
     0u,    // Not a month
    31u,    // January
    28u,    // February     note: except in a leap year.
    30u,    // March
    30u,    // April
    31u,    // May
    30u,    // June
    31u,    // July
    31u,    // August
    31u,    // September
    31u,    // October
    30u,    // November
    31u,    // December
};

gregorian gregorian::to_calendar(uint64_t seconds_since_epoch)
{
    uint64_t span_400 = seconds_since_epoch  / seconds_per_400_years;
                        seconds_since_epoch -= seconds_per_400_years * span_400;
    uint64_t span_100 = seconds_since_epoch  / seconds_per_100_years;
             span_100 = (span_100 > 3u) ? 3u : span_100;
                        seconds_since_epoch -= seconds_per_100_years * span_100;
    uint64_t span_4   = seconds_since_epoch  / seconds_per_4_years;
             span_4   = (span_4 > 24u) ? 24u : span_4;
                        seconds_since_epoch -= seconds_per_4_years * span_4;
    uint64_t span_1   = seconds_since_epoch  / seconds_per_1_years;
             span_1   = (span_1 > 3u) ? 3u : span_1;
                        seconds_since_epoch -= seconds_per_1_years * span_1;

    uint16_t const year_span =
        span_400 * 400u +
        span_100 * 100u +
        span_4   *   4u +
        span_1   *   1u ;

    uint16_t const year         = static_cast<uint16_t>(year_span) + gregorian::epoch_year;
    uint16_t const day_of_year  = seconds_since_epoch  / seconds_per_day;
                                  seconds_since_epoch -= seconds_per_day * day_of_year;

    uint16_t day_count = day_of_year;
    for (uint8_t month_iter = January; month_iter <= December; ++month_iter)
    {
        uint16_t const days_in_month_iter = days_in_month(month_iter, year);
        if (day_count >= days_in_month_iter)
        {
            day_count -= days_in_month_iter;
        }
        else
        {
            uint8_t const hours = seconds_since_epoch / seconds_per_hour;
            seconds_since_epoch -= hours * seconds_per_hour;

            uint8_t const minutes = seconds_since_epoch / seconds_per_minute;
            seconds_since_epoch -= minutes * seconds_per_minute;

            // The day count of zero means the first day in the month.
            // Add one to day_count to normalize to human style day of month
            // interpretation.
            return gregorian(year, month_iter, day_count + 1u,
                             hours, minutes, seconds_since_epoch);
        }
    }

    // This should never happen. We have run out of days within the year.
    ASSERT(0);
    return gregorian();
}

void gregorian::to_chars(char *first, char *last, gregorian const& greg)
{
    snprintf(first, last - first, "%04u-%02u-%02uT%02u:%02u:%02u",
             greg.year,  greg.month, greg.day_of_month,
             greg.hours, greg.minutes, greg.seconds);
}

uint32_t gregorian::days_since_epoch(gregorian const &greg)
{
    // Determine the number of years and leap years since the epoch.
    uint16_t const years_since_epoch = greg.year - epoch_year;
    uint16_t const leap_year_count   = leap_years_since_epoch(greg.year);

    // Determine the number of days within the number of years since epoch.
    uint32_t day_count = 0u;
    day_count += years_since_epoch * days_per_non_leap_year;
    day_count += leap_year_count;
    day_count += calc_day_of_year(greg) - 1u;

    return day_count;
}

uint64_t gregorian::seconds_since_epoch(gregorian const &greg)
{
    uint64_t const day_count = days_since_epoch(greg);

    uint64_t const seconds = day_count      * seconds_per_day     +
                             greg.hours     * seconds_per_hour    +
                             greg.minutes   * seconds_per_minute  +
                             greg.seconds;
    return seconds;
}

bool gregorian::is_leap_year(uint16_t year)
{
    // Years which are divisible by 400 are leap years.
    if ((year % 400u) == 0u) { return true; }

    // Years which are divisible by 100 and are not divisible by 400
    // (div 400 case is handled above) are not leap years.
    if ((year % 100u) == 0u) { return false; }

    // All 100, 400 year cases have been handled;
    // Years divisible by 4 are leap years.
    if ((year %   4u) == 0u) { return true; }

    // All other years are not leap years.
    return false;
}

uint8_t gregorian::days_in_month(uint8_t month, uint16_t year)
{
    if ((month >= January) && (month <= December))
    {
        if (month == February)
        {
            uint8_t days = days_per_month[month];
            days += is_leap_year(year) ? 1u : 0u;
            return days;
        }

        return days_per_month[month];
    }

    logger::instance().error("invalid month: %u in year %u", month, year);
    return 0u;
}

uint16_t gregorian::leap_years_since_epoch(uint16_t year)
{
    if (year >= gregorian::epoch_year)
    {
        uint16_t const delta_years = year - gregorian::epoch_year;
        uint16_t const delta_400   = delta_years / 400u;
        uint16_t const delta_100   = delta_years / 100u;
        uint16_t const delta_4     = delta_years /   4u;
        uint16_t const leap_years  = delta_4 - delta_100 + delta_400;
        return leap_years;
    }

    logger::instance().error("invalid year: %u", year);
    return 0u;
}

uint16_t gregorian::calc_day_of_year(gregorian const &greg)
{
    if (not is_valid(greg))
    {
        char buffer[utility::gregorian::char_buffer_size];
        utility::gregorian::to_chars(std::begin(buffer), std::end(buffer), greg);

        logger::instance().error("invalid gregorian: %s", buffer);
        return 0u;
    }

    uint16_t day_count = 0u;
    for (uint8_t month_iter = January; month_iter < greg.month; ++month_iter)
    {
        day_count += days_in_month(month_iter, greg.year);
    }

    // Note: since 'day of year' and 'day of month' both start with '1',
    // and not '0' there is not adjustment necessary between them.
    day_count += greg.day_of_month;
    return day_count;
}

/**
 * @note A uint32_t can count > 136 years of time in seconds.
 * Therefore uint64_t will hold billions of years worth of time.
 * The upper limit on gregorian years does not need checked.
 *
 * @return bool true if the gergorian date/time struct values are valid;
 *              false if the struct gregorian fields are out of bounds.
 */
bool gregorian::is_valid(gregorian const &greg)
{
    return  (greg.year   >= greg.epoch_year)            &&
            (greg.month  >= January)                    &&
            (greg.month  <= December)                   &&
            (greg.hours   < greg.hours_per_day)         &&
            (greg.minutes < greg.minutes_per_hour)      &&
            (greg.seconds < greg.seconds_per_minute)    &&
            (greg.day_of_month >= 1u)                   &&
            (greg.day_of_month <= greg.days_in_month(greg.month, greg.year));
}

gregorian::day_of_week gregorian::calc_day_of_week(gregorian const &greg)
{
    uint32_t const day_count = days_since_epoch(greg);
    uint8_t const dow = day_count % days_per_week + monday;
    return static_cast<day_of_week>(dow);
}

bool operator == (gregorian const& greg_1, gregorian const& greg_2)
{
    return (gregorian::seconds_since_epoch(greg_1) ==
            gregorian::seconds_since_epoch(greg_2));
}

bool operator < (gregorian const& greg_1, gregorian const& greg_2)
{
    return (gregorian::seconds_since_epoch(greg_1) <
            gregorian::seconds_since_epoch(greg_2));
}

std::ostream& operator << (std::ostream& os, gregorian const& greg)
{
    char buffer[utility::gregorian::char_buffer_size];
    utility::gregorian::to_chars(std::begin(buffer), std::end(buffer), greg);

    os << buffer;
    return os;
}

} // namespace utility
