###
# nrf/external/Makefile
# Obtain and configure external dependencies.
# Copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
###

.PHONY: all mrproper

all: downloads nRF5_SDK_15.2.0_9412b96 googletest

downloads:
	mkdir $@

nRF5_SDK_15.2.0_9412b96: downloads/nRF5SDK15209412b96.zip
	unzip $^
	cd ../nordic/sdk-modified && ./replace_sdk_files
	touch $@

googletest-release-1.8.1: downloads/release-1.8.1.zip
	unzip $^
	touch $@

googletest: googletest-release-1.8.1
	ln -s googletest-release-1.8.1 $@

downloads/release-1.8.1.zip:
	wget --directory-prefix=downloads https://github.com/google/googletest/archive/release-1.8.1.zip

downloads/nRF5SDK15209412b96.zip:
	wget --directory-prefix=downloads  https://www.nordicsemi.com/-/media/Software-and-other-downloads/SDKs/nRF5/Binaries/nRF5SDK15209412b96.zip

mrproper:
	rm -rf downloads
	rm -rf nRF5_SDK_15.2.0_9412b96
	rm -rf googletest*