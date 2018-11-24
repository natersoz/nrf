/**
 * @file unit_tests/test_gregorian.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "gregorian.h"

#include <vector>
#include <iostream>
#include <iomanip>
#include <iterator>

static constexpr bool const debug_print = false;

bool test_greg(utility::gregorian const& greg)
{
    uint64_t const seconds_since_epoch = utility::gregorian::seconds_since_epoch(greg);
    utility::gregorian const greg_eq = utility::gregorian::to_calendar(seconds_since_epoch);
    utility::gregorian const greg_lt = utility::gregorian::to_calendar(seconds_since_epoch - 1u);
    utility::gregorian const greg_gt = utility::gregorian::to_calendar(seconds_since_epoch + 1u);

    bool const check_eq = (greg_eq == greg);
    bool const check_lt = (greg_lt <  greg) && (greg_lt <= greg);
    bool const check_gt = (greg_gt >  greg) && (greg_gt >= greg);

    bool const check = check_eq && check_lt && check_gt;
    if (not check)
    {
        std::cerr << greg << " ";
        std::cerr << std::setw(11) << seconds_since_epoch;

        if (not check_eq)
        {
            uint64_t const seconds_since_epoch_eq = utility::gregorian::seconds_since_epoch(greg_eq);
            std::cerr << ", eq: " << greg_eq;
            std::cerr << std::setw(11) << seconds_since_epoch_eq;
        }

        if (not check_lt)
        {
            uint64_t const seconds_since_epoch_lt = utility::gregorian::seconds_since_epoch(greg_lt);
            std::cerr << ", lt: " << greg_lt;
            std::cerr << " " << std::setw(11) << seconds_since_epoch_lt;
        }

        if (not check_gt)
        {
            uint64_t const seconds_since_epoch_gt = utility::gregorian::seconds_since_epoch(greg_gt);
            std::cerr << ", gt: " << greg_gt;
            std::cerr << " " << std::setw(11) << seconds_since_epoch_gt;
        }

        std::cerr << " " << (check? "PASS" : "FAIL");
        std::cerr << std::endl;
    }

    return check;
}

/**
 * Test equality over a very fine step.
 * From 1601 to 2501 in 1 secnod increments takes 30 minutes on a macbook pro.
 *
 * @param seconds_increment step size for testing in seconds.
 *
 * @return bool
 */
bool test_in_steps(utility::gregorian const& greg_begin,
                   utility::gregorian const& greg_end,
                   uint32_t seconds_increment)
{
    uint16_t status_year = 0u;

    uint64_t const seconds_begin = utility::gregorian::seconds_since_epoch(greg_begin);
    uint64_t const seconds_end   = utility::gregorian::seconds_since_epoch(greg_end);

    for (uint64_t seconds = seconds_begin; seconds < seconds_end;
         seconds += seconds_increment)
    {
        utility::gregorian const greg = utility::gregorian(seconds);

        if (not test_greg(greg)) { return false; }

        if (debug_print && (greg.year % 100u == 0u) && (greg.year != status_year))
        {
            std::cout << "update: " << greg << std::endl;
            status_year = greg.year;
        }
    }

    return true;
}

