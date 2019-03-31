###
# Makefile for testing Nordic nRF52 Timer interface(s).
# @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
###

PROJECT_ROOT := ../../..
SDK_ROOT     := $(PROJECT_ROOT)/sdk

# The executable name
TARGET_NAME = app_timer_test

# Where the compiled files are built.
BUILD_PATH = _build

# Set for Debug or Release build types.
BUILD_TYPE	?= Debug

# Path to boot installation.
BOOST_ROOT=/opt/boost

# It is important to list the more specialized source files first.
# The ordering of searching for files - especially include files -
# is dependent on which is listed first.
# Some Nordic headers are intentionally replaced in the project.

SOURCE_FILES =
SOURCE_FILES += ./main.cc

SOURCE_FILES += $(PROJECT_ROOT)/nordic/app_timer.cc
SOURCE_FILES += $(PROJECT_ROOT)/nordic/peripherals/rtc_debug.cc
SOURCE_FILES += $(PROJECT_ROOT)/nordic/peripherals/clocks.cc
SOURCE_FILES += $(PROJECT_ROOT)/nordic/peripherals/rtc.cc
SOURCE_FILES += $(PROJECT_ROOT)/nordic/peripherals/gpio.cc
SOURCE_FILES += $(PROJECT_ROOT)/nordic/peripherals/timer.cc
SOURCE_FILES += $(PROJECT_ROOT)/nordic/peripherals/leds_pca10040.cc
SOURCE_FILES += $(PROJECT_ROOT)/nordic/peripherals/buttons_pca10040.cc
SOURCE_FILES += $(PROJECT_ROOT)/gcc-arm/gcc_startup_nrf52.s
SOURCE_FILES += $(PROJECT_ROOT)/gcc-arm/gcc_stubs.cc
SOURCE_FILES += $(PROJECT_ROOT)/utility/project_assert.cc
SOURCE_FILES += $(PROJECT_ROOT)/utility/format_conversion.cc
SOURCE_FILES += $(PROJECT_ROOT)/utility/int_to_string.cc
SOURCE_FILES += $(PROJECT_ROOT)/utility/vwritef.cc
SOURCE_FILES += $(PROJECT_ROOT)/utility/write_data.cc
SOURCE_FILES += $(PROJECT_ROOT)/utility/std_stubs.cc
SOURCE_FILES += $(PROJECT_ROOT)/logger/logger.cc
SOURCE_FILES += $(PROJECT_ROOT)/logger/logger_c.cc
SOURCE_FILES += $(PROJECT_ROOT)/logger/rtt_output_stream.cc
SOURCE_FILES += $(PROJECT_ROOT)/segger/segger_rtt.cc

# SDK source files:
SOURCE_FILES += $(SDK_ROOT)/modules/nrfx/mdk/system_nrf52.c

SOURCE_FILES += $(SDK_ROOT)/components/libraries/util/app_util_platform.c

# Include folders:
INCLUDE_PATHS =  .
INCLUDE_PATHS += $(PROJECT_ROOT)/gcc-arm
INCLUDE_PATHS += $(PROJECT_ROOT)/logger
INCLUDE_PATHS += $(PROJECT_ROOT)/nordic
INCLUDE_PATHS += $(PROJECT_ROOT)/nordic/peripherals
INCLUDE_PATHS += $(PROJECT_ROOT)/segger
INCLUDE_PATHS += $(PROJECT_ROOT)/utility

INCLUDE_PATHS += ./config
INCLUDE_PATHS += $(BOOST_ROOT)/include

INCLUDE_PATHS += $(SDK_ROOT)/modules/nrfx/mdk
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/util
INCLUDE_PATHS += $(SDK_ROOT)/components/softdevice/s132/headers
INCLUDE_PATHS += $(SDK_ROOT)/components/toolchain
INCLUDE_PATHS += $(SDK_ROOT)/components/toolchain/cmsis/include

###
# Use the LD_PATHS to allow the linker to bring in common linker scripts
# The assumption is that projects are at the saem dieectory level as common.
###
LD_PATHS   = $(PROJECT_ROOT)/nordic

LIB_FILES +=

# Nordic specific compiler definitions
# NRF52840_XXAA		One of these types must be defined
# NRF52832_XXAA		<- Only testing with this one to start.
# NRF52832_XXAB
# NRF52810_XXAA		Does not have FPU
# NRF51
#
NORDIC_DEFS += -D NRF52832_XXAA
NORDIC_DEFS += -D BOARD_PCA10040
NORDIC_DEFS += -D FLOAT_ABI_HARD

NORDIC_DEFS += -D CONFIG_GPIO_AS_PINRESET
NORDIC_DEFS += -D SWI_DISABLE0

NORDIC_DEFS += -D RTC1_ENABLED
NORDIC_DEFS += -D RTC2_ENABLED
NORDIC_DEFS += -D TIMER2_ENABLED

CXXFLAGS    += $(NORDIC_DEFS)
CFLAGS      += $(NORDIC_DEFS)
ASFLAGS     += $(NORDIC_DEFS)

.PHONY:  all relink clean info

# Building all targets
all: 	$(BUILD_PATH)				\
	$(BUILD_PATH)/$(TARGET_NAME).out	\
	$(BUILD_PATH)/$(TARGET_NAME).bin	\
	$(BUILD_PATH)/$(TARGET_NAME).hex	\
	$(BUILD_PATH)/$(TARGET_NAME).lst	\
	$(BUILD_PATH)/$(TARGET_NAME).dump.txt

relink: $(BUILD_PATH)
	$(RM) $(BUILD_PATH)/$(TARGET_NAME).*
	make -C . all

clean:
	$(RM) $(BUILD_PATH)

info:
	@echo "BUILD_PATH         = $(BUILD_PATH)"
	@echo "BUILD_TYPE         = $(BUILD_TYPE)"
	@echo "LINKER_SCRIPT      = $(LINKER_SCRIPT)"
	@echo "CFLAGS             = $(CFLAGS)"
	@echo "CXXFLAGS           = $(CXXFLAGS)"
	@echo "ASFLAGS            = $(ASFLAGS)"
	@echo "LDFLAGS            = $(LDFLAGS)"
	@echo "SDK_ROOT           = $(SDK_ROOT)"
	@echo
	@echo "SOURCE_FILES       = $(SOURCE_FILES)"
	@echo
	@echo "OBJECT_FILES       = $(OBJECT_FILES)"
	@echo
	@echo "DEPEND_FILES       = $(DEPEND_FILES)"
	@echo

include $(PROJECT_ROOT)/make_rules/arm_gcc_rules.mak
include $(PROJECT_ROOT)/make_rules/jlink_rules.mak

