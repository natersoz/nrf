###
# Makefile for the BLE peripheral class project
###

# The root directory of this project.
PROJECT_ROOT := ..

# Location of the Nordic SDK
SDK_ROOT     := $(PROJECT_ROOT)/sdk

# The executable name
TARGET_NAME = ble_peripheral

GDB_PORT			?= 2331
SWO_PORT			?= 2332
TELNET_PORT			?= 2333
RTT_PORT			?= 2334

# Include the file which defines the SEGGER_SN symbol
# If it does not exist, debugging ble_central and ble_peripheral simultaneously
# will be less convenient (although it can be done).
-include .segger_sn

# git version symbols GIT_HASH and GIT_TAG
include $(PROJECT_ROOT)/make_rules/version_git.mak

# Where the compiled files are built.
BUILD_PATH = _build

# Set for Debug or Release build types.
BUILD_TYPE	?= Debug

# Path to boost installation.
BOOST_ROOT	= ../external/boost

SOURCE_FILES =
SOURCE_FILES += ./main.cc
SOURCE_FILES += ./ble_peripheral_init.cc
SOURCE_FILES += ./ble_gap_connection.cc
SOURCE_FILES += ./ble_gatts_observer.cc
SOURCE_FILES += ./nordic_saadc_sensor_acquisition.cc
SOURCE_FILES += $(PROJECT_ROOT)/gcc-arm/gcc_startup_nrf52.s
SOURCE_FILES += $(PROJECT_ROOT)/gcc-arm/gcc_stubs.cc
SOURCE_FILES += $(PROJECT_ROOT)/gcc-arm/stack_usage.cc
SOURCE_FILES += $(PROJECT_ROOT)/nordic/app_error_fault_handler.cc
SOURCE_FILES += $(PROJECT_ROOT)/nordic/nordic_critical_section.cc
SOURCE_FILES += $(PROJECT_ROOT)/nordic/peripherals/buttons_pca10040.cc
SOURCE_FILES += $(PROJECT_ROOT)/nordic/peripherals/clocks.cc
SOURCE_FILES += $(PROJECT_ROOT)/nordic/peripherals/gpio.cc
SOURCE_FILES += $(PROJECT_ROOT)/nordic/peripherals/leds_pca10040.cc
SOURCE_FILES += $(PROJECT_ROOT)/nordic/peripherals/mwu.cc
SOURCE_FILES += $(PROJECT_ROOT)/nordic/peripherals/mwu_debug.cc
SOURCE_FILES += $(PROJECT_ROOT)/nordic/peripherals/ppi.cc
SOURCE_FILES += $(PROJECT_ROOT)/nordic/peripherals/rtc.cc
SOURCE_FILES += $(PROJECT_ROOT)/nordic/peripherals/saadc.cc
SOURCE_FILES += $(PROJECT_ROOT)/nordic/peripherals/timer.cc

SOURCE_FILES += $(PROJECT_ROOT)/ble/gap_connection.cc
SOURCE_FILES += $(PROJECT_ROOT)/ble/gap_event_logger.cc
SOURCE_FILES += $(PROJECT_ROOT)/ble/gap_connection_negotiation_state.cc
SOURCE_FILES += $(PROJECT_ROOT)/ble/gatt_attribute.cc
SOURCE_FILES += $(PROJECT_ROOT)/ble/gatt_declaration.cc
SOURCE_FILES += $(PROJECT_ROOT)/ble/gatt_service.cc
SOURCE_FILES += $(PROJECT_ROOT)/ble/gatt_service_container.cc
SOURCE_FILES += $(PROJECT_ROOT)/ble/gatt_service_discovery_iterator.cc
SOURCE_FILES += $(PROJECT_ROOT)/ble/gatts_event_observer.cc
SOURCE_FILES += $(PROJECT_ROOT)/ble/nordic_ble_att.cc
SOURCE_FILES += $(PROJECT_ROOT)/ble/nordic_ble_common_event_observable.cc
SOURCE_FILES += $(PROJECT_ROOT)/ble/nordic_ble_common_event_observer.cc
SOURCE_FILES += $(PROJECT_ROOT)/ble/nordic_ble_event_observables.cc
SOURCE_FILES += $(PROJECT_ROOT)/ble/nordic_ble_event_strings.cc
SOURCE_FILES += $(PROJECT_ROOT)/ble/nordic_ble_gap_advertising.cc
SOURCE_FILES += $(PROJECT_ROOT)/ble/nordic_ble_gap_event_observable.cc
SOURCE_FILES += $(PROJECT_ROOT)/ble/nordic_ble_gap_logger.cc
SOURCE_FILES += $(PROJECT_ROOT)/ble/nordic_ble_gap_operations.cc
SOURCE_FILES += $(PROJECT_ROOT)/ble/nordic_ble_gattc_event_observable.cc
SOURCE_FILES += $(PROJECT_ROOT)/ble/nordic_ble_gattc_discovery_observable.cc
SOURCE_FILES += $(PROJECT_ROOT)/ble/nordic_ble_gatts.cc
SOURCE_FILES += $(PROJECT_ROOT)/ble/nordic_ble_gatts_event_observable.cc
SOURCE_FILES += $(PROJECT_ROOT)/ble/nordic_ble_gatts_operations.cc
SOURCE_FILES += $(PROJECT_ROOT)/ble/nordic_ble_peer.cc
SOURCE_FILES += $(PROJECT_ROOT)/ble/nordic_ble_stack.cc
SOURCE_FILES += $(PROJECT_ROOT)/ble/nordic_ble_stack_observer.cc
SOURCE_FILES += $(PROJECT_ROOT)/ble/nordic_error.cc
SOURCE_FILES += $(PROJECT_ROOT)/ble/nordic_req_observable.cc
SOURCE_FILES += $(PROJECT_ROOT)/ble/nordic_soc_observable.cc
SOURCE_FILES += $(PROJECT_ROOT)/ble/nordic_state_observable.cc
SOURCE_FILES += $(PROJECT_ROOT)/ble/service/battery_service.cc
SOURCE_FILES += $(PROJECT_ROOT)/ble/service/custom_uuid.cc
SOURCE_FILES += $(PROJECT_ROOT)/ble/ltv_encode.cc
SOURCE_FILES += $(PROJECT_ROOT)/ble/uuid.cc
SOURCE_FILES += $(PROJECT_ROOT)/utility/format_conversion.cc
SOURCE_FILES += $(PROJECT_ROOT)/utility/gregorian.cc
SOURCE_FILES += $(PROJECT_ROOT)/utility/int_to_string.cc
SOURCE_FILES += $(PROJECT_ROOT)/utility/project_assert.cc
SOURCE_FILES += $(PROJECT_ROOT)/utility/std_stubs.cc
SOURCE_FILES += $(PROJECT_ROOT)/utility/vwritef.cc
SOURCE_FILES += $(PROJECT_ROOT)/utility/write_data.cc
SOURCE_FILES += $(PROJECT_ROOT)/utility/version_info.c
SOURCE_FILES += $(PROJECT_ROOT)/logger/logger.cc
SOURCE_FILES += $(PROJECT_ROOT)/logger/logger_c.cc
SOURCE_FILES += $(PROJECT_ROOT)/logger/rtt_output_stream.cc
SOURCE_FILES += $(PROJECT_ROOT)/segger/segger_rtt.cc

# These have been dumped - reminder to look inside before dumping.
# SOURCE_FILES += $(SDK_ROOT)/components/ble/common/ble_conn_params.c
# SOURCE_FILES += $(SDK_ROOT)/components/ble/common/ble_srv_common.c
# SOURCE_FILES += $(SDK_ROOT)/components/libraries/util/sdk_mapped_flags.c

# Required by peer manager, but hope to dump later.
SOURCE_FILES += $(SDK_ROOT)/components/libraries/util/app_error.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/util/app_error_weak.c
SOURCE_FILES += $(SDK_ROOT)/components/ble/common/ble_conn_state.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/strerror/nrf_strerror.c

# Peer manager and required dependencies:
SOURCE_FILES += $(SDK_ROOT)/components/ble/peer_manager/gatt_cache_manager.c
SOURCE_FILES += $(SDK_ROOT)/components/ble/peer_manager/gatts_cache_manager.c
SOURCE_FILES += $(SDK_ROOT)/components/ble/peer_manager/id_manager.c
SOURCE_FILES += $(SDK_ROOT)/components/ble/peer_manager/peer_data_storage.c
SOURCE_FILES += $(SDK_ROOT)/components/ble/peer_manager/peer_database.c
SOURCE_FILES += $(SDK_ROOT)/components/ble/peer_manager/peer_id.c
SOURCE_FILES += $(SDK_ROOT)/components/ble/peer_manager/peer_manager.c
SOURCE_FILES += $(SDK_ROOT)/components/ble/peer_manager/pm_buffer.c
SOURCE_FILES += $(SDK_ROOT)/components/ble/peer_manager/security_dispatcher.c
SOURCE_FILES += $(SDK_ROOT)/components/ble/peer_manager/security_manager.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/atomic/nrf_atomic.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/atomic_fifo/nrf_atfifo.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/atomic_flags/nrf_atflags.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/crc16/crc16.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/fds/fds.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/fstorage/nrf_fstorage.c
SOURCE_FILES += $(SDK_ROOT)/components/libraries/fstorage/nrf_fstorage_sd.c
SOURCE_FILES += $(SDK_ROOT)/modules/nrfx/mdk/system_nrf52.c

