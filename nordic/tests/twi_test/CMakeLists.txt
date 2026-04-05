###
# File: nrf/nordic/tests/twi_test/CMakeLists.txt
# @copyright (c) 2026, natersoz. Distributed under the Apache 2.0 license.
#
# SPI Master/Slave Test
#
# Typical usage:
# cmake -S . -B ./_build --toolchain ../../../Toolchain-gcc-arm-none-eabi.cmake
# cmake --build ./_build
#
# Optional build parameters:
# --parallel <N> --target <target_name> --verbose
###

cmake_minimum_required(VERSION 4.0.0)

set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)

# Top level CMake file for the Impinj Reader Chip SDK
project(twi_test
    VERSION 0.0
    DESCRIPTION "nRF hobby project, SAADC test"
    LANGUAGES CXX C ASM
)

if(NOT DEFINED PROJECT_ROOT)
    set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../..)
endif()

if(NOT DEFINED SDK_ROOT)
    set(SDK_ROOT ${PROJECT_ROOT}/sdk)
endif()

if(NOT DEFINED BOOST_ROOT)
    set(BOOST_ROOT ${PROJECT_ROOT}/external/boost)
endif()

if(NOT TARGET nordic_sdk)
    add_subdirectory(${PROJECT_ROOT}/nordic/sdk-lib/ nordic_sdk)
endif()

message(STATUS "----------------------------")
message(STATUS "CMAKE_CURRENT_SOURCE_DIR    : ${CMAKE_CURRENT_SOURCE_DIR}")
message(DEBUG  "TOOLCHAIN                   : ${CMAKE_TOOLCHAIN_FILE}")
message(DEBUG  "PROJECT_ROOT                : ${PROJECT_ROOT}")
message(DEBUG  "BOOST_ROOT                  : ${BOOST_ROOT}")

add_executable(twi_test)

target_link_libraries(twi_test PRIVATE nordic_sdk)

target_sources(twi_test PRIVATE
    ./main.cc
    ${PROJECT_ROOT}/nordic/peripherals/twi_common.cc
    ${PROJECT_ROOT}/nordic/peripherals/twim.cc
    ${PROJECT_ROOT}/nordic/peripherals/twis.cc
    ${PROJECT_ROOT}/nordic/peripherals/temperature_sensor.cc
    ${PROJECT_ROOT}/nordic/peripherals/rtc_debug.cc
    ${PROJECT_ROOT}/nordic/peripherals/clocks.cc
    ${PROJECT_ROOT}/nordic/peripherals/rtc.cc
    ${PROJECT_ROOT}/nordic/peripherals/gpio.cc
    ${PROJECT_ROOT}/nordic/peripherals/gpio_te.cc
    ${PROJECT_ROOT}/nordic/peripherals/ppi.cc
    ${PROJECT_ROOT}/nordic/peripherals/timer.cc
    ${PROJECT_ROOT}/nordic/peripherals/leds_pca10040.cc
    ${PROJECT_ROOT}/nordic/peripherals/buttons_pca10040.cc
    ${PROJECT_ROOT}/gcc-arm/gcc_startup_nrf52.s
    ${PROJECT_ROOT}/gcc-arm/gcc_stubs.cc
    ${PROJECT_ROOT}/utility/project_assert.cc
    ${PROJECT_ROOT}/utility/format_conversion.cc
    ${PROJECT_ROOT}/utility/int_to_string.cc
    ${PROJECT_ROOT}/utility/vwritef.cc
    ${PROJECT_ROOT}/utility/write_data.cc
    ${PROJECT_ROOT}/utility/std_stubs.cc
    ${PROJECT_ROOT}/logger/logger.cc
    ${PROJECT_ROOT}/logger/logger_c.cc
    ${PROJECT_ROOT}/logger/rtt_output_stream.cc
    ${PROJECT_ROOT}/segger/segger_rtt.cc

    ${SDK_ROOT}/modules/nrfx/mdk/system_nrf52.c

    ${SDK_ROOT}/components/libraries/util/app_util_platform.c
 )

target_compile_definitions(twi_test PRIVATE
    NRF52832_XXAA
    BOARD_PCA10040
    FLOAT_ABI_HARD

    CONFIG_GPIO_AS_PINRESET
    SWI_DISABLE0

    TWIM0_ENABLED
    TWIS1_ENABLED
    TIMER1_ENABLED
    RTC1_ENABLED

    NRF_STRERROR_ENABLED
)

 target_include_directories(twi_test PRIVATE
    ${PROJECT_ROOT}
    ${PROJECT_ROOT}/gcc-arm
    ${PROJECT_ROOT}/logger
    ${PROJECT_ROOT}/nordic
    ${PROJECT_ROOT}/nordic/peripherals
    ${PROJECT_ROOT}/segger
    ${PROJECT_ROOT}/utility
    ${PROJECT_ROOT}/nordic/sdk-lib      # for sdk_config.h

    ${BOOST_ROOT}
 )

target_include_directories(twi_test SYSTEM PRIVATE
    ${SDK_ROOT}/components/ble/common
    ${SDK_ROOT}/components/ble/peer_manager
    # ${SDK_ROOT}/components/drivers_nrf/hal
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
    ${SDK_ROOT}/modules/nrfx/mdk

    # These would be nice to dump:
    ${SDK_ROOT}/components/libraries/strerror
    ${SDK_ROOT}/components/libraries/util

    ${SDK_ROOT}/components/softdevice/common
    ${SDK_ROOT}/components/softdevice/s132/headers
    ${SDK_ROOT}/components/softdevice/s132/headers/nrf52
    ${SDK_ROOT}/components/toolchain
    ${SDK_ROOT}/components/toolchain/cmsis/include

    # These might be interesting, but not required:
    # ${SDK_ROOT}/components/libraries/mutex
    # ${SDK_ROOT}/components/libraries/queue
    # ${SDK_ROOT}/components/libraries/hardfault
    # ${SDK_ROOT}/components/libraries/ecc
    # ${SDK_ROOT}/components/libraries/hci
)

target_link_directories(twi_test PRIVATE
    ${PROJECT_ROOT}/twi_test
    ${PROJECT_ROOT}/nordic
)

target_link_options(twi_test PRIVATE
    -Wl,-Map=twi_test.map
    --specs=nano.specs
    -lc
    -lnosys
    -Wl,--gc-sections
    -T ${CMAKE_CURRENT_SOURCE_DIR}/twi_test.ld
)

# Generate post-build files for the target: .hex, .lst, .symbols.txt
include(${PROJECT_ROOT}/post_elf_build.cmake)
add_firmware_postbuild(twi_test)
