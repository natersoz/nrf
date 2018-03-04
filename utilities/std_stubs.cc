/**
 * @file std_stubs.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include <cstddef>

// Under gcc using virtual destructors, even when no malloc is involved,
// requires a delete operator to satisy the linker.
void operator delete(void*, size_t)
{}

