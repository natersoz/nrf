/**
 * @file project_assert.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Define the ASSERT() macro on a project specific basis and avoid
 * the use of the standard library assert().
 */

#include "project_assert.h"
#include <cstdio>

extern "C"
void assertion_failed(char const*  file_name,
                      char const*  func_name,
                      size_t       line_no,
                      char const*  condition)
{
    fprintf(stderr, "file: %s, func: %s, line: %4zd: '%s'",
            file_name, func_name, line_no, condition);
    exit(-1);
}
