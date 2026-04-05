/**
 * @file version_info.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "version_info.h"
#include "section_macros.h"

struct version_info const version_info IN_SECTION(".version_info") =
{
    /// @todo need actual implementation
    .version  = "No Tag Implemented",
    .git_hash = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
};
