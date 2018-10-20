
PROJECT_ROOT := ..
SDK_ROOT     := $(PROJECT_ROOT)/sdk

# The executable name
TARGET_NAME = ble_peripheral_thermometer

# Where the compiled files are built.
BUILD_PATH = _build

# Set for Debug or Release build types.
BUILD_TYPE	?= Debug

# Path to boot installation.
BOOST_ROOT=/opt/boost

SOURCE_FILES =
SOURCE_FILES += ./main.cc
SOURCE_FILES += $(PROJECT_ROOT)/gcc-arm/gcc_startup_nrf52.s
SOURCE_FILES += $(PROJECT_ROOT)/gcc-arm/gcc_stubs.cc
SOURCE_FILES += $(PROJECT_ROOT)/nordic/app_timer.cc
SOURCE_FILES += $(PROJECT_ROOT)/nordic/peripherals/buttons_pca10040.cc
SOURCE_FILES += $(PROJECT_ROOT)/nordic/peripherals/clocks.cc
SOURCE_FILES += $(PROJECT_ROOT)/nordic/peripherals/leds_pca10040.cc
SOURCE_FILES += $(PROJECT_ROOT)/nordic/peripherals/rtc.cc
SOURCE_FILES += $(PROJECT_ROOT)/nordic/peripherals/gpio.cc

SOURCE_FILES += $(PROJECT_ROOT)/utilities/project_assert.cc
SOURCE_FILES += $(PROJECT_ROOT)/utilities/format_conversion.cc
SOURCE_FILES += $(PROJECT_ROOT)/utilities/int_to_string.cc
SOURCE_FILES += $(PROJECT_ROOT)/utilities/vwritef.cc
SOURCE_FILES += $(PROJECT_ROOT)/utilities/write_data.cc
SOURCE_FILES += $(PROJECT_ROOT)/utilities/std_stubs.cc
SOURCE_FILES += $(PROJECT_ROOT)/logger/logger.cc
SOURCE_FILES += $(PROJECT_ROOT)/logger/logger_c.cc
SOURCE_FILES += $(PROJECT_ROOT)/logger/segger_rtt_output_stream.cc
SOURCE_FILES += $(PROJECT_ROOT)/segger_rtt/SEGGER_RTT.c

SOURCE_FILES += $(SDK_ROOT)/components/ble/ble_services/ble_bas/ble_bas.c
SOURCE_FILES += $(SDK_ROOT)/components/ble/ble_services/ble_dis/ble_dis.c
SOURCE_FILES += $(SDK_ROOT)/components/ble/ble_services/ble_hts/ble_hts.c
SOURCE_FILES += $(SDK_ROOT)/components/ble/nrf_ble_gatt/nrf_ble_gatt.c

SOURCE_FILES += $(SDK_ROOT)/components/ble/ble_advertising/ble_advertising.c
SOURCE_FILES += $(SDK_ROOT)/components/ble/common/ble_advdata.c

SOURCE_FILES += $(SDK_ROOT)/components/ble/common/ble_conn_params.c
SOURCE_FILES += $(SDK_ROOT)/components/ble/common/ble_conn_state.c
SOURCE_FILES += $(SDK_ROOT)/components/ble/common/ble_srv_common.c
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
# SOURCE_FILES += $(SDK_ROOT)/components/drivers_nrf/clock/nrf_drv_clock.c
SOURCE_FILES += $(SDK_ROOT)/components/drivers_nrf/common/nrf_drv_common.c
# SOURCE_FILES += $(SDK_ROOT)/components/drivers_nrf/gpiote/nrf_drv_gpiote.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/atomic_fifo/nrf_atfifo.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/crc16/crc16.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/experimental_section_vars/nrf_section_iter.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/fds/fds.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/fstorage/nrf_fstorage.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/fstorage/nrf_fstorage_sd.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/hardfault/hardfault_implementation.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/sensorsim/sensorsim.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/strerror/nrf_strerror.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/util/app_error.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/util/app_error_weak.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/util/app_util_platform.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/util/sdk_mapped_flags.c
SOURCE_FILES += $(SDK_ROOT)/components/softdevice/common/nrf_sdh.c
SOURCE_FILES += $(SDK_ROOT)/components/softdevice/common/nrf_sdh_ble.c
SOURCE_FILES += $(SDK_ROOT)/components/softdevice/common/nrf_sdh_soc.c
SOURCE_FILES += $(SDK_ROOT)/components/toolchain/gcc/gcc_startup_nrf52.S
SOURCE_FILES += $(SDK_ROOT)/components/toolchain/system_nrf52.c

INCLUDE_PATHS =  .
INCLUDE_PATHS += $(PROJECT_ROOT)/gcc-arm
INCLUDE_PATHS += $(PROJECT_ROOT)/logger
INCLUDE_PATHS += $(PROJECT_ROOT)/nordic
INCLUDE_PATHS += $(PROJECT_ROOT)/nordic/peripherals
INCLUDE_PATHS += $(PROJECT_ROOT)/segger_rtt
INCLUDE_PATHS += $(PROJECT_ROOT)/utilities

INCLUDE_PATHS += ./config
INCLUDE_PATHS += $(BOOST_ROOT)/include

INCLUDE_PATHS += $(SDK_ROOT)/components
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_advertising
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_racp
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_services/ble_bas
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_services/ble_dis
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/ble_services/ble_hts
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/common
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/nrf_ble_gatt
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/nrf_ble_qwr
INCLUDE_PATHS += $(SDK_ROOT)/components/ble/peer_manager
INCLUDE_PATHS += $(SDK_ROOT)/components/boards
INCLUDE_PATHS += $(SDK_ROOT)/components/device
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/common
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/comp
INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/hal
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/atomic
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/atomic_fifo
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/crc16
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/crc32
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/csense
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/csense_drv
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/ecc
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/experimental_log
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/experimental_log/src
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/experimental_memobj
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/experimental_section_vars
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/fds
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/fstorage
# INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/gpiote
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/hardfault
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/hci
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/led_softblink
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/low_power_pwm
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/mem_manager
INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/pwm
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
INCLUDE_PATHS += $(SDK_ROOT)/external/fprintf
INCLUDE_PATHS += $(SDK_ROOT)/external/segger_rtt

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

include $(PROJECT_ROOT)/make_rules/arm_gcc_rules.mak
include $(PROJECT_ROOT)/make_rules/jlink_rules.mak

