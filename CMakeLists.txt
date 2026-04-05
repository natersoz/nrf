###
# File: nrf/CMakeLists.txt
# @copyright (c) 2026, natersoz. Distributed under the Apache 2.0 license.
#
# nRF project top level CMake file.
#
# Typical usage:
# cmake -S . -B ./_build --toolchain Toolchain-gcc-arm-none-eabi.cmake
# cmake --build ./_build
#
# Optional build parameters:
# --parallel <N> --target <target_name> --verbose
###

cmake_minimum_required(VERSION 4.0.0)

set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)

# Top level CMake file for the Impinj Reader Chip SDK
project(nrf
    VERSION 0.0
    DESCRIPTION "nRF hobby project"
    LANGUAGES C CXX
)

set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

if(NOT DEFINED SDK_ROOT)
    set(SDK_ROOT ${PROJECT_ROOT}/sdk)
endif()

if(NOT DEFINED BOOST_ROOT)
    set(BOOST_ROOT ${PROJECT_ROOT}/external/boost)
endif()

if(NOT DEFINED CMAKE_BUILD_TYPE OR "${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE "Debug")
    message(STATUS "CMAKE_BUILD_TYPE (default)  : ${CMAKE_BUILD_TYPE}")
endif()

message(STATUS "CMAKE_CURRENT_SOURCE_DIR    : ${CMAKE_CURRENT_SOURCE_DIR}")
message(DEBUG  "TOOLCHAIN                   : ${CMAKE_TOOLCHAIN_FILE}")
message(DEBUG  "PROJECT_ROOT                : ${PROJECT_ROOT}")
message(DEBUG  "BOOST_ROOT                  : ${BOOST_ROOT}")

block()
string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE_UPPER)
message(DEBUG  "CMAKE_CONFIGURATION_TYPES   : ${CMAKE_CONFIGURATION_TYPES}")
message(DEBUG  "C_FLAGS                     : ${CMAKE_C_FLAGS_${BUILD_TYPE_UPPER}}")
message(DEBUG  "CXX_FLAGS                   : ${CMAKE_CXX_FLAGS_${BUILD_TYPE_UPPER}}")
message(DEBUG  "EXE_LINKER_FLAGS            : ${CMAKE_EXE_LINKER_FLAGS_${BUILD_TYPE_UPPER}}")
message(DEBUG  "SHARED_LINKER_FLAGS         : ${CMAKE_SHARED_LINKER_FLAGS_${BUILD_TYPE_UPPER}}")
endblock()

add_subdirectory(${PROJECT_ROOT}/nordic/sdk-lib/ nordic_sdk)
add_subdirectory(ble_peripheral                 ble_peripheral)
add_subdirectory(ble_central                    ble_central)

add_subdirectory(nordic/tests/app_timer_test    nordic/tests/app_timer_test)
add_subdirectory(nordic/tests/buttons_leds_test nordic/tests/buttons_leds_test)
add_subdirectory(nordic/tests/gpio_te_test      nordic/tests/gpio_te_test)
add_subdirectory(nordic/tests/rtc_test          nordic/tests/rtc_test)
add_subdirectory(nordic/tests/saadc_test        nordic/tests/saadc_test)
add_subdirectory(nordic/tests/spi_test          nordic/tests/spi_test)
add_subdirectory(nordic/tests/timer_test        nordic/tests/timer_test)
add_subdirectory(nordic/tests/twi_test          nordic/tests/twi_test)
add_subdirectory(nordic/tests/usart_test        nordic/tests/usart_test)
