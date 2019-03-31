/**
 * @file version_info.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "version_info.h"
#include "section_macros.h"

struct version_info const version_info IN_SECTION(".version_info") =
{
    .version  = GIT_TAG,
    .rfu      = {0},
    .git_hash = {GIT_HASH}
};
