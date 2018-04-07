/**
 * @file project_assert.h
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

