project: natersoz/nrf
====================
Copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.

This project is a starting point for working with the Nordic NRF52
PCA10040 development boards.

This README.md is getting large. At some point it will be broken up into
smaller pieces.

Goals:
------

+ Simplify the usage of the Nordic SDK.
  There are too many changes Nordic is making between releases,
  much of which I have no use for (`FreeRTOS`, `app_scheduler`, `sdk_config.h`).
  Nordic is trying to be all things to all people and do their work for
  them. I object. Let this project illustrate my expectations.
+ Peripheral device drivers should access the CMSIS mapped registers and
  rely minimally on the SDK. Progress will be indicated by the lack
  of file and module coupling between device drivers.
+ Isolate and simplify the softdevice usage. The softdevice is very well
  architected (from what I can tell) and does not incur many of the wild
  gyrations of the SDK. The softdevice will be called directly.
+ There will be some portions of the SDK that will be used:
    + As an example: I am expecting the fs modules to be used; at least the
      interface provided in `nrf_fstorage.h`. This is an example of how the SDK
      should look across the board. An interface decoupled from implementation
      which the softdevice will use. The OEM can provide their own
      impleemntation or use what the SDK provides. This is an example of
      work well done.

LICENSING
---------
+ File which contain a Nordic Semiconductor License in the comments header
  are covered under the terms found in `NORDIC_LICENSE.txt`.
+ All other files are covered under the Apache 2.0 LICENSE file.

Noridc SDK Version:
-------------------
+ Nordic SDK Version = `nRF5_SDK_14.2.0_17b948a`

    Note: In my opinion, SDK-15 was a step in the wrong direction.
    More interdependencies with even more horrific macros.

+ Set as symlink at top level dir:

        $ ln -s <path to Nordic SDK> sdk

+ SDK modifications:

    + remove `components/libraries/util/nrf_assert.orig.h`
      Replaced with `nordic/nrf_assert.h`
    + `components/libraries/util/app_error_weak.c`
      Add line:
      `#include "app_util_platform.h"`

Nordic SDK Changes:
-------------------
All nordic changed files can be found in the directory: `nrf/nordic`

+ mv `../sdk/components/libraries/util/nrf_assert.h`:
    renamed: `../sdk/components/libraries/util/nrf_assert.orig.h`
    To avoid inclusion, replaced with `../nrf/nrf_assert.h`

Nordic Linker Scripts and Sections
----------------------------------
The softdevice requires specific sections to be defined in the linker script
See `ble_peripheral_template.ld` As the usage is discovered, exmplanations
are provided here:

### FLASH based sections:

Note that all FLASH based sections contain the variables containing
the prefix `__start_` and `__stop_` followed by the section name.
The prefixes are used to iterate through the observers the user,
via the SDK, registers with the softdevice. This is the means by which
the softdevice calls back to the application: through these observers.

Each observer has 2 parts:
    + A handler, which is the callback function the softdevice will call.
    + A context, which is a `void*`, which is cast back within the `SDK/app`
      callback and provides the data 'context' in which the observer
      operates.

See also the macros defined in nrf_section.h:
    + `NRF_SECTION_ITEM_GET()` using the `__start_` and `section_name`,
      extract the data pointer from the section name.
    + `NRF_SECTION_ITEM_COUNT()` The number of data nodes within the section.

#### `.sdh_soc_observers`:
#### `.sdh_ble_observers`:
#### `.sdh_req_observers`:
#### `.sdh_state_observers`:
These are how the softdevice communicates event notifications with the client
code using the observer pattern. These observers are implemented in classes
within the code base and the explanation is found within the headers and srouce.

#### `.sdh_stack_observers`:
This one ginormous observer for all stack events and is not used within this
code base. Separation of functionality is a good thing.

### `.pwr_mgmt_data`:
This is not expected to be used. I do not like what Noric has done here.

### RAM based sections:


#### `.log_dynamic_data`

