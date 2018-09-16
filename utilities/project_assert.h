/**
 * @file project_assert.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Define the ASSERT() macro on a project specific basis and avoid
 * the use of the standard library assert().
 */

#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void assertion_failed(char const*  file_name,
                      char const*  func_name,
                      size_t       line_no,
                      char const*  condition);

void assertion_check_failed(char const*  file_name,
                            char const*  func_name,
                            size_t       line_no,
                            char const*  param_1,
                            char const*  reason,
                            char const*  param_2);

#if defined (NDEBUG)
    #define ASSERT(condition)
#else
    #define ASSERT(condition)                                               \
        if (!(condition)) {                                                 \
            assertion_failed(__FILE__, __func__, __LINE__, #condition);     \
        }
#endif

#ifdef __cplusplus
}
#endif

