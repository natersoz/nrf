/**
 * @file unit_tests/test_gregorian.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "gtest/gtest.h"
#include "gregorian.h"
#include <vector>

/**
 * This may seem simple minded, but it is effective at finding bugs when
 * tested across a large set of data.
 */
static void test_greg(utility::gregorian const& greg)
{
    uint64_t const seconds_since_epoch = utility::gregorian::seconds_since_epoch(greg);
    utility::gregorian const greg_eq = utility::gregorian::to_calendar(seconds_since_epoch);
    utility::gregorian const greg_lt = utility::gregorian::to_calendar(seconds_since_epoch - 1u);
    utility::gregorian const greg_gt = utility::gregorian::to_calendar(seconds_since_epoch + 1u);

    // Note: We're testing the gregorian operators as well as functionality.
    // To insure the actual binary operators are being called use
    // ASSERT_TRUE() and not ASSERT_EQ(), ASSERT_LT(), etc.
    ASSERT_TRUE(greg_eq == greg);

    ASSERT_TRUE(greg_lt <  greg);
    ASSERT_TRUE(greg_lt <= greg);
    ASSERT_TRUE(greg_eq <= greg);

    ASSERT_TRUE(greg_gt >  greg);
    ASSERT_TRUE(greg_gt >= greg);
    ASSERT_TRUE(greg_eq >= greg);

    ASSERT_FALSE(greg_eq != greg);
}

/**
 * Test Gregorian comparisons over a very fine step.
 * From 1601 to 2501 in 1 secnod increments takes 30 minutes on a macbook pro.
 *
 * @note Use the seconds_increment as a prime so that dispersion of
 * test data across day boundaries is maximized.
 */
TEST(GregorianTest, Steps)
{
    // The starting date must be 1 greater than the epoch.
    // Otherwise the greg_lt calculation in test_greg() will precede the epoch.
    utility::gregorian greg_begin(1601u, utility::gregorian::January, 1u, 0u, 0u, 1u);
    utility::gregorian greg_end(  2701u, utility::gregorian::October, 1u);
    uint32_t const seconds_increment = 1299653;

    uint64_t const seconds_begin = utility::gregorian::seconds_since_epoch(greg_begin);
    uint64_t const seconds_end   = utility::gregorian::seconds_since_epoch(greg_end);

    for (uint64_t seconds = seconds_begin; seconds < seconds_end;
         seconds += seconds_increment)
    {
        utility::gregorian const greg = utility::gregorian(seconds);
        test_greg(greg);
    }
}

/**
 * Test Gregorian comparisons for a fixed set of data
 */
TEST(GregorianTest, Fixed)
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

    for (utility::gregorian const& greg: greg_test)
    {
        test_greg(greg);
    }
}