This appears to be a dynamically allocated heap pool location (?) for
the Nordic log. I'm not using it and it should be removed.

#### `.fs_data`

+ Used in: `./components/libraries/fstorage/nrf_fstorage.h, .c`
+ Stored in RAM
+ Used to store `nrf_fstorage_t`
+ This appears to be a desdriptor related to the FLASH controller (?)
  and/or how flash is being used within the SDK.
+ This does not appear to interact with the soft device directly.
  i.e. limited to SDK functionality.

#### `.cli_sorted_cmd_ptrs`

+ Used in `./components/libraries/cli/nrf_cli.c, .h`
+ This appears to be a command interface buffer allocation.
  Perhaps used as a console command type of thing.
+ Not used, should be removed.

### Device Differences

 `sdk/examples/ble_central/ble_app_multilink_central`

```
         diff pca10040/s132/armgcc/ble_app_multilink_central_gcc_nrf52.ld
              pca10056/s140/armgcc/ble_app_multilink_central_gcc_nrf52.ld

        <   FLASH (rx) : ORIGIN =    0x23000, LENGTH = 0x5d000
        <   RAM  (rwx) : ORIGIN = 0x20004190, LENGTH =  0xbe70
        ---
        >   FLASH (rx) : ORIGIN =    0x22000, LENGTH = 0xde000
        >   RAM  (rwx) : ORIGIN = 0x20004170, LENGTH = 0x3be90
```

### Softdevice 5.0, 6.0 header file differences:

Summary: the `ble_gap.h` has changed significantly.
If not for these changes this the migration from softdevice 5 -> 6
would merely be a re-compile.

Only changes which have a meaningful difference are noted here -
i.e. API changes.

+ `ble_err.h`:
  BLE Error codes change:
    + 5.0.0
        `BLE_ERROR_INVALID_ROLE = NRF_ERROR_STK_BASE_NUM + 4`
    + 6.0.0
        ```
        BLE_ERROR_INVALID_ADV_HANDLE = NRF_ERROR_STK_BASE_NUM + 4
        BLE_ERROR_INVALID_ROLE       = NRF_ERROR_STK_BASE_NUM + 5
        BLE_ERROR_INVALID_ROLE       = BLE_ERROR_BLOCKED_BY_OTHER_LINKS + 6
        ```

+ `ble.h`:
    Comments mostly, change in `LE_EVT_LEN_MAX(ATT_MTU)` macro that
    should be transparent if code is recompiled.

+ `ble_ranges.h`
    + `BLE_XXX_SVC_LAST` changes - recompile to pick up changes.

+ `ble_gatts.h`
    Comments only:
    Added `NRF_ERROR_TIMEOUT` comments to several functions,
    which means that a new error type was added and handled.

+ `nrf_nvic.h`
    Inside `__NRF_NVIC_SD_IRQS_0` definition `SWI5_EGU5_IRQn`
    changes name to `SWI5_IRQn`.

+ `nrf_soc.h`
  + `SOC_SVC_BASE_NOT_AVAILABLE` changes value
  + `NRF_SOC_SVCS`
    Add `SD_PROTECTED_REGISTER_WRITE`
    Add function `sd_protected_register_write()`

+ `nrf_sdm.h`
    `SD_MAJOR_VERSION= 6, was 5`

