###
# File: jlink_rules.mak
# @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
# Makefile rules for executing JLinkExe commands.
#
# Parameters:
#	BUILD_PATH	_build	The build output directory
#	TARGET_NAME		The binary file (elf) name,
#				determines the name of the .hex and .out files.
#	SDK_ROOT
#	SOFT_DEVICE	s132	The nRF52 Softdevice
#	NORDIC_DEVICE	NRF52	The NRF5x device name - for now only NRF52.
#
# Use target jlink-help for usage.
###

# Determine the Operating System:
OS_NAME				= $(shell 'uname')

# By default do not print out the toolchain details.
VERBOSE				?= @

NORDIC_DEVICE			?= NRF52
GDB_PORT			?= 2331
SWO_PORT			?= 2332
TELNET_PORT			?= 2333

SOFT_DEVICE			?= s132
SOFT_DEVICE_PATH		:= $(SDK_ROOT)/components/softdevice/$(SOFT_DEVICE)/hex
SOFT_DEVICE_HEX_FILE		:= $(shell find -L $(SOFT_DEVICE_PATH) -iname '*.hex')
SWD_SPEED			:= 96000

###
# Writing to FLASH is controlled via the Nordic Non-volatile Memory Controller (NVMC).
# See Product specification,  Chapter 11 NVMC http://infocenter.nordicsemi.com/index.jsp
# Nordic CONFIG, ERASEALLm ERASEUICR registers.
###
NVMC_REG_CONFIG			:= 0x4001e504
NVMC_REG_CONFIG_REN		:= 0
NVMC_REG_CONFIG_WEN		:= 1
NVMC_REG_CONFIG_EEN		:= 2

NVMC_REG_ERASEALL		:= 0x4001e50c
NVMC_REG_ERASEALL_NOOP 		:= 1
NVMC_REG_ERASEALL_ERASE		:= 1

NVMC_REG_ERASEUICR		:= 0x4001e514
NVMC_REG_ERASEUICR_NOOP 	:= 1
NVMC_REG_ERASEUICR_ERASE	:= 1

###
# OS specific JLink installation.
###
ifneq (,$(findstring Linux,$(OS_NAME)))
	SUDO              := sudo
	SEGGER_JLINK_ROOT := /opt/JLink
	JLINK_EXE         := "$(SEGGER_JLINK_ROOT)/JLinkExe"
	JLINK_GDB_SERVER  := "$(SEGGER_JLINK_ROOT)/JLinkGDBServerCLExe"
endif

ifneq (,$(findstring Darwin,$(OS_NAME)))
	SUDO              :=
	SEGGER_JLINK_ROOT := /Applications/SEGGER/JLink
	JLINK_EXE         := "$(SEGGER_JLINK_ROOT)/JLinkExe"
	JLINK_GDB_SERVER  := "$(SEGGER_JLINK_ROOT)/JLinkGDBServerCLExe"
endif

ifneq (,$(findstring CYGWIN,$(OS_NAME)))
	SUDO              :=
	SEGGER_JLINK_ROOT := "C:/Program\ Files\ \(x86\)/SEGGER/JLink_V510g"
	JLINK_EXE         := "$(SEGGER_JLINK_ROOT)"/JLink.exe
	JLINK_GDB_SERVER  := "$(SEGGER_JLINK_ROOT)"/JLinkGDBServerCLExe.exe
endif

###
# Segger Toolchain commands
###
JLINK_OPTS	+= -device $(NORDIC_DEVICE)
JLINK_OPTS	+= -if swd
JLINK_OPTS	+= -speed $(SWD_SPEED)

###
# Segger GDB Server
# -vd			Verify download data.
# -ir			Initialize CPU registers on start.
# -localhostonly	1: Allow locahost connections only.
#			0: Allow remote   connections.
# -strict		Exit on invalid parameters in start
###
JLINK_GDB_OPTS	+= -endian little
JLINK_GDB_OPTS	+= -port $(GDB_PORT)
JLINK_GDB_OPTS	+= -swoport $(SWO_PORT)
JLINK_GDB_OPTS	+= -telnetport $(TELNET_PORT)
JLINK_GDB_OPTS  += -RTTTelnetport $(RTT_PORT)
JLINK_GDB_OPTS	+= -vd
JLINK_GDB_OPTS	+= -noir
JLINK_GDB_OPTS	+= -localhostonly 1
JLINK_GDB_OPTS	+= -strict
JLINK_GDB_OPTS	+= -timeout 0

# If SEGGER_SN is defined then use it to specify the dev kit board to connect to.
ifneq (,$(SEGGER_SN))
JLINK_GDB_OPTS	+= -select USB=$(SEGGER_SN)
endif

.PHONY: gdb-server flash-all flash flash-softdevice flash-erase flash-erase-all flash-app-valid flash-app-invalid jlink-help

###
# gdb-server does not have any dependencies since you may
# want to attach to the debugger without resetting the target.
###
gdb-server:
	$(SUDO) $(JLINK_GDB_SERVER) $(JLINK_OPTS) $(JLINK_GDB_OPTS)

###
# JLink related flashing and debug init commands
###
flash-all: flash-softdevice flash

flash: $(BUILD_PATH)/flash_target.jlink
	$(SUDO) $(JLINK_EXE) $(JLINK_OPTS) $^
	$(VERBOSE) $(RM) $^

flash-softdevice: $(BUILD_PATH)/flash_softdevice.jlink
	$(SUDO) $(JLINK_EXE) $(JLINK_OPTS) $^
	$(VERBOSE) $(RM) $^

