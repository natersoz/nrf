###
# File: nrf/nordic/sdk-lib/CMakeLists.txt
# @copyright (c) 2026, natersoz. Distributed under the Apache 2.0 license.
#
# Nordic SDK library builder
# Build the Nordic SDK separately into a library.
# - Reduces compiling the same files for each application.
# - Nordic SDK code is emits warnings that need to be ignored.
#
# Typical usage:
# cmake -S . -B ./_build ../../--toolchain Toolchain-gcc-arm-none-eabi.cmake
# cmake --build ./_build
#
# Optional build parameters:
# --parallel <N> --target <target_name> --verbose
###

cmake_minimum_required(VERSION 4.0.0)

set(CMAKE_C_COMPILER_WORKS 1)

# Top level CMake file for the Impinj Reader Chip SDK
project(nordic_sdk_lib
    VERSION 0.0
    DESCRIPTION "nRF hobby project, Nordic SDK library"
    LANGUAGES C
)

if(NOT DEFINED PROJECT_ROOT)
    set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../..)
endif()

if(NOT DEFINED SDK_ROOT)
    set(SDK_ROOT ${PROJECT_ROOT}/sdk)
endif()

message(DEBUG  "----------------------------")
message(DEBUG  "TOOLCHAIN                   : ${CMAKE_TOOLCHAIN_FILE}")
message(DEBUG  "PROJECT_ROOT                : ${PROJECT_ROOT}")

add_library(nordic_sdk STATIC)

# Nordic SDK is sloppy
target_compile_options(nordic_sdk PRIVATE
    -Wno-pedantic
    -Wno-expansion-to-defined
    -Wno-undef
    -Wno-unused-parameter
    -Wno-strict-prototypes
    -Wno-old-style-definition
    -Wno-cast-qual
    -Wno-switch-default
    -Wno-implicit-fallthrough
)

target_include_directories(nordic_sdk SYSTEM PRIVATE
    .
    ..
    ../../logger
    ../../utility

    ${SDK_ROOT}/components/ble/common
    ${SDK_ROOT}/components/ble/peer_manager
#    ${SDK_ROOT}/components/drivers_nrf/hal
    ${SDK_ROOT}/components/libraries/atomic
    ${SDK_ROOT}/components/libraries/atomic_fifo
    ${SDK_ROOT}/components/libraries/atomic_flags
    ${SDK_ROOT}/components/libraries/crc16
    ${SDK_ROOT}/components/libraries/crc32
    ${SDK_ROOT}/components/libraries/experimental_section_vars
    ${SDK_ROOT}/components/libraries/fds
    ${SDK_ROOT}/components/libraries/fstorage
    ${SDK_ROOT}/components/libraries/log
    ${SDK_ROOT}/components/libraries/mutex

# These would be nice to dump:
    ${SDK_ROOT}/components/libraries/strerror
    ${SDK_ROOT}/components/libraries/util

# Softdevice headers. Eventually (hopefully), only these and CMSIS remain.
    ${SDK_ROOT}/components/softdevice/common
    ${SDK_ROOT}/components/softdevice/s132/headers
    ${SDK_ROOT}/components/softdevice/s132/headers/nrf52
    ${SDK_ROOT}/components/toolchain
    ${SDK_ROOT}/components/toolchain/cmsis/include

# Nordic CMSIS related. Also essential.
    ${SDK_ROOT}/modules/nrfx/mdk

# These might be interesting, but not required:
#   ${SDK_ROOT}/components/libraries/mutex
#   ${SDK_ROOT}/components/libraries/queue
#   ${SDK_ROOT}/components/libraries/hardfault
#   ${SDK_ROOT}/components/libraries/ecc
#   ${SDK_ROOT}/components/libraries/hci
)

target_sources(nordic_sdk PRIVATE
    ${SDK_ROOT}/components/libraries/util/app_error.c
    ${SDK_ROOT}/components/libraries/util/app_error_weak.c
    ${SDK_ROOT}/components/ble/common/ble_conn_state.c
    ${SDK_ROOT}/components/libraries/strerror/nrf_strerror.c

    # Peer manager and required dependencies:
    ${SDK_ROOT}/components/ble/peer_manager/gatt_cache_manager.c
    ${SDK_ROOT}/components/ble/peer_manager/gatts_cache_manager.c
    ${SDK_ROOT}/components/ble/peer_manager/id_manager.c
    ${SDK_ROOT}/components/ble/peer_manager/peer_data_storage.c
    ${SDK_ROOT}/components/ble/peer_manager/peer_database.c
    ${SDK_ROOT}/components/ble/peer_manager/peer_id.c
    ${SDK_ROOT}/components/ble/peer_manager/peer_manager.c
    ${SDK_ROOT}/components/ble/peer_manager/pm_buffer.c
    ${SDK_ROOT}/components/ble/peer_manager/security_dispatcher.c
    ${SDK_ROOT}/components/ble/peer_manager/security_manager.c
    ${SDK_ROOT}/components/libraries/atomic/nrf_atomic.c
    ${SDK_ROOT}/components/libraries/atomic_fifo/nrf_atfifo.c
    ${SDK_ROOT}/components/libraries/atomic_flags/nrf_atflags.c
    ${SDK_ROOT}/components/libraries/crc16/crc16.c
    ${SDK_ROOT}/components/libraries/fds/fds.c
    ${SDK_ROOT}/components/libraries/fstorage/nrf_fstorage.c
    ${SDK_ROOT}/components/libraries/fstorage/nrf_fstorage_sd.c

    # Nordic CMSIS
    ${SDK_ROOT}/modules/nrfx/mdk/system_nrf52.c

    # How Nordic deals with linker sections.
    ${SDK_ROOT}/components/libraries/experimental_section_vars/nrf_section_iter.c
    ${SDK_ROOT}/components/softdevice/common/nrf_sdh.c
)
