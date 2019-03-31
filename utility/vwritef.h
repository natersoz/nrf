/**
 * @file vwritef.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * A variable argument printf like writer interface.
 */

#pragma once

#include "stream.h"
#include <cstddef>
#include <cstdarg>

size_t  writef(io::output_stream& os, char const* fmt, ...);
size_t vwritef(io::output_stream& os, char const* fmt, va_list& args);


