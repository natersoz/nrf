###
# nrf/nordic/test/spi_test/Makefile
#
# Copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
###

# Note: TARGET_NAME is required for the jlink rules to work.
TARGET_NAME			?= spi_test
PROJECT_ROOT		?= $(shell git rev-parse --show-toplevel)
BUILD_PATH			?= _build

.DEFAULT_GOAL := all
TARGETS       := all relink clean

.PHONY: $(TARGETS) info

$(TARGETS):
	cmake -S . -B $(BUILD_PATH) --toolchain $(PROJECT_ROOT)/Toolchain-gcc-arm-none-eabi.cmake
	cmake --build $(BUILD_PATH) --target $@

info:
	@echo "TARGET_NAME:  $(TARGET_NAME)"
	@echo "PROJECT_ROOT: $(PROJECT_ROOT)"
	@echo "BUILD_PATH:   $(BUILD_PATH)"

include $(PROJECT_ROOT)/make_rules/jlink_rules.mak
