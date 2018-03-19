/**
 * @file project_assert.h
 * Define the ASSERT() macro on a project specific basis and avoid
 * the use of the standard library assert().
 */

#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void assertion_failed(char const*  func_name,
                      size_t       line_no,
                      char const*  condition);

#if defined (NDEBUG)
    #define ASSERT(condition)
#else
    #define ASSERT(condition)                                     \
        if (!(condition)) {                                       \
            assertion_failed(__func__, __LINE__, #condition);     \
        }
#endif

#ifdef __cplusplus
}
#endif

