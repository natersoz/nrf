/**
 * @file gcc_exception_stubs.cc
 *
 * If exceptions are enabled, and the linker is supplied the --no-stdlib
 * option, then this file will need to be linked in.
 *
 * Here is a great resource related to ARM, exception handling and the ABI:
 * https://monoinfinito.wordpress.com/series/exception-handling-in-c/
 */

#include <unwind-arm-common.h>

#include "logger.h"
#include "project_assert.h"

extern "C" {

_Unwind_Reason_Code __gxx_personality_v0(int                        version,
                                         _Unwind_Action             actions,
                                         uint64_t                   exception_class,
                                         struct _Unwind_Exception*  exception_object,
                                         struct _Unwind_Context*    context)
{
    logger::instance().error("__gxx_personality_v0(): version: %u, actions: %u, class: %llu",
                             version, actions, exception_class);

    ASSERT(0);
    while (true) {}
}

void __cxa_end_cleanup(void)
{
    ASSERT(0);
    while (true) {}
}

void __cxa_begin_catch()
{
    ASSERT(0);
}

void __cxa_end_catch()
{
    ASSERT(0);
}

void __cxa_rethrow()
{
    ASSERT(0);
}

}

