###
# File: nrf/unit_tests/CMakeLists.txt
# @copyright (c) 2026, natersoz. Distributed under the Apache 2.0 license.
#
# Nordic SDK unit tests
#
# Typical usage:
# cmake -S . -B ./_build
# cmake --build ./_build
#
# Optional build parameters:
# --parallel <N> --target <target_name> --verbose
###

cmake_minimum_required(VERSION 4.0.0)

project(unit_tests LANGUAGES C CXX)

if(NOT DEFINED PROJECT_ROOT)
    set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)
endif()

if(NOT DEFINED BOOST_ROOT)
    set(BOOST_ROOT ${PROJECT_ROOT}/external/boost)
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED YES)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

option(ENABLE_TESTING "Enable building tests" ON)

if(NOT ENABLE_TESTING)
    return()
endif()

# ---- GoogleTest (download at configure time) ----
include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
)

# Prevent GoogleTest from overriding your compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

enable_testing()

function(add_gtest name)
    add_executable(${name} ${ARGN})

    target_include_directories(${name}
        PRIVATE
            ${PROJECT_ROOT}
            ${PROJECT_ROOT}/utility
            ${PROJECT_ROOT}/logger
            ${PROJECT_ROOT}/nordic/peripherals

        PRIVATE SYSTEM
            ${BOOST_ROOT}
    )

    target_link_libraries(${name}
        PRIVATE
            GTest::gtest
            GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(${name})
endfunction()


add_gtest(test_bit_manip
    test_bit_manip.cc
)

add_gtest(test_ble_service_container
    test_ble_service_container.cc
    ${PROJECT_ROOT}/ble/gatt_attribute.cc
    ${PROJECT_ROOT}/ble/gatt_declaration.cc
    ${PROJECT_ROOT}/ble/gatt_service.cc
    ${PROJECT_ROOT}/ble/gatt_service_container.cc
    ${PROJECT_ROOT}/ble/gatt_service_discovery_iterator.cc
    ${PROJECT_ROOT}/ble/gatt_write_ostream.cc
    ${PROJECT_ROOT}/ble/uuid.cc
    ${PROJECT_ROOT}/ble/gatt_enum_types_strings.cc

    ${PROJECT_ROOT}/utility/int_to_string.cc
    ${PROJECT_ROOT}/utility/vwritef.cc
    ${PROJECT_ROOT}/utility/format_conversion.cc
    ${PROJECT_ROOT}/utility/write_data.cc

    ${PROJECT_ROOT}/logger/logger.cc

    assert_stubs.cc
    rtc_stubs.cc
)

add_gtest(test_ble_service
    test_ble_service.cc

    ${PROJECT_ROOT}/ble/gatt_attribute.cc
    ${PROJECT_ROOT}/ble/gatt_declaration.cc
    ${PROJECT_ROOT}/ble/gatt_service.cc
    ${PROJECT_ROOT}/ble/gatt_service_container.cc
    ${PROJECT_ROOT}/ble/gatt_service_discovery_iterator.cc
    ${PROJECT_ROOT}/ble/gatt_write_ostream.cc
    ${PROJECT_ROOT}/ble/uuid.cc
    ${PROJECT_ROOT}/ble/gatt_enum_types_strings.cc

    ${PROJECT_ROOT}/utility/int_to_string.cc
    ${PROJECT_ROOT}/utility/vwritef.cc
    ${PROJECT_ROOT}/utility/format_conversion.cc
    ${PROJECT_ROOT}/utility/write_data.cc

    ${PROJECT_ROOT}/logger/logger.cc

    assert_stubs.cc
    rtc_stubs.cc
)

add_gtest(test_fixed_allocator
    test_fixed_allocator.cc
)

add_gtest(test_format_conversion
    test_format_conversion.cc
    ${PROJECT_ROOT}/utility/format_conversion.cc
)

add_gtest(test_gregorian
    test_gregorian.cc
    ${PROJECT_ROOT}/utility/gregorian.cc
    ${PROJECT_ROOT}/utility/int_to_string.cc
    ${PROJECT_ROOT}/utility/vwritef.cc
    ${PROJECT_ROOT}/utility/format_conversion.cc
    ${PROJECT_ROOT}/utility/write_data.cc

    ${PROJECT_ROOT}/logger/logger.cc

    assert_stubs.cc
    rtc_stubs.cc
)

add_gtest(test_int_to_string
    test_int_to_string.cc
    ${PROJECT_ROOT}/utility/int_to_string.cc
)

add_gtest(test_make_array
    test_make_array.cc
)

add_gtest(test_observer
    test_observer.cc
)

add_gtest(test_uuid
    test_uuid.cc
    ${PROJECT_ROOT}/ble/uuid.cc
    ${PROJECT_ROOT}/utility/int_to_string.cc
)

# ---- Warnings (per target) ----
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    add_compile_options(unit_tests PRIVATE -Wall -Wextra -Wpedantic)
elseif(MSVC)
    add_compile_options(unit_tests PRIVATE /W4)
endif()

# ---- Discover tests automatically ----
include(GoogleTest)
