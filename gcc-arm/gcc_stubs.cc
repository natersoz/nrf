/**
 * @file gcc_stubs.cc
 */

#include "project_assert.h"

// Handler for pure virtual functions (error).
extern "C" void __cxa_pure_virtual()
{
    ASSERT(0);
    while (true)
    {
    }
}

namespace std
{

void __throw_bad_function_call()
{
    ASSERT(0);
}

}   // namespace std
