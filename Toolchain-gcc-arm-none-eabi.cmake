###
# File: Toolchain-gcc-arm-none-eabi.cmake
# @copyright (c) 2026, natersoz. Distributed under the Apache 2.0 license.
#
# ARM Cortex M/R bare metal tool chain.
###

set(ARM_GCC_PATH /opt/gcc-arm-none-eabi)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR cortex-m4)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED YES)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

set(CMAKE_C_COMPILER    ${ARM_GCC_PATH}/arm-none-eabi/bin/arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER  ${ARM_GCC_PATH}/arm-none-eabi/bin/arm-none-eabi-g++)
set(CMAKE_OBJCOPY       ${ARM_GCC_PATH}/arm-none-eabi/bin/arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP       ${ARM_GCC_PATH}/arm-none-eabi/bin/arm-none-eabi-objdump)
set(CMAKE_NM            ${ARM_GCC_PATH}/arm-none-eabi/bin/arm-none-eabi-nm)
set(CMAKE_SIZE          ${ARM_GCC_PATH}/arm-none-eabi/bin/arm-none-eabi-size)

# set(CMAKE_C_COMPILER_ID   ARMCC)
# set(CMAKE_CXX_COMPILER_ID ARMCC)
# set(CMAKE_ASM_COMPILER_ID ARMCC)

set(CMAKE_C_COMPILER_VERSION,   13.2.0)
set(CMAKE_CXX_COMPILER_VERSION, 13.2.0)
set(CMAKE_ASM_COMPILER_VERSION, 13.2.0)

# The only directory that a toolchain files knows about is its current
# directory CMAKE_CURRENT_LIST_DIR.
message(DEBUG  "---------------------------- Toolchain")
message(DEBUG  "TOOLCHAIN                   : ${CMAKE_TOOLCHAIN_FILE}")
message(DEBUG  "TOOLCHAIN_PREFIX            : ${TOOLCHAIN_PREFIX}")
message(DEBUG  "CMAKE_CURRENT_LIST_DIR      : ${CMAKE_CURRENT_LIST_DIR}")
message(DEBUG  "CMAKE_BUILD_TYPE            : ${CMAKE_BUILD_TYPE}")

# GCC typical compiler options
add_compile_options(
    -fno-common
    -fstack-protector-all
    -fstrict-aliasing
    -fstrict-overflow
    -Wall
    -Wpedantic
    -Wextra
    -Wcast-qual
    -Wfloat-equal
    -Wformat=2
    -Winline
    -Wlogical-op
    -Wmissing-field-initializers
    -Wpointer-arith
    -Wredundant-decls
    -Wshadow
    -Wswitch-default
    -Wundef
    -Wunreachable-code
    -Wunknown-pragmas
    -Wno-psabi
    -Wmaybe-uninitialized
    -Wno-enum-compare
    -Werror

    -ffunction-sections
    -fdata-sections
    -fno-builtin
    -fshort-enums

    -nostdlib

    $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
    $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>

    $<$<COMPILE_LANGUAGE:C>:-Wold-style-definition>
    $<$<COMPILE_LANGUAGE:C>:-Wstrict-prototypes>
    $<$<COMPILE_LANGUAGE:C>:-Wnested-externs>
)

# ARM Cortex specific options
# Used for both the compiler and linker.
set(ARM_CPU_FLAGS
    -mthumb
    -mabi=aapcs
    -mcpu=cortex-m4
    -mfloat-abi=hard
    -mfpu=fpv4-sp-d16
)

add_compile_options(${ARM_CPU_FLAGS})
add_link_options(${ARM_CPU_FLAGS})

# Nordic specific compiler definitions
# NRF52840_XXAA		One of these types must be defined
# NRF52832_XXAA		<- Only testing with this one to start.
# NRF52832_XXAB
# NRF52810_XXAA		Does not have FPU
# NRF51
add_compile_definitions(
    "NRF52832_XXAA"
    "BOARD_PCA10040"
    "FLOAT_ABI_HARD"

    "NRF_SD_BLE_API_VERSION=6"
    "S132"
    "SOFTDEVICE_PRESENT=1"

    "CONFIG_GPIO_AS_PINRESET"
    "SWI_DISABLE0"

    "RTC1_ENABLED"
    "TIMER1_ENABLED"
)

set(CMAKE_C_FLAGS_RELEASE_INIT      "-g  -O3")
set(CMAKE_CXX_FLAGS_RELEASE_INIT    "-g  -O3")
set(CMAKE_ASM_FLAGS_RELEASE_INIT    "-g  -O3")
set(CMAKE_C_FLAGS_DEBUG_INIT        "-g3 -O0")
set(CMAKE_CXX_FLAGS_DEBUG_INIT      "-g3 -O0")
set(CMAKE_ASM_FLAGS_DEBUG_INIT      "-g3 -O0")
set(CMAKE_C_FLAGS_MINSIZEREL_INIT   "-g  -Os")
set(CMAKE_CXX_FLAGS_MINSIZEREL_INIT "-g  -Os")
set(CMAKE_ASM_FLAGS_MINSIZEREL_INIT "-g  -Os")

#set (CMAKE_EXE_LINKER_FLAGS
#    --specs=nano.specs
#    -lc
#    -lnosys
#    -Wl,--gc-sections
#)
