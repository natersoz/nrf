/**
 * @file gcc_stubs.cc
 */

#include "project_assert.h"
#include <new>

extern "C" void __cxa_pure_virtual()
{
    ASSERT(0);
    while (true) {}
}

void operator delete(void *ptr)
{
    ASSERT(0);
    while (true) {}
}

void operator delete[] (void *ptr)
{
    ASSERT(0);
    while (true) {}
}

namespace std
{

void __throw_bad_function_call()
{
    ASSERT(0);
    while (true) {}
}

void __throw_length_error(char const *error_string)
{
    ASSERT(0);
    while (true) {}
}

} // namespace std


