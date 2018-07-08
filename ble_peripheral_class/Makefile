###
# Makefile for the BLE peripheral class project
###

SDK_ROOT := ../sdk

# The executable name
TARGET_NAME = ble_peripheral_class

# Where the compiled files are built.
BUILD_PATH = _build

# Set for Debug or Release build types.
BUILD_TYPE	?= Debug

# Path to boot installation. Using 1.65.1
BOOST_ROOT=/opt/boost

SOURCE_FILES =
SOURCE_FILES += ./main.cc
#SOURCE_FILES += ./advertising.cc
SOURCE_FILES += ../gcc-arm/gcc_startup_nrf52.s
SOURCE_FILES += ../gcc-arm/gcc_stubs.cc
SOURCE_FILES += ../nordic/app_timer.cc
SOURCE_FILES += ../peripherals/buttons_pca10040.cc
SOURCE_FILES += ../peripherals/clocks.cc
SOURCE_FILES += ../peripherals/leds_pca10040.cc
SOURCE_FILES += ../peripherals/rtc.cc

SOURCE_FILES += ../ble/uuid.cc
SOURCE_FILES += ../ble/tlv_encode.cc
SOURCE_FILES += ../ble/advertising.cc
SOURCE_FILES += ../ble/service/battery_service.cc
SOURCE_FILES += ../ble/nordic_gatts.cc
SOURCE_FILES += ../ble/nordic_softdevice_init.cc
SOURCE_FILES += ../ble/nordic_state_observable.cc
SOURCE_FILES += ../ble/nordic_req_observable.cc
SOURCE_FILES += ../ble/nordic_soc_observable.cc
SOURCE_FILES += ../ble/nordic_ble_event_observables.cc
SOURCE_FILES += ../ble/nordic_ble_gap_event_observable.cc
SOURCE_FILES += ../ble/nordic_ble_gatts_event_observable.cc
SOURCE_FILES += ../ble/nordic_ble_gattc_event_observable.cc
SOURCE_FILES += ../ble/nordic_ble_common_event_observable.cc
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

SOURCE_FILES += $(SDK_ROOT)/components/ble/ble_advertising/ble_advertising.c
SOURCE_FILES += $(SDK_ROOT)/components/ble/common/ble_advdata.c
SOURCE_FILES += $(SDK_ROOT)/components/ble/common/ble_conn_params.c
SOURCE_FILES += $(SDK_ROOT)/components/ble/common/ble_conn_state.c
SOURCE_FILES += $(SDK_ROOT)/components/ble/common/ble_srv_common.c
SOURCE_FILES += $(SDK_ROOT)/components/ble/nrf_ble_gatt/nrf_ble_gatt.c
SOURCE_FILES += $(SDK_ROOT)/components/ble/peer_manager/gatt_cache_manager.c
SOURCE_FILES += $(SDK_ROOT)/components/ble/peer_manager/gatts_cache_manager.c
SOURCE_FILES += $(SDK_ROOT)/components/ble/peer_manager/id_manager.c
SOURCE_FILES += $(SDK_ROOT)/components/ble/peer_manager/peer_data_storage.c
SOURCE_FILES += $(SDK_ROOT)/components/ble/peer_manager/peer_database.c
SOURCE_FILES += $(SDK_ROOT)/components/ble/peer_manager/peer_id.c
SOURCE_FILES += $(SDK_ROOT)/components/ble/peer_manager/peer_manager.c
SOURCE_FILES += $(SDK_ROOT)/components/ble/peer_manager/pm_buffer.c
SOURCE_FILES += $(SDK_ROOT)/components/ble/peer_manager/pm_mutex.c
SOURCE_FILES += $(SDK_ROOT)/components/ble/peer_manager/security_dispatcher.c
SOURCE_FILES += $(SDK_ROOT)/components/ble/peer_manager/security_manager.c
SOURCE_FILES += $(SDK_ROOT)/components/boards/boards.c
#SOURCE_FILES += $(SDK_ROOT)/components/drivers_nrf/clock/nrf_drv_clock.c
SOURCE_FILES += $(SDK_ROOT)/components/drivers_nrf/common/nrf_drv_common.c
SOURCE_FILES += $(SDK_ROOT)/components/drivers_nrf/gpiote/nrf_drv_gpiote.c
SOURCE_FILES += $(SDK_ROOT)/components/drivers_nrf/uart/nrf_drv_uart.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/atomic_fifo/nrf_atfifo.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/crc16/crc16.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/experimental_section_vars/nrf_section_iter.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/fds/fds.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/fstorage/nrf_fstorage.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/fstorage/nrf_fstorage_sd.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/hardfault/hardfault_implementation.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/pwr_mgmt/nrf_pwr_mgmt.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/scheduler/app_scheduler.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/strerror/nrf_strerror.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/util/app_error.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/util/app_error_weak.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/util/app_util_platform.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/util/nrf_assert.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/util/sdk_mapped_flags.c
SOURCE_FILES += $(SDK_ROOT)/components/softdevice/common/nrf_sdh.c
SOURCE_FILES += $(SDK_ROOT)/components/softdevice/common/nrf_sdh_ble.c
SOURCE_FILES += $(SDK_ROOT)/components/softdevice/common/nrf_sdh_soc.c
SOURCE_FILES += $(SDK_ROOT)/components/toolchain/system_nrf52.c

