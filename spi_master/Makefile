###
# Makefile for testing Nordic nRF52 SPI interface.
# @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
###

SDK_ROOT := ../sdk

# The executable name
TARGET_NAME = nrf_spi_master

# Where the compiled files are built.
BUILD_PATH = _build

# Set for Debug or Release build types.
BUILD_TYPE	?= Debug

# Source files common to all targets
SOURCE_FILES =
SOURCE_FILES += $(SDK_ROOT)/components/boards/boards.c
# SOURCE_FILES += $(SDK_ROOT)/components/libraries/button/app_button.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/util/app_error.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/util/app_error_weak.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/util/app_util_platform.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/timer/app_timer.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/strerror/nrf_strerror.c
SOURCE_FILES += $(SDK_ROOT)/components/drivers_nrf/common/nrf_drv_common.c
# SOURCE_FILES += $(SDK_ROOT)/components/drivers_nrf/gpiote/nrf_drv_gpiote.c
SOURCE_FILES += $(SDK_ROOT)/components/drivers_nrf/spi_master/nrf_drv_spi.c
# SOURCE_FILES += $(SDK_ROOT)/components/drivers_nrf/uart/nrf_drv_uart.c
SOURCE_FILES += $(SDK_ROOT)/components/toolchain/system_nrf52.c
SOURCE_FILES += ./main.cc
SOURCE_FILES += ../common/gcc_startup_nrf52.s

SOURCE_FILES += ../utilities/format_conversion.cc
SOURCE_FILES += ../utilities/int_to_string.cc
SOURCE_FILES += ../utilities/vwritef.cc
SOURCE_FILES += ../utilities/write_data.cc
SOURCE_FILES += ../utilities/std_stubs.cc
SOURCE_FILES += ../logger/logger.cc
SOURCE_FILES += ../logger/segger_rtt_output_stream.cc
SOURCE_FILES += ../segger_rtt/SEGGER_RTT.c

# Include folders common to all targets
INCLUDE_PATHS =
INCLUDE_PATHS += $(SDK_ROOT)/components
INCLUDE_PATHS += $(SDK_ROOT)/components/boards
INCLUDE_PATHS += $(SDK_ROOT)/components/device
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/delay
# INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/gpiote
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/hal
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/nrf_soc_nosd
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/spi_master
# INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/atomic
# INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/button
INCLUDE_PATHS += $(SDK_ROOT)/components/toolchain/cmsis/include
INCLUDE_PATHS +=  ./config

# Libraries can be added here, includes path: path/filename.a
LIB_FILES +=

# C flags common to all Nordic targets
NORDIC_DEFS += -DBOARD_PCA10040
NORDIC_DEFS += -DCONFIG_GPIO_AS_PINRESET
NORDIC_DEFS += -DFLOAT_ABI_HARD
NORDIC_DEFS += -DNRF52
NORDIC_DEFS += -DNRF52832_XXAA
NORDIC_DEFS += -DNRF52_PAN_74
NORDIC_DEFS += -DSWI_DISABLE0

# For now only C files use Nordic specific compile time symbols.
CFLAGS      += $(NORDIC_DEFS)
CXXFLAGS    += $(NORDIC_DEFS)
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

include ../common/arm_gcc_rules.mak
include ../common/jlink_rules.mak

