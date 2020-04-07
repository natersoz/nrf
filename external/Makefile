###
# nrf/external/Makefile
# Obtain and configure external dependencies.
# Copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
###

GTEST_VERSION = 1.10.0

.PHONY: all mrproper

all: downloads nRF5_SDK_16.0.0_98a08e2 googletest

downloads:
	mkdir $@

nRF5_SDK_16.0.0_98a08e2: downloads/nRF5SDK160098a08e2.zip
	mkdir -p $@
	unzip $^ -d $@
	cd ../nordic/sdk-modified && ./replace_sdk_files
	touch $@

googletest-release-$(GTEST_VERSION): downloads/release-$(GTEST_VERSION).zip
	unzip $^
	touch $@

googletest: googletest-release-$(GTEST_VERSION)
	ln -s googletest-release-$(GTEST_VERSION) $@

downloads/release-$(GTEST_VERSION).zip:
	wget --directory-prefix=downloads https://github.com/google/googletest/archive/release-$(GTEST_VERSION).zip

# Nordic nRF5 SDK 16.0
downloads/nRF5SDK160098a08e2.zip:
	wget --directory-prefix=downloads https://www.nordicsemi.com/-/media/Software-and-other-downloads/SDKs/nRF5/Binaries/nRF5SDK160098a08e2.zip

mrproper:
	rm -rf downloads
	rm -rf nRF5_SDK_16.0.0_98a08e2
	rm -rf googletest*
