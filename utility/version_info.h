/**
 * @file version_info.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @enum index
 * Provide meaning for indexing into the version[] array.
 */
enum version_index
{
    major = 0u,
    minor = 1u,
    patch = 2u,
    rfu   = 4u
};

struct version_info
{
    char const  version[32u];
    uint8_t     rfu[4u];
    uint8_t     git_hash[4u];
};

extern struct version_info const version_info;

#ifdef __cplusplus
}
#endif
