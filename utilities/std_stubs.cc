/**
 * @file std_stubs.cc
 */

#include <cstddef>

// Under gcc using virtual destructors, even when no malloc is involved,
// requires a delete operator to satisy the linker.
void operator delete(void*, size_t)
{}

