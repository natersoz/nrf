natersoz/nrf project
====================
Copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.


This project is a starting point for working with the Nordic NRF52
PCA10040 development boards.

LICENSING
---------
+ File which contain a Nordic Semiconductor License in the comments header
  are covered under the terms found in NORDIC_LICENSE.txt.
+ All other files are covered under the Apache 2.0 LICENSE file.

Noridc SDK Version:
-------------------
+ Nordic SDK Version = `nRF5_SDK_15.0.0_a53641a`
+ Set as symlink at top level dir:

		$ ln -s <path to Nordic SDK> sdk

Nordic SDK Changes:
-------------------
+ ./components/libraries/util/app_error_weak.c:
	Add:
	#include "app_util_platform.h"

+ mv ../sdk/components/libraries/util/nrf_assert.h:
	renamed: ../sdk/components/libraries/util/nrf_assert-orig.h
	To avoid inclusion, replaced with ../nrf/nrf_assert.h

+ nrf_atfifo.h
	#include "sdk_errors.h"

Nordic Development Boards:
--------------------------
### nRF52 Development board PCA10040
+ `nRF52832 QFAAB0 1606AA`
+ Softdevice: `S132`
+ Memory

		Variant           RAM     FLASH
		nRF52832-QFAA     64 kB   512 kB <-- PCA10040 board.
		nRF52832-QFAB     32 kB   256 kB
		nRF52832-CIAA     64 kB   512 kB

### nRF52 Development board PCA10056
+ `nRF52840`
+ Softdevice: `S140`, includes BLE 5 protocol updates.
+ Memory

		Variant           RAM     FLASH
		nRF52840-QIAA    256 kB  1024 kB <-- PCA10056 board.

### [nRF52 device comparision](http://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.nrf52%2Fdita%2Fnrf52%2Fnrf52_series.html)

	At present, just dealing with nRF52832.

Nordic Compiler Options:
------------------------
### Common to Nordic boards:
		CFLAGS += -DCONFIG_GPIO_AS_PINRESET
		CFLAGS += -DFLOAT_ABI_HARD
		CFLAGS += -DSWI_DISABLE0
		CFLAGS += -DNRF_SD_BLE_API_VERSION=5
		CFLAGS += -DSOFTDEVICE_PRESENT

### nRF52832 Compiler Options:
		CFLAGS += -DBOARD_PCA10040
		CFLAGS += -DNRF52
		CFLAGS += -DNRF52832_XXAA
		CFLAGS += -DNRF52_PAN_74
		CFLAGS += -DS132

### nRF52840 Compiler Options:
		CFLAGS += -DBOARD_PCA10056
		CFLAGS += -DNRF52840_XXAA
		CFLAGS += -DS140

Nordic Linker File Breakdown:
-----------------------------
+ Under construction

+ `sdk/examples/ble_central/ble_app_multilink_central`

		 diff pca10040/s132/armgcc/ble_app_multilink_central_gcc_nrf52.ld
			  pca10056/s140/armgcc/ble_app_multilink_central_gcc_nrf52.ld

		<   FLASH (rx) : ORIGIN =    0x23000, LENGTH = 0x5d000
		<   RAM  (rwx) : ORIGIN = 0x20004190, LENGTH =  0xbe70
		---
		>   FLASH (rx) : ORIGIN =    0x22000, LENGTH = 0xde000
		>   RAM  (rwx) : ORIGIN = 0x20004170, LENGTH = 0x3be90

ARM GCC Compiler:
-----------------
+ Stating the obvious: Download the [ARM GCC toolchain](https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads)
+ Adjust the Makefile inclusion file arm_gcc_rules.mak by setting the
  `GNU_GCC_ROOT` therein.

+ An alias for `gdb` might be helpful. Something like:

  `alias gdb-arm='/usr/local/gcc-arm-none-eabi/bin/arm-none-eabi-gdb --quiet'`

