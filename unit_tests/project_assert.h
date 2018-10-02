/**
 * @file project_assert.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Define the ASSERT() macro on a project specific basis and avoid
 * the use of the standard library assert().
 */

#pragma once

#include <cstddef>
#include <cassert>

#define ASSERT      assert
