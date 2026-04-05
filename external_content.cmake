###
# File: nrf/external_content.cmake
# @copyright (c) 2026, natersoz. Distributed under the Apache 2.0 license.
#
# nRF project external content downloads, extraction and post-processing.
#
# Usage:
# include(${PROJECT_ROOT}/external_content.cmake)
# Note: PROJECT_ROOT must be defined.
###

if(NOT DEFINED PROJECT_ROOT)
    message(FATAL_ERROR "PROJECT_ROOT is not defined")
endif()

# ----- Download external dependencies: boost and nRF SDK ----- #
set(FETCHCONTENT_BASE_DIR ${PROJECT_ROOT}/external)

include(FetchContent)

# See: https://cmake.org/cmake/help/latest/module/FetchContent.html#command:fetchcontent_makeavailable
# Set all declarations before any calls to FetchContent_MakeAvailable()
FetchContent_Declare(
    Boost
    URL https://archives.boost.io/release/1.90.0/source/boost_1_90_0.tar.bz2
    URL_HASH SHA256=49551aff3b22cbc5c5a9ed3dbc92f0e23ea50a0f7325b0d198b705e8ee3fc305
    DOWNLOAD_EXTRACT_TIMESTAMP ON
)

FetchContent_Declare(
    NordicSdk
    URL https://nsscprodmedia.blob.core.windows.net/prod/software-and-other-downloads/sdks/nrf5/binaries/nrf5_sdk_17.1.0_ddde560.zip
    DOWNLOAD_EXTRACT_TIMESTAMP ON
)

# This will download and extract, but it might still bark if it
# looks for a CMakeLists.txt. To bypass the "sub-build" behavior:
FetchContent_MakeAvailable(Boost)

# Create an interface library for the headers
if(NOT TARGET boost_headers)
  add_library(boost_headers INTERFACE)
  # boost_SOURCE_DIR is automatically set by FetchContent_MakeAvailable
  target_include_directories(boost_headers INTERFACE ${boost_SOURCE_DIR})
endif()

FetchContent_MakeAvailable(NordicSdk)

# Move nrf_assert.h out of the way so that our ./nordic/nrf_assert.h gets used instead.
if(EXISTS "${nordicsdk_SOURCE_DIR}/components/libraries/util/nrf_assert.h")
    file(RENAME
         "${nordicsdk_SOURCE_DIR}/components/libraries/util/nrf_assert.h"
         "${nordicsdk_SOURCE_DIR}/components/libraries/util/nrf_assert.orig.h"
    )
endif()

if(NOT EXISTS "${nordicsdk_SOURCE_DIR}/components/toolchain/cmsis/include/cmsis_gcc.orig.h")
    file(RENAME
         "${nordicsdk_SOURCE_DIR}/components/toolchain/cmsis/include/cmsis_gcc.h"
         "${nordicsdk_SOURCE_DIR}/components/toolchain/cmsis/include/cmsis_gcc.orig.h"
    )
    file(COPY_FILE
        "${PROJECT_ROOT}/nordic/sdk-modified/cmsis_gcc.h"
        "${nordicsdk_SOURCE_DIR}/components/toolchain/cmsis/include/cmsis_gcc.h"
    )
endif()


message(STATUS "boost_SOURCE_DIR            : ${boost_SOURCE_DIR}")
message(STATUS "nordicsdk_SOURCE_DIR        : ${nordicsdk_SOURCE_DIR}")
