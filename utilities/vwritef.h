/**
 * @file vwritef.h
 * A variable argument printf like writer interface.
 */

#pragma once

#include "stream.h"
#include <cstddef>
#include <cstdarg>

size_t  writef(output_stream& os, char const *fmt, ...);
size_t vwritef(output_stream& os, char const *fmt, va_list &args);


