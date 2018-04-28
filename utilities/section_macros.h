/**
 * @file section_macros.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 *
 * Define section placment macros here. Keep these simple, yet still
 * use macros so that other compilers can be accommodated when needed.
 */

#pragma once

#define IN_SECTION(section_name) __attribute__((section(section_name))) __attribute((used))

