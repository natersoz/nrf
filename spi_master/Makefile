###
# Makefile for testing Nordic nRF52 SPI interface.
# @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
###

SDK_ROOT := ../sdk

# The executable name
TARGET_NAME = spi_master

# Where the compiled files are built.
BUILD_PATH = _build

# Set for Debug or Release build types.
BUILD_TYPE	?= Debug

# Path to boot installation. Using 1.65.1
BOOST_ROOT=/usr/local/boost

# It is important to list the more specialized source files first.
# The ordering of searching for files - especially include files -
# is dependent on which is listed first.
# Some Nordic headers are intentionally replaced in the project.

SOURCE_FILES =
SOURCE_FILES += ./main.cc

SOURCE_FILES += ../nrf/spim.cc
SOURCE_FILES += ../nrf/spis.cc
SOURCE_FILES += ../nrf/spim_debug.cc
SOURCE_FILES += ../nrf/spis_debug.cc
SOURCE_FILES += ../nrf/timer.cc
SOURCE_FILES += ../nrf/timer_observer.cc
SOURCE_FILES += ../nrf/leds_pca10040.cc
SOURCE_FILES += ../nrf/gcc_startup_nrf52.s
SOURCE_FILES += ../nrf/gcc_stubs.cc

SOURCE_FILES += ../utilities/project_assert.cc
SOURCE_FILES += ../utilities/format_conversion.cc
SOURCE_FILES += ../utilities/int_to_string.cc
SOURCE_FILES += ../utilities/vwritef.cc
SOURCE_FILES += ../utilities/write_data.cc
SOURCE_FILES += ../utilities/std_stubs.cc
SOURCE_FILES += ../logger/logger.cc
SOURCE_FILES += ../logger/logger_c.cc
SOURCE_FILES += ../logger/segger_rtt_output_stream.cc
SOURCE_FILES += ../segger_rtt/SEGGER_RTT.c

# SDK source files:
#SOURCE_FILES += $(SDK_ROOT)/components/boards/boards.c
# SOURCE_FILES += $(SDK_ROOT)/components/libraries/button/app_button.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/util/app_error.c
#SOURCE_FILES += $(SDK_ROOT)/components/libraries/util/app_error_weak.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/util/app_util_platform.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/timer/app_timer.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/strerror/nrf_strerror.c
SOURCE_FILES += $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_gpiote.c

#SOURCE_FILES += $(SDK_ROOT)/components/drivers_nrf/common/nrf_drv_common.c
#SOURCE_FILES += $(SDK_ROOT)/components/drivers_nrf/gpiote/nrf_drv_gpiote.c
# SOURCE_FILES += $(SDK_ROOT)/components/drivers_nrf/uart/nrf_drv_uart.c
SOURCE_FILES += $(SDK_ROOT)/modules/nrfx/mdk/system_nrf52.c

# Include folders:
INCLUDE_PATHS += ./config/
INCLUDE_PATHS += $(BOOST_ROOT)/include
INCLUDE_PATHS += $(SDK_ROOT)/modules/nrfx/templates/nRF52832
INCLUDE_PATHS += $(SDK_ROOT)/modules/nrfx/templates
INCLUDE_PATHS += $(SDK_ROOT)/modules/nrfx
INCLUDE_PATHS += $(SDK_ROOT)/modules/nrfx/mdk
INCLUDE_PATHS += $(SDK_ROOT)/modules/nrfx/hal
INCLUDE_PATHS += $(SDK_ROOT)/modules/nrfx/drivers
INCLUDE_PATHS += $(SDK_ROOT)/modules/nrfx/drivers/include


INCLUDE_PATHS += $(SDK_ROOT)/components
INCLUDE_PATHS += $(SDK_ROOT)/components/boards
INCLUDE_PATHS += $(SDK_ROOT)/components/device
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/delay
# INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/delay
# INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/gpiote
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/hal
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/nrf_soc_nosd
# INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/spi_master
# INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/atomic
# INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/button
INCLUDE_PATHS += $(SDK_ROOT)/components/toolchain/cmsis/include

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

NORDIC_DEFS += -D SPIM0_ENABLED
NORDIC_DEFS += -D SPIS1_ENABLED
NORDIC_DEFS += -D TIMER0_ENABLED

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

include ../make_rules/arm_gcc_rules.mak
include ../make_rules/jlink_rules.mak

