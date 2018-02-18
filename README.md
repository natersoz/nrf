natersoz/nrf project
====================

This project is a starting point for working with the Nordic NRF52
PCA10040 development boards.

LICENSING
---------
+ File which contain a Nordic Semiconductor License in the comments header
  are covered under the terms found in NORDIC_LICENSE.txt.
+ All other files are covered under the Apache 2.0 LICENSE file.

Noridc SDK Version:
-------------------
+ Nordic SDK Version = `nRF5_SDK_14.2.0_17b948a`
+ Set as symlink at top level dir.

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

### nRf52 Development board PCA10056
+ `nRF52840`
+ Softdevice: `S140`, includes BLE 5 protocol updates.
+ Memory

		Variant           RAM     FLASH
		nRF52840-QIAA    256 kB  1024 kB <-- PCA10056 board.

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

