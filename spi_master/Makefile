###
# Makefile for testing Nordic nRF52 SPI interface.
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
SOURCE_FILES += $(SDK_ROOT)/components/libraries/experimental_log/src/nrf_log_backend_rtt.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/experimental_log/src/nrf_log_backend_serial.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/experimental_log/src/nrf_log_backend_uart.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/experimental_log/src/nrf_log_default_backends.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/experimental_log/src/nrf_log_frontend.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/experimental_log/src/nrf_log_str_formatter.c
SOURCE_FILES += $(SDK_ROOT)/components/boards/boards.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/button/app_button.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/util/app_error.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/util/app_error_weak.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/scheduler/app_scheduler.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/timer/app_timer.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/util/app_util_platform.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/util/nrf_assert.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/balloc/nrf_balloc.c
SOURCE_FILES += $(SDK_ROOT)/external/fprintf/nrf_fprintf.c
SOURCE_FILES += $(SDK_ROOT)/external/fprintf/nrf_fprintf_format.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/experimental_memobj/nrf_memobj.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/strerror/nrf_strerror.c
SOURCE_FILES += $(SDK_ROOT)/components/drivers_nrf/common/nrf_drv_common.c
SOURCE_FILES += $(SDK_ROOT)/components/drivers_nrf/gpiote/nrf_drv_gpiote.c
SOURCE_FILES += $(SDK_ROOT)/components/drivers_nrf/spi_master/nrf_drv_spi.c
SOURCE_FILES += $(SDK_ROOT)/components/drivers_nrf/uart/nrf_drv_uart.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/bsp/bsp.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/bsp/bsp_nfc.c
SOURCE_FILES += $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT.c
SOURCE_FILES += $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT_Syscalls_GCC.c
SOURCE_FILES += $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT_printf.c
SOURCE_FILES += $(SDK_ROOT)/components/toolchain/gcc/gcc_startup_nrf52.S
SOURCE_FILES += $(SDK_ROOT)/components/toolchain/system_nrf52.c
SOURCE_FILES += ./main.c
SOURCE_FILES += ../common/gcc_startup_nrf52.s

# Include folders common to all targets
INCLUDE_PATHS =
INCLUDE_PATHS += $(SDK_ROOT)/components
INCLUDE_PATHS += $(SDK_ROOT)/components/boards
INCLUDE_PATHS += $(SDK_ROOT)/components/device
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/common
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/delay
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/gpiote
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/hal
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/nrf_soc_nosd
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/spi_master
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/uart
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/atomic
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/balloc
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/bsp
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/button
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/experimental_log
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/experimental_log/src
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/experimental_memobj
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/experimental_section_vars
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/scheduler
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/strerror
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/timer
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/util
INCLUDE_PATHS += $(SDK_ROOT)/components/toolchain
INCLUDE_PATHS += $(SDK_ROOT)/components/toolchain/cmsis/include
INCLUDE_PATHS += $(SDK_ROOT)/components/toolchain/gcc
INCLUDE_PATHS += $(SDK_ROOT)/external/fprintf
INCLUDE_PATHS += $(SDK_ROOT)/external/segger_rtt
INCLUDE_PATHS +=  ./config

# Libraries can be added here, includes path: path/filename.a
# todo : unused so far.
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