+ `ble_gap.h` -- perhaps too many to enumerate
    + `BLE_GAP_SVCS`:
        Adds
        + `SD_BLE_GAP_QOS_CHANNEL_SURVEY_START`
        + `SD_BLE_GAP_QOS_CHANNEL_SURVEY_STOP`
        + Changes:
    + `SD_BLE_GAP_ADV_DATA_SET -> SD_BLE_GAP_ADV_SET_CONFIGURE`
        This could be handled by:
        `#define SD_BLE_GAP_ADV_DATA_SET SD_BLE_GAP_ADV_SET_CONFIGURE`
        when porting softdevice 5 -> 6.
    + `BLE_GAP_OPTS`
        Removes: `BLE_GAP_OPT_SCAN_REQ_REPORT`
    + `BLE_GAP_TX_POWER_ROLES` enum added
    + `BLE_GAP_TIMEOUT_SRC_ADVERTISING` removed
    + `BLE_GAP_ADDR_TYPE_ANONYMOUS` added
    + Added symbols:
        + `BLE_GAP_POWER_LEVEL_INVALID`
        + `BLE_GAP_ADV_SET_HANDLE_NOT_SET`
        + `BLE_GAP_ADV_SET_COUNT_DEFAULT`
        + `BLE_GAP_ADV_SET_COUNT_MAX`
        + `BLE_GAP_ADV_SET_DATA_SIZE_MAX`
        + `BLE_GAP_ADV_REPORT_SET_ID_NOT_AVAILABLE`
        + `BLE_GAP_EVT_ADV_SET_TERMINATED_REASON_TIMEOUT`
        + `BLE_GAP_EVT_ADV_SET_TERMINATED_REASON_LIMIT_REACHED`
    + Changed values for longer scans:
        + `BLE_GAP_ADV_INTERVAL_MAX`
        + `BLE_GAP_SCAN_INTERVAL_MAX`
        + `BLE_GAP_SCAN_WINDOW_MAX`

+ `ble_gattc.h`
    + Added comments related to new result type `NRF_ERROR_TIMEOUT`.


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
```
        CFLAGS += -DCONFIG_GPIO_AS_PINRESET
        CFLAGS += -DFLOAT_ABI_HARD
        CFLAGS += -DSWI_DISABLE0
        CFLAGS += -DNRF_SD_BLE_API_VERSION=5
        CFLAGS += -DSOFTDEVICE_PRESENT
```

### nRF52832 Compiler Options:
```
        CFLAGS += -DBOARD_PCA10040
        CFLAGS += -DNRF52
        CFLAGS += -DNRF52832_XXAA
        CFLAGS += -DNRF52_PAN_74
        CFLAGS += -DS132
```

### nRF52840 Compiler Options:
```
        CFLAGS += -DBOARD_PCA10056
        CFLAGS += -DNRF52840_XXAA
        CFLAGS += -DS140
```

ARM GCC Compiler:
-----------------
+ Stating the obvious: Download the [ARM GCC toolchain](https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads)
+ Adjust the Makefile inclusion file arm_gcc_rules.mak by setting the
  `GNU_GCC_ROOT` therein.

+ An alias for `gdb` might be helpful. Something like:

  `alias gdb-arm='/opt/gcc-arm-none-eabi/bin/arm-none-eabi-gdb --quiet'`

Boost Libraries:
----------------
+ If you do not like C++, well, you better look elsewhere.
+ Boost libraries will used as they make sense.
    + The boost::intrusive is certainly going to be used.
      This is an excellent library for embedded work.
    + Boost 1.66.0 is being used at this time.

Nordic S132 Softdevice
----------------------
The S132 Softdevice Specification describes which peripherals:

    + Must be accessed through the SoftDevice API (restricted).
    + Must not be used when the SoftDevice is active (blocked).

    CLOCK                               Restricted
    POWER                               Restricted
    BPROT                               Restricted
    RADIO                               Blocked
    TIMER0                              Blocked
    TEMP                                Restricted
    RNG                                 Restricted
    ECB                                 Restricted
    CCM                                 Restricted
    AAR                                 Blocked
    EGU1/SWI1/Radio Notification        Restricted
    EGU2/SWI2/SoftDevice Event          Blocked
    EGU4/SWI4                           Blocked
    EGU5/SWI5                           Blocked
    FICR                                Blocked
    UICR                                Restricted
    NVIC                                Restricted

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
A script `nrf/scripts/jlink-nrf` provides a short cut for the command line invocation.
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


