###
# nrf/external/Makefile
# Obtain and configure external dependencies.
# Copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
###

GTEST_VERSION   = 1.10.0
BOOST_VERSION   = 1.76.0
NRF_SDK_VERSION = 17.0.2_d674dde

# Change boost version X.Y.Z => X_Y_Z
BOOST_VERSION_UNDER = $(shell echo $(BOOST_VERSION) | tr '.' '_')

.PHONY: all mrproper

all: downloads nrf_sdk boost googletest

SDK_MODIFIED_FILES_DIR = ../nordic/sdk-modified/

downloads:
	mkdir $@

nRF5_SDK_$(NRF_SDK_VERSION): downloads/nRF5SDK1702d674dde.zip
	unzip $^
	touch $@
	mv $@/components/libraries/util/nrf_assert.h $@/components/libraries/util/nrf_assert.orig.h

nrf_sdk : nRF5_SDK_$(NRF_SDK_VERSION)
	ln -s $^ $@

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

downloads/nRF5SDK1702d674dde.zip:
	wget --directory-prefix=downloads https://www.nordicsemi.com/-/media/Software-and-other-downloads/SDKs/nRF5/Binaries/nRF5SDK1702d674dde.zip

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

