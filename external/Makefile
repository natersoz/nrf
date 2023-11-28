###
# nrf/external/Makefile
# Obtain and configure external dependencies.
# Copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
###

# Uncomment for old SDK versions. Leave commented out for 17.1.0 and later.
# USE_NRF_OLD_ZIP_NAME := 1
# NRF_SDK_VERSION      := 17.0.2_d674dde

GTEST_VERSION   := 1.10.0
BOOST_VERSION   := 1.76.0
NRF_SDK_VERSION := 17.1.0_ddde560

# Change boost version X.Y.Z => X_Y_Z
BOOST_VERSION_UNDER := $(shell echo $(BOOST_VERSION) | tr '.' '_')

.PHONY: all mrproper

NRF_SDK          := nRF5_SDK_$(NRF_SDK_VERSION)
NRF_SDK_ZIP_NAME := nRF5_SDK_$(NRF_SDK_VERSION)

ifeq ($(USE_NRF_OLD_ZIP_NAME), 1)
NRF_SDK_ZIP_NAME := $(subst _,,$(NRF_SDK_ZIP_NAME))
NRF_SDK_ZIP_NAME := $(subst .,,$(NRF_SDK_ZIP_NAME))
endif

NRF_SDK_ZIP_FILE     := $(NRF_SDK_ZIP_NAME).zip
NRF_SDK_DOWNLOAD_URL := https://www.nordicsemi.com/-/media/Software-and-other-downloads/SDKs/nRF5/Binaries/$(NRF_SDK_ZIP_FILE)

all: downloads nrf_sdk boost googletest

SDK_MODIFIED_FILES_DIR = ../nordic/sdk-modified/

downloads:
	mkdir $@

# Download the Nordic SDK, unzip it, and move some hated files out of the way.
$(NRF_SDK): downloads/$(NRF_SDK_ZIP_FILE)
	unzip $^
	touch $@
	mv -v $@/components/libraries/util/nrf_assert.h         $@/components/libraries/util/nrf_assert.orig.h
	mv -v $@/components/toolchain/cmsis/include/cmsis_gcc.h $@/components/toolchain/cmsis/include/cmsis_gcc.orig.h
	cp ../nordic/sdk-modified/cmsis_gcc.h                   $@/components/toolchain/cmsis/include/cmsis_gcc.h

nrf_sdk : $(NRF_SDK)
	ln -sfv $^ $@

boost_$(BOOST_VERSION_UNDER): downloads/boost_$(BOOST_VERSION_UNDER).zip
	unzip $^
	touch $@

boost: boost_$(BOOST_VERSION_UNDER)
	rm -fv $@
	ln -s $^ $@

googletest-release-$(GTEST_VERSION): downloads/release-$(GTEST_VERSION).zip
	unzip $^
	touch $@

googletest: googletest-release-$(GTEST_VERSION)
	ln -s $^ $@

downloads/release-$(GTEST_VERSION).zip:
	wget --directory-prefix=downloads https://github.com/google/googletest/archive/release-$(GTEST_VERSION).zip

downloads/$(NRF_SDK_ZIP_FILE):
	wget --directory-prefix=downloads https://www.nordicsemi.com/-/media/Software-and-other-downloads/SDKs/nRF5/Binaries/$(NRF_SDK_ZIP_FILE)

downloads/boost_$(BOOST_VERSION_UNDER).zip:
	wget --directory-prefix=downloads https://boostorg.jfrog.io/artifactory/main/release/$(BOOST_VERSION)/source/boost_$(BOOST_VERSION_UNDER).zip

mrproper:
	rm -rf downloads
	rm -rf nRF5_$(NRF_SDK_VERSION)
	rm -rf boost_$(BOOST_VERSION_UNDER)
	rm -rf googletest*

help:
	@echo "BOOST_VERSION       = $(BOOST_VERSION)"
	@echo "BOOST_VERSION_UNDER = $(BOOST_VERSION_UNDER)"
	@echo "GTEST_VERSION       = $(GTEST_VERSION)"
	@echo "NORDIC_SDK_VERSION  = $(NRF_SDK_VERSION)"
	@echo "NRF_SDK_ZIP_FILE    = $(NRF_SDK_ZIP_FILE)"
