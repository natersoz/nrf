###
# nrf/external/Makefile
# Obtain and configure external dependencies.
# Copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
###

BOOST_VERSION   := 1.90.0
NRF_SDK_VERSION := 17.1.0_ddde560

NRF_SDK_TARGET_DIR := nRF5_SDK_$(NRF_SDK_VERSION)
NRF_SDK_ZIP_FILE   := nrf5_sdk_$(NRF_SDK_VERSION).zip

# Change boost version X.Y.Z => X_Y_Z
BOOST_VERSION_UNDER := $(shell echo $(BOOST_VERSION) | tr '.' '_')

.PHONY: all scrub

NRF_SDK_DOWNLOAD_URL := https://nsscprodmedia.blob.core.windows.net/prod/software-and-other-downloads/sdks/nrf5/binaries/nrf5_sdk_17.1.0_ddde560.zip
all: downloads nrf_sdk boost

SDK_MODIFIED_FILES_DIR = ../nordic/sdk-modified/

downloads:
	mkdir $@

# Download the Nordic SDK, unzip it, and move some hated files out of the way.
$(NRF_SDK_TARGET_DIR): downloads/$(NRF_SDK_ZIP_FILE)
	unzip -q $^
	touch $@
	mv -v $@/components/libraries/util/nrf_assert.h         $@/components/libraries/util/nrf_assert.orig.h
	mv -v $@/components/toolchain/cmsis/include/cmsis_gcc.h $@/components/toolchain/cmsis/include/cmsis_gcc.orig.h
	cp -v $(SDK_MODIFIED_FILES_DIR)/cmsis_gcc.h             $@/components/toolchain/cmsis/include/cmsis_gcc.h

nrf_sdk : $(NRF_SDK_TARGET_DIR)
	ln -sfv $^ $@

boost_$(BOOST_VERSION_UNDER): downloads/boost_$(BOOST_VERSION_UNDER).tar.gz
	tar -xf $^
	touch $@

boost: boost_$(BOOST_VERSION_UNDER)
	rm -fv $@
	ln -s $^ $@

downloads/$(NRF_SDK_ZIP_FILE):
	wget --directory-prefix=downloads https://nsscprodmedia.blob.core.windows.net/prod/software-and-other-downloads/sdks/nrf5/binaries/nrf5_sdk_$(NRF_SDK_VERSION).zip

downloads/boost_$(BOOST_VERSION_UNDER).tar.gz:
	wget --directory-prefix=downloads https://archives.boost.io/release/$(BOOST_VERSION)/source/boost_$(BOOST_VERSION_UNDER).tar.gz

scrub:
	rm -rf downloads
	rm -rf nRF5_$(NRF_SDK_VERSION)
	rm -rf boost_$(BOOST_VERSION_UNDER)

help:
	@echo "BOOST_VERSION       = $(BOOST_VERSION)"
	@echo "BOOST_VERSION_UNDER = $(BOOST_VERSION_UNDER)"
	@echo "NORDIC_SDK_VERSION  = $(NRF_SDK_VERSION)"
	@echo "NRF_SDK_ZIP_FILE    = $(NRF_SDK_ZIP_FILE)"
