Building the projects
=====================

ARM GCC Compiler
----------------
+ Stating the obvious: Download the [ARM GCC toolchain](https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads)
+ Adjust the nrf/make_fules/arm_gcc_rules.mak file by setting the `GNU_GCC_ROOT` therein.

		I'm using gcc-arm-none-eabi-7-2018-q2-update installed:
		/opt/gcc-arm-none-eabi-7-2018-q2-update
		A symlink /opt/gcc-arm-none-eabi -> gcc-arm-none-eabi-7-2018-q2-update

		Using this installation requires no modification to the make rules.

+ An alias for `gdb` is helpful. Something like:

  `alias gdb-arm='/opt/gcc-arm-none-eabi/bin/arm-none-eabi-gdb --quiet'`

Boost Libraries
---------------
+ If you do not like C++, well, you better look elsewhere.
+ Boost libraries will used as they make sense.
    + The boost::intrusive is certainly going to be used.
      This is an excellent library for embedded work.
    + Boost 1.66.0 is being used at this time.

			I'm using boost 1.66.0 installed in /opt/boost_1_66_0
			with symlink /opt/boost -> boost_1_66_0

Nordic SDK
----------
+ Install the [Nordic SDK](http://infocenter.nordicsemi.com/topic/com.nordic.infocenter.sdk/dita/sdk/nrf5_sdk.html)
  version 14.2 `nRF5_SDK_14.2.0_17b948a`
+ The symlinks are set in git as:

		sdk@      -> sdk-14
		sdk-14@   -> ../../nRF/SDK/nRF5_SDK_14.2.0_17b948a
		sdk-15@   -> ../../nRF/SDK/nRF5_SDK_15.2.0_9412b96
		sdk-15.0@ -> ../../nRF/SDK/nRF5_SDK_15.0.0_a53641a
		sdk-15.2@ -> ../../nRF/SDK/nRF5_SDK_15.2.0_9412b96

+ Some SDK files need replaced or moved out of the way.
  For more details see doc/NORDIC.md section "Nordic SDK modifications".
  To quickly get going:

		$ cd nrf/nordic/sdk-modified
		$ ./replace_sdk_files

Building
--------
From the top directory `nrf`:

	$ make

It should compile and link all projects without errors or warnings.