flash-erase: $(BUILD_PATH)/flash_erase.jlink
	$(SUDO) $(JLINK_EXE) $(JLINK_OPTS) $^
	$(VERBOSE) $(RM) $^

flash-erase-all: $(BUILD_PATH)/flash_erase_all.jlink
	$(SUDO) $(JLINK_EXE) $(JLINK_OPTS) $^
	$(VERBOSE) $(RM) $^

flash-app-valid: $(BUILD_PATH)/flash_app_valid.jlink
	$(SUDO) $(JLINK_EXE) $(JLINK_OPTS) $^
	$(VERBOSE) $(RM) $^

flash-app-invalid: $(BUILD_PATH)/flash_app_invalid.jlink
	$(SUDO) $(JLINK_EXE) $(JLINK_OPTS) $^
	$(VERBOSE) $(RM) $^

$(BUILD_PATH)/flash_target.jlink: $(BUILD_PATH)/$(TARGET_NAME).hex
	$(VERBOSE) printf "Fashing $<\n"
	$(VERBOSE) printf "if swd\n"                   > $@
	$(VERBOSE) printf "speed $(SWD_SPEED)\n"      >> $@
	$(VERBOSE) printf "device $(NORDIC_DEVICE)\n" >> $@
	$(VERBOSE) printf "loadfile $<\n"             >> $@
	$(VERBOSE) printf "r\n"                       >> $@
	$(VERBOSE) printf "exit\n"                    >> $@

$(BUILD_PATH)/flash_softdevice.jlink: | $(BUILD_PATH)
	$(VERBOSE) printf "Flashing $(SOFT_DEVICE_HEX_FILE)\n"
	$(VERBOSE) printf "speed $(SWD_SPEED)\n"                > $@
	$(VERBOSE) printf "loadfile $(SOFT_DEVICE_HEX_FILE)\n" >> $@
	$(VERBOSE) printf "exit\n"                             >> $@

$(BUILD_PATH)/flash_erase.jlink:
	$(VERBOSE) printf "erase\n"      > $@
	$(VERBOSE) printf "r\n"         >> $@
	$(VERBOSE) printf "exit\n"      >> $@

# Erase EVERYTHING.
$(BUILD_PATH)/flash_erase_all.jlink:
	$(VERBOSE) printf "Erasing all flash\n"
	$(VERBOSE) printf "w4 $(NVMC_REG_CONFIG)    $(NVMC_REG_CONFIG_EEN)\n"       > $@
	$(VERBOSE) printf "w4 $(NVMC_REG_ERASEALL)  $(NVMC_REG_ERASEALL_ERASE)\n"  >> $@
	$(VERBOSE) printf "exit\n"                                                 >> $@

# Indicate to the bootloader that the application is valid
$(BUILD_PATH)/flash_app_valid.jlink:
	$(VERBOSE) printf "h\n"                     > $@
	$(VERBOSE) printf "w4 0x0003fc00 0x0001\n" >> $@
	$(VERBOSE) printf "w4 0x0003fc04 0x0000\n" >> $@
	$(VERBOSE) printf "w4 0x0003fc08 0x00fe\n" >> $@
	$(VERBOSE) printf "w4 0x0003fc0c 0x0000\n" >> $@
	$(VERBOSE) printf "w4 0x0003fc10 0x0000\n" >> $@
	$(VERBOSE) printf "w4 0x0003fc14 0x0000\n" >> $@
	$(VERBOSE) printf "w4 0x0003fc18 0x0000\n" >> $@
	$(VERBOSE) printf "w4 0x0003fc1c 0x0000\n" >> $@
	$(VERBOSE) printf "r\n"                    >> $@
	$(VERBOSE) printf "exit\n"                 >> $@

# Indicate to the bootloader that the application is not valid
$(BUILD_PATH)/flash_app_invalid.jlink:
	$(VERBOSE) printf "h\n"                     > $@
	$(VERBOSE) printf "w4 0x0003fc00 0x0000\n" >> $@
	$(VERBOSE) printf "w4 0x0003fc04 0x0000\n" >> $@
	$(VERBOSE) printf "w4 0x0003fc08 0x00fe\n" >> $@
	$(VERBOSE) printf "w4 0x0003fc0c 0x0000\n" >> $@
	$(VERBOSE) printf "w4 0x0003fc10 0x0000\n" >> $@
	$(VERBOSE) printf "w4 0x0003fc14 0x0000\n" >> $@
	$(VERBOSE) printf "w4 0x0003fc18 0x0000\n" >> $@
	$(VERBOSE) printf "w4 0x0003fc1c 0x0000\n" >> $@
	$(VERBOSE) printf "r\n"                    >> $@
	$(VERBOSE) printf "exit\n"                 >> $@

jlink-help:
	@printf "targets:\n"
	@printf "\tflash-all       : Flash the build target and softdevice\n"
	@printf "\tflash           : Flash the build target\n"
	@printf "\tflash-softdevice: Flash the softdevice\n"
	@printf "\tflash-erase:    : Erase the program flash\n"
	@printf "\tflash-erase-all : Erase all flash using special Nordic registers\n"
	@printf "\tgdb-server      : Start the gdb server - resets the device\n"
	@printf "\n"

jlink-info:
	@printf "NORDIC_DEVICE        = $(NORDIC_DEVICE)\n"
	@printf "SOFT_DEVICE_HEX_FILE = $(SOFT_DEVICE_HEX_FILE)\n"
	@printf "OS_NAME              = $(OS_NAME)\n"
