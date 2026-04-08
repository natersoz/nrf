###
# nrf/nordic/test/buttons_leds_test/Makefile
#
# Copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
###

# Note: TARGET_NAME is required for the jlink rules to work.
TARGET_NAME			?= buttons_leds_test
PROJECT_ROOT		?= $(shell git rev-parse --show-toplevel)
BUILD_PATH			?= _build
BUILD_TYPE			?= Debug
VERBOSE				?= 0

ifeq ($(VERBOSE), 0)
    BUILD_VERBOSE=""
else
    BUILD_VERBOSE="--verbose"
endif

.DEFAULT_GOAL := all
TARGETS       := all relink clean

.PHONY: $(TARGETS) info scrub

$(TARGETS):
	cmake -S . -B $(BUILD_PATH) -D CMAKE_BUILD_TYPE=$(BUILD_TYPE) --toolchain $(PROJECT_ROOT)/Toolchain-gcc-arm-none-eabi.cmake
	cmake --build $(BUILD_PATH) --target $@ $(BUILD_VERBOSE)

scrub:
	rm -rf $(BUILD_PATH)

info:
	@echo "TARGET_NAME:  $(TARGET_NAME)"
	@echo "PROJECT_ROOT: $(PROJECT_ROOT)"
	@echo "BUILD_PATH:   $(BUILD_PATH)"

include $(PROJECT_ROOT)/make_rules/jlink_rules.mak
