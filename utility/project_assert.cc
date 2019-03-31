/**
 * @file project_assert.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Define the ASSERT() macro on a project specific basis and avoid
 * the use of the standard library assert().
 */

#include "project_assert.h"
#include "logger.h"

void assertion_failed(char const*  file_name,
                      char const*  func_name,
                      size_t       line_no,
                      char const*  condition)
{
    logger& logger = logger::instance();
    logger.error("file: %s, func: %s, line: %4d: '%s'",
                 file_name, func_name, line_no, condition);
    logger.flush();

    __asm("bkpt #0");

    while (true)
    {
    }
}

void assertion_check_failed(char const*  file_name,
                            char const*  func_name,
                            size_t       line_no,
                            char const*  param_1,
                            char const*  reason,
                            char const*  param_2)
{
    logger& logger = logger::instance();
    logger.error("file: %s, func: %s, line: %4d: failed: '%s %s %s'",
                 file_name, func_name, line_no, param_1, reason, param_2);
    logger.flush();

    __asm("bkpt #0");

    while (true)
    {
    }
}