# How Nordic deals with linker sections.
SOURCE_FILES += $(SDK_ROOT)/components/libraries/experimental_section_vars/nrf_section_iter.c
SOURCE_FILES += $(SDK_ROOT)/components/softdevice/common/nrf_sdh.c

INCLUDE_PATHS =  .
INCLUDE_PATHS =  $(PROJECT_ROOT)
INCLUDE_PATHS += $(PROJECT_ROOT)/gcc-arm
INCLUDE_PATHS += $(PROJECT_ROOT)/logger
INCLUDE_PATHS += $(PROJECT_ROOT)/nordic
INCLUDE_PATHS += $(PROJECT_ROOT)/nordic/peripherals
INCLUDE_PATHS += $(PROJECT_ROOT)/segger
INCLUDE_PATHS += $(PROJECT_ROOT)/utility

INCLUDE_PATHS += ./config
INCLUDE_PATHS += $(BOOST_ROOT)

# Nordic SDK include paths

SYSTEM_INCLUDE_PATHS += $(SDK_ROOT)/components/ble/common
SYSTEM_INCLUDE_PATHS += $(SDK_ROOT)/components/ble/peer_manager
# SYSTEM_INCLUDE_PATHS += $(SDK_ROOT)/components/drivers_nrf/hal
SYSTEM_INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/atomic
SYSTEM_INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/atomic_fifo
SYSTEM_INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/atomic_flags
SYSTEM_INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/crc16
SYSTEM_INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/crc32
SYSTEM_INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/experimental_section_vars
SYSTEM_INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/fds
SYSTEM_INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/fstorage
SYSTEM_INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/log
SYSTEM_INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/mutex
SYSTEM_INCLUDE_PATHS += $(SDK_ROOT)/modules/nrfx/mdk

# These would be nice to dump:
SYSTEM_INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/strerror
SYSTEM_INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/util

SYSTEM_INCLUDE_PATHS += $(SDK_ROOT)/components/softdevice/common
SYSTEM_INCLUDE_PATHS += $(SDK_ROOT)/components/softdevice/s132/headers
SYSTEM_INCLUDE_PATHS += $(SDK_ROOT)/components/softdevice/s132/headers/nrf52
SYSTEM_INCLUDE_PATHS += $(SDK_ROOT)/components/toolchain
SYSTEM_INCLUDE_PATHS += $(SDK_ROOT)/components/toolchain/cmsis/include

# These might be interesting, but not required:
# SYSTEM_INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/mutex
# SYSTEM_INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/queue
# SYSTEM_INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/hardfault
# SYSTEM_INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/ecc
# SYSTEM_INCLUDE_PATHS += $(SDK_ROOT)/components/libraries/hci

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

NORDIC_DEFS += -D NRF_SD_BLE_API_VERSION=6
NORDIC_DEFS += -D S132
NORDIC_DEFS += -D SOFTDEVICE_PRESENT=1

NORDIC_DEFS += -D CONFIG_GPIO_AS_PINRESET
NORDIC_DEFS += -D SWI_DISABLE0

NORDIC_DEFS += -D RTC1_ENABLED
NORDIC_DEFS += -D TIMER1_ENABLED

CXXFLAGS    += $(NORDIC_DEFS)
CFLAGS      += $(NORDIC_DEFS)

CXXFLAGS    += -D GIT_HASH=$(GIT_HASH)
CFLAGS      += -D GIT_HASH=$(GIT_HASH)

CXXFLAGS    += -D GIT_TAG=$(GIT_TAG)
CFLAGS      += -D GIT_TAG=$(GIT_TAG)

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
	@echo "INCLUDE_PATHS"     ="$(INCLUDE_PATHS)"

include $(PROJECT_ROOT)/make_rules/arm_gcc_rules.mak
include $(PROJECT_ROOT)/make_rules/checker_rules.mak
include $(PROJECT_ROOT)/make_rules/jlink_rules.mak
