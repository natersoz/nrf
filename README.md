project: natersoz/nrf
====================
Copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.

LICENSING
---------
+ File which contain a Nordic Semiconductor License in the comments header
  are covered under the terms found in `NORDIC_LICENSE.txt`.
+ Files in the segger/ directory are covered under the terms of SEGGER's
  licencing found in the source files and in the license file
  segger_rtt/license/license.txt.
+ All other files are covered under the Apache 2.0 LICENSE file.

Documentation
-------------
This README.md will be limited.
More detailed descriptions are in the directory [docs](https://github.com/natersoz/nrf/tree/master/docs).

Goals:
------
+ Provide a generic BLE implementation using C++ class structures which
  can be adapted to any vendor specific hardware. The first instance will
  be the Nordic nrf52 development board.
  + The implementation will not use an OS, but should be easily adapted to an OS.
  + It is intended that nearly all processing occur within an ISR.
    Given the ARM Nested Vector Interrupt Controller (NVIC) this should be
	easy to do and yeild the best performance.
+ Peripheral device drivers should access the CMSIS mapped registers and
  rely minimally on the SDK. Progress will be indicated by the lack
  of file and module coupling between device drivers.

Progress:
---------
+ Peripherals on `master` in the directory `peripherals` have been tested.
  Faults have been logged as issues.
+ Peripheral tests can be found in the directory nordic/tests. Each test is
  given its own directory and Makefile: `spi_test`, `rtc_test`, etc.
+ Code which can be tested without hardware dependencies is found under `unit_tests`.
+ The project `ble_peripheral_class` is a BLE peripheral example using the
  BLE class based implementation of this code base.
  It is a GATT server only peripheral (at this time).
  It is working as expected on Nordic hardware.

