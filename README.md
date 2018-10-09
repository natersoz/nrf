project: natersoz/nrf
====================
Copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.

LICENSING
---------
+ File which contain a Nordic Semiconductor License in the comments header
  are covered under the terms found in `NORDIC_LICENSE.txt`.
+ All other files are covered under the Apache 2.0 LICENSE file.

Documentation
-------------
This README.md will be limited.
More detailed descriptions are in the directory `doc`.

Goals:
------
+ Provide a generic BLE implementation using C++ class structures which
  can be adapted to any vendor specific hardware. The first instance will
  be the Nordic nrf52 development board.
  + The implementation will not use an OS, but should be easily adapted to an OS.
+ Peripheral device drivers should access the CMSIS mapped registers and
  rely minimally on the SDK. Progress will be indicated by the lack
  of file and module coupling between device drivers.
+ Isolate and simplify the softdevice usage. The softdevice is very well
  architected (from what I can tell) and does not incur many of the wild
  gyrations of the SDK. The softdevice will be called directly.
+ What about mbed BLE? This is a good question.
  Show me the declaration for the securityManager class and we can proceed.
  + This appears to be a well-meaning work in progress.
    It will be nice if it ever happens.

Progress:
---------
+ Peripherals on `master` in the directory `peripherals` have been tested.
+ Peripheral tests can be found in the directories `spi_test`, `rtc_test`, etc.
+ Code which can be tested without hardware dependencies is found under `unit_tests`.
+ The project `ble_peripheral_class` is a BLE peripheral example using the BLE class
  based implementation of this code base. It is a GATT server only peripheral.
  It is tested and working as expected on Nordic hardware.

