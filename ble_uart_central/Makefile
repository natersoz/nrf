###
# Makefile for testing Nordic nRF52 BLE UART central device.
###

SDK_ROOT := ../sdk

# The executable name
TARGET_NAME = ble_uart_central

# Where the compiled files are built.
BUILD_PATH = _build

# Set for Debug or Release build types.
BUILD_TYPE	?= Debug

SOURCE_FILES =
SOURCE_FILES += $(SDK_ROOT)/components/libraries/experimental_log/src/nrf_log_backend_rtt.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/experimental_log/src/nrf_log_backend_serial.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/experimental_log/src/nrf_log_default_backends.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/experimental_log/src/nrf_log_frontend.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/experimental_log/src/nrf_log_str_formatter.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/button/app_button.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/util/app_error.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/util/app_error_weak.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/fifo/app_fifo.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/scheduler/app_scheduler.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/timer/app_timer.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/uart/app_uart_fifo.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/util/app_util_platform.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/hardfault/hardfault_implementation.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/util/nrf_assert.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/atomic_fifo/nrf_atfifo.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/balloc/nrf_balloc.c
SOURCE_FILES += $(SDK_ROOT)/external/fprintf/nrf_fprintf.c
SOURCE_FILES += $(SDK_ROOT)/external/fprintf/nrf_fprintf_format.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/experimental_memobj/nrf_memobj.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/pwr_mgmt/nrf_pwr_mgmt.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/experimental_section_vars/nrf_section_iter.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/strerror/nrf_strerror.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/uart/retarget.c
SOURCE_FILES += $(SDK_ROOT)/components/boards/boards.c
SOURCE_FILES += $(SDK_ROOT)/components/drivers_nrf/clock/nrf_drv_clock.c
SOURCE_FILES += $(SDK_ROOT)/components/drivers_nrf/common/nrf_drv_common.c
SOURCE_FILES += $(SDK_ROOT)/components/drivers_nrf/gpiote/nrf_drv_gpiote.c
SOURCE_FILES += $(SDK_ROOT)/components/drivers_nrf/uart/nrf_drv_uart.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/bsp/bsp.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/bsp/bsp_btn_ble.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/bsp/bsp_nfc.c
SOURCE_FILES += $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT.c
SOURCE_FILES += $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT_Syscalls_GCC.c
SOURCE_FILES += $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT_printf.c
SOURCE_FILES += $(SDK_ROOT)/components/ble/common/ble_advdata.c
SOURCE_FILES += $(SDK_ROOT)/components/ble/ble_db_discovery/ble_db_discovery.c
SOURCE_FILES += $(SDK_ROOT)/components/ble/common/ble_srv_common.c
SOURCE_FILES += $(SDK_ROOT)/components/ble/nrf_ble_gatt/nrf_ble_gatt.c
SOURCE_FILES += $(SDK_ROOT)/components/toolchain/gcc/gcc_startup_nrf52.S
SOURCE_FILES += $(SDK_ROOT)/components/toolchain/system_nrf52.c
SOURCE_FILES += $(SDK_ROOT)/components/ble/ble_services/ble_nus_c/ble_nus_c.c
SOURCE_FILES += $(SDK_ROOT)/components/softdevice/common/nrf_sdh.c
SOURCE_FILES += $(SDK_ROOT)/components/softdevice/common/nrf_sdh_ble.c
SOURCE_FILES += $(SDK_ROOT)/components/softdevice/common/nrf_sdh_soc.c
SOURCE_FILES += ./main.c

# Include folders common to all targets
INCLUDE_PATHS =
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/comp
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/twi_master
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_services/ble_ancs_c
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_services/ble_ias_c
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/pwm
INCLUDE_PATHS += $(SDK_ROOT)/components/softdevice/s132/headers/nrf52
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/usbd/class/cdc/acm
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/usbd/class/hid/generic
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/usbd/class/msc
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/usbd/class/hid
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_services/ble_lbs
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_services/ble_gls
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/fstorage
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/i2s
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/mutex
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/gpiote
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/experimental_log/src
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/gpiote
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/fifo
INCLUDE_PATHS += $(SDK_ROOT)/components/boards
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/experimental_memobj
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/common
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_advertising
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_services/ble_bas_c
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_services/ble_hrs_c
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/queue
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/pwr_mgmt
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_dtm
INCLUDE_PATHS += $(SDK_ROOT)/components/toolchain/cmsis/include
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_services/ble_rscs_c
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/uart
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/common
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_services/ble_lls
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/wdt
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/bsp
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_db_discovery
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_services/ble_bas
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/experimental_section_vars
INCLUDE_PATHS += $(SDK_ROOT)/components/softdevice/s132/headers
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_services/ble_ans_c
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/slip
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/mem_manager
INCLUDE_PATHS += $(SDK_ROOT)/external/segger_rtt
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/usbd/class/cdc
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/hal
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_services/ble_nus_c
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/rtc
INCLUDE_PATHS += $(SDK_ROOT)/components/softdevice/common
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_services/ble_ias
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/usbd/class/hid/mouse
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/ecc
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/ppi
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_services/ble_dfu
INCLUDE_PATHS += $(SDK_ROOT)/external/fprintf
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/twis_slave
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/atomic
INCLUDE_PATHS += $(SDK_ROOT)/components
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/scheduler
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/cli
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/experimental_log
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_services/ble_hts
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/delay
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/crc16
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/timer
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/util
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/pwm
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/csense_drv
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/csense
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/balloc
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/low_power_pwm
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/hardfault
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_services/ble_cscs
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/uart
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/hci
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/usbd/class/hid/kbd
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/spi_slave
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/lpcomp
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/timer
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/rng
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/power
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/usbd/config
INCLUDE_PATHS += $(SDK_ROOT)/components/toolchain
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/led_softblink
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/qdec
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_services/ble_cts_c
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/spi_master
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_services/ble_nus
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/twi_mngr
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_services/ble_hids
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/strerror
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/pdm
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/crc32
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/usbd/class/audio
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/peer_manager
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/swi
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_services/ble_tps
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_services/ble_dis
INCLUDE_PATHS += $(SDK_ROOT)/components/device
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/nrf_ble_gatt
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/nrf_ble_qwr
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/button
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/usbd
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/saadc
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/atomic_fifo
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_services/ble_lbs_c
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_racp
INCLUDE_PATHS += $(SDK_ROOT)/components/toolchain/gcc
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/fds
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/twi
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/clock
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_services/ble_rscs
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/usbd
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_services/ble_hrs
INCLUDE_PATHS += ./config

# Libraries common to all targets
LIB_FILES +=

# Optimization flags
OPT = -O3 -g3
# Uncomment the line below to enable link time optimization
#OPT += -flto

# C flags common to Nordic targets
NORDIC_DEFS += -DBOARD_PCA10040
NORDIC_DEFS += -DCONFIG_GPIO_AS_PINRESET
NORDIC_DEFS += -DFLOAT_ABI_HARD
NORDIC_DEFS += -DNRF52
NORDIC_DEFS += -DNRF52832_XXAA
NORDIC_DEFS += -DNRF52_PAN_74
NORDIC_DEFS += -DNRF_SD_BLE_API_VERSION=5
NORDIC_DEFS += -DS132
NORDIC_DEFS += -DSOFTDEVICE_PRESENT
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