INCLUDE_PATHS =  .
INCLUDE_PATHS =  ..
INCLUDE_PATHS += ../gcc-arm
INCLUDE_PATHS += ../logger
INCLUDE_PATHS += ../nordic
INCLUDE_PATHS += ../peripherals
INCLUDE_PATHS += ../segger_rtt
INCLUDE_PATHS += ../utilities

INCLUDE_PATHS += ./config
INCLUDE_PATHS += $(BOOST_ROOT)/include

# Nordic SDK include paths
INCLUDE_PATHS += $(SDK_ROOT)/components
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_advertising
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_dtm
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_racp
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_services/ble_ancs_c
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_services/ble_ans_c
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_services/ble_bas
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_services/ble_bas_c
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_services/ble_cscs
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_services/ble_cts_c
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_services/ble_dfu
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_services/ble_dis
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_services/ble_gls
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_services/ble_hids
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_services/ble_hrs
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_services/ble_hrs_c
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_services/ble_hts
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_services/ble_ias
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_services/ble_ias_c
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_services/ble_lbs
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_services/ble_lbs_c
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_services/ble_lls
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_services/ble_nus
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_services/ble_nus_c
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_services/ble_rscs
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_services/ble_rscs_c
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_services/ble_tps
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/common
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/nrf_ble_gatt
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/nrf_ble_qwr
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/peer_manager
INCLUDE_PATHS += $(SDK_ROOT)/components/boards
INCLUDE_PATHS += $(SDK_ROOT)/components/device
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/clock
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/common
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/comp
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/delay
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/gpiote
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/hal
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/i2s
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/lpcomp
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/pdm
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/power
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/ppi
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/pwm
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/qdec
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/rng
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/rtc
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/saadc
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/spi_master
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/spi_slave
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/swi
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/timer
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/twi_master
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/twis_slave
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/uart
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/usbd
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/wdt
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/atomic
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/atomic_fifo
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/balloc
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/bsp
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/button
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/cli
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/crc16
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/crc32
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/csense
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/csense_drv
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/ecc
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/experimental_memobj
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/experimental_section_vars
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/fds
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/fstorage
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/gpiote
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/hardfault
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/hci
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/led_softblink
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/low_power_pwm
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/mem_manager
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/mutex
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/pwm
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/pwr_mgmt
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/queue
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/scheduler
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/sensorsim
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/slip
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/strerror
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/timer
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/twi
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/twi_mngr
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/uart
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/usbd
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/usbd/class/audio
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/usbd/class/cdc
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/usbd/class/cdc/acm
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/usbd/class/hid
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/usbd/class/hid/generic
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/usbd/class/hid/kbd
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/usbd/class/hid/mouse
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/usbd/class/msc
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/usbd/config
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/util
INCLUDE_PATHS += $(SDK_ROOT)/components/softdevice/common
INCLUDE_PATHS += $(SDK_ROOT)/components/softdevice/s132/headers
INCLUDE_PATHS += $(SDK_ROOT)/components/softdevice/s132/headers/nrf52
INCLUDE_PATHS += $(SDK_ROOT)/components/toolchain
INCLUDE_PATHS += $(SDK_ROOT)/components/toolchain/cmsis/include
INCLUDE_PATHS += $(SDK_ROOT)/components/toolchain/gcc

###
# Use the LD_PATHS to allow the linker to bring in common linker scripts
# The assumption is that projects are at the saem dieectory level as common.
###
LD_PATHS   = ../nordic

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

NORDIC_DEFS += -D NRF_SD_BLE_API_VERSION=5
NORDIC_DEFS += -D S132
NORDIC_DEFS += -D SOFTDEVICE_PRESENT

NORDIC_DEFS += -D CONFIG_GPIO_AS_PINRESET
NORDIC_DEFS += -D SWI_DISABLE0

NORDIC_DEFS += -D RTC1_ENABLED

CXXFLAGS    += $(NORDIC_DEFS)
CFLAGS      += $(NORDIC_DEFS)
CXXFLAGS    += $(NORDIC_DEFS)

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