int main()
{
    std::vector<utility::gregorian> greg_test;

    greg_test.push_back(utility::gregorian{1602,  1,  1,  0,  0,  0});

    greg_test.push_back(utility::gregorian{1999, 12, 31, 23, 59, 59});
    greg_test.push_back(utility::gregorian{2000, 12, 31, 23, 59, 59});
    greg_test.push_back(utility::gregorian{2001,  1, 31, 23, 59, 59});
    greg_test.push_back(utility::gregorian{2001,  1,  1,  0,  0,  0});
    greg_test.push_back(utility::gregorian{2001,  2,  2,  0,  0,  0});

    greg_test.push_back(utility::gregorian{1604, 12, 31, 23, 59, 59});
    greg_test.push_back(utility::gregorian{1605,  1,  0,  0,  0,  0});
    greg_test.push_back(utility::gregorian{1605,  1,  0,  0,  0,  1});

    greg_test.push_back(utility::gregorian{1899, 12, 31, 23, 59, 59});
    greg_test.push_back(utility::gregorian{1900,  1,  0,  0,  0,  0});
    greg_test.push_back(utility::gregorian{1900,  1,  0,  0,  0,  1});

    greg_test.push_back(utility::gregorian{1999, 12, 31, 23, 59, 59});
    greg_test.push_back(utility::gregorian{2000,  1,  0,  0,  0,  0});
    greg_test.push_back(utility::gregorian{2000,  1,  0,  0,  0,  1});

    greg_test.push_back(utility::gregorian{2000, 12, 31, 23, 59, 59});
    greg_test.push_back(utility::gregorian{2001,  1,  0,  0,  0,  0});
    greg_test.push_back(utility::gregorian{2001,  1,  0,  0,  0,  1});

    greg_test.push_back(utility::gregorian{2001, 12, 31, 23, 59, 59});
    greg_test.push_back(utility::gregorian{2002,  1,  0,  0,  0,  0});
    greg_test.push_back(utility::gregorian{2002,  1,  0,  0,  0,  1});

    greg_test.push_back(utility::gregorian{1950,  3,  3,  0,  0,  0});
    greg_test.push_back(utility::gregorian{1950,  3,  0,  0,  0,  0});
    greg_test.push_back(utility::gregorian{1970,  4,  14, 7, 30, 28});

    greg_test.push_back(utility::gregorian{1633,  3, 21,  7, 30, 28});
    greg_test.push_back(utility::gregorian{1643, 11, 23,  7, 30, 28});
    greg_test.push_back(utility::gregorian{1681,  6, 30,  7, 30, 28});
    greg_test.push_back(utility::gregorian{1693,  4, 18,  7, 30, 28});
    greg_test.push_back(utility::gregorian{1812,  1, 24,  7, 30, 28});
    greg_test.push_back(utility::gregorian{1817,  5,  8,  7, 30, 28});
    greg_test.push_back(utility::gregorian{1817,  8, 24,  7, 30, 28});
    greg_test.push_back(utility::gregorian{1822,  8,  3,  7, 30, 28});
    greg_test.push_back(utility::gregorian{1873,  9,  3,  7, 30, 28});
    greg_test.push_back(utility::gregorian{1899,  7, 23,  7, 30, 28});
    greg_test.push_back(utility::gregorian{2043,  3,  2,  7, 30, 28});
    greg_test.push_back(utility::gregorian{2055, 12, 31,  7, 30, 28});
    greg_test.push_back(utility::gregorian{2174,  7, 17,  7, 30, 28});
    greg_test.push_back(utility::gregorian{2190, 11, 29,  7, 30, 28});
    greg_test.push_back(utility::gregorian{2260, 10,  3,  7, 30, 28});
    greg_test.push_back(utility::gregorian{2322, 12, 21,  7, 30, 28});
    greg_test.push_back(utility::gregorian{2344,  9,  9,  7, 30, 28});
    greg_test.push_back(utility::gregorian{2382,  2, 28,  7, 30, 28});
    greg_test.push_back(utility::gregorian{2392, 10, 22,  7, 30, 28});
    greg_test.push_back(utility::gregorian{2404,  2, 28,  7, 30, 28});
    greg_test.push_back(utility::gregorian{2426, 10, 14,  7, 30, 28});
    greg_test.push_back(utility::gregorian{2437, 11,  7,  7, 30, 28});
    greg_test.push_back(utility::gregorian{2464, 11, 29,  7, 30, 28});
    greg_test.push_back(utility::gregorian{2486,  3, 28,  7, 30, 28});
    greg_test.push_back(utility::gregorian{2504,  6,  2,  7, 30, 28});

    bool check = true;
    for (utility::gregorian const& greg: greg_test)
    {
        check = test_greg(greg) && check;
    }

    utility::gregorian greg_begin(1601u, utility::gregorian::January, 1, 0, 0, 2);
    utility::gregorian greg_end(  2701u, utility::gregorian::October, 1);
    bool const check_2 = test_in_steps(greg_begin, greg_end, 7253u);

    check = check && check_2;

    std::cerr << (check ? "-- PASS --" : "-- FAIL --") << std::endl;
    return check ? 0 : -1;
}
