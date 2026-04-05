###
# File: nrf/external/CMakeLists.txt
# @copyright (c) 2026, natersoz. Distributed under the Apache 2.0 license.
#
# nRF project external libraries download
#
# Typical usage:
# cmake -S . -B ./_build
# cmake --build ./_build
#
# Optional build parameters:
# --parallel <N> --target <target_name> --verbose
###

cmake_minimum_required(VERSION 4.0.0)

project(nrf
    VERSION 0.0
    DESCRIPTION "nRF hobby project, external library downloads"
)

# set(FETCHCONTENT_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external)

include(FetchContent)

FetchContent_Declare(
    Boost
    URL https://archives.boost.io/release/1.90.0/source/boost_1_90_0.tar.bz2
    URL_HASH SHA256=49551aff3b22cbc5c5a9ed3dbc92f0e23ea50a0f7325b0d198b705e8ee3fc305
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

FetchContent_Declare(
    NordicSdk
    URL https://nsscprodmedia.blob.core.windows.net/prod/software-and-other-downloads/sdks/nrf5/binaries/nrf5_sdk_17.1.0_ddde560.zip
    DOWNLOAD_EXTRACT_TIMESTAMP ON
)

FetchContent_MakeAvailable(NordicSdk)