Nordic S132 Softdevice
----------------------
The S132 Softdevice Specification describes which peripherals:

	+ Must be accessed through the SoftDevice API (restricted).
    + Must not be used when the SoftDevice is active (blocked).

	CLOCK								Restricted
	POWER								Restricted
	BPROT								Restricted
	RADIO								Blocked
	TIMER0								Blocked
	TEMP								Restricted
	RNG									Restricted
	ECB									Restricted
	CCM									Restricted
	AAR									Blocked
	EGU1/SWI1/Radio Notification		Restricted
	EGU2/SWI2/SoftDevice Event			Blocked
	EGU4/SWI4							Blocked
	EGU5/SWI5							Blocked
	FICR								Blocked
	UICR								Restricted
	NVIC								Restricted

### Interrupt levels:

	In SDK components/libraries/util/app_util_platform.h:

	There are the following definitions which give us insight
	into the interrupt levels reserved for the softdevice 'SD'
	and application 'APP' interrupt priority assignments.

		```
			#if __CORTEX_M == (0x00U)
				#define _PRIO_SD_HIGH       0
				#define _PRIO_APP_HIGH      1
				#define _PRIO_APP_MID       1
				#define _PRIO_SD_LOW        2
				#define _PRIO_APP_LOW       3
				#define _PRIO_APP_LOWEST    3
				#define _PRIO_THREAD        4
			#elif __CORTEX_M == (0x04U)
				#define _PRIO_SD_HIGH       0
				#define _PRIO_SD_MID        1
				#define _PRIO_APP_HIGH      2
				#define _PRIO_APP_MID       3
				#define _PRIO_SD_LOW        4
				#define _PRIO_SD_LOWEST     5
				#define _PRIO_APP_LOW       6
				#define _PRIO_APP_LOWEST    7
				#define _PRIO_THREAD        15
			#else
				#error "No platform defined"
			#endif
		```


Debugging and Flashing nRF5x:
-----------------------------
### Jlink Commander (on OSX JLinkExe)

  See script nrf/scripts/jlink-nrf

+ OSX:      `/Applications/SEGGER/JLink/JLinkExe`
+ Linux:    `/opt/JLink` (or where ever ...)

### JLinkClient
To get the Jlink RTT Client running, JLink Commander (JLinkExe) needs to be running first.
A script nrf/scripts/jlink-nrf provides a short cut for the command line invocation.
After running JLinkExe the JLinkClient can be started and will find the telnet
port to connect. Starting JLinkClient first does not seem to work.

If JLinkExe is terminated then JLinkClient will need to restart.

### JLinkRTTLogger
JLinkRTTLogger works in a similar manner as JLinkClient, however the default channel
of '1' is incorrect as configured in these projects. These projects use channel 0.
JLinkRTTLogger seems to reconnect properly with the starting and stoppping of
JLinkExe; unlike JLinkClient.

### jlink_rules.mak
+ Provides several rules for programming the nRF52 development boards.
	+ Only the `nRF52832/PCA10040` is supported right now.
    + It should not be hard to support the `nRF52840/PCA10056`.
+ `make jlink-help` provides rule specific command information.
+ To program the application onto the nRF52 board: `make flash`

	Note: This rule has a dependency on `TARGET_NAME.hex`
	(where `TARGET_NAME` is project name being built.)
	So building this target will update the compile object files,
	elf `.out` and `.hex` files, but not the `.lst` and `.dump.txt` files.
	The makefile rules could use a little tweaking here.
	Use the rule `make all` to insure all of the final targets are up to date.

Note: Attempting to flash the nRF boards while the jlink RTT client is attached
causes the NRF boards to hang. If this happens, drop the RTT client and disconnect
the board USB. Reconnect the USB and start over.

### gdb
+ `jlink_rules.mak`

  Provides a rules for programming and running the gdb server on the NRF board.
  Again, right now the `nRF52832/PCA10040` is set with symbols in this file as
  is the softdevice `s132`.

  The `jlink-rules.mak` contains gdb specific settings in the `JLINK_GDB_OPTS` symbol.
  Change these as needed.

+ gdb initialization
  The gdb server setting must be matched with the gdb client settings.
  Using a gdb initialization file makes connecting and setting up gdb faster
  and easier. Example:

		target remote localhost:2331
		file _build/<targetname.out>
		load
		monitor reset

  Obviously the port must match the port defined in jlink-rules,mak.

		gdb-arm --quiet --command=./gdb-init


