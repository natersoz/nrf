/**
 * @file boost_exception.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "logger.h"
#include "project_assert.h"

#include <exception>

namespace boost
{

/**
 * Since exceptions are turned off, and boost may throw an exception,
 * the following implementation is required to satisfy the linker and
 * will catch any boost allocator exceptions.
 *
 * @example
 * The fixed_allocator is improperly used in the circular_buffer context.
 */
void throw_exception(std::exception const& except)
{
    logger& logger = logger::instance();
    logger.error("fatal exception: %s", except.what());
    ASSERT(0);
}

} // namespace boost
