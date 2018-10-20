NORDIC Notes
============

Unless otherwise specified these notes pertain to softdevice 5.0 and SDK version 14.2.
i.e. `nRF5_SDK_14.2.0_17b948a`.

Softdevice 6.0 is delivered with SDK version 15.x.

Nordic Development Boards
-------------------------
### nRF52 Development board PCA10040
+ `nRF52832 QFAAB0 1606AA`
+ Softdevice: `S132`
+ Memory

        Variant           RAM     FLASH
        nRF52832-QFAA     64 kB   512 kB    <-- PCA10040 board.
        nRF52832-QFAB     32 kB   256 kB
        nRF52832-CIAA     64 kB   512 kB

### nRF52 Development board PCA10056
+ `nRF52840`
+ Softdevice: `S140`, includes BLE 5 protocol updates.
+ Memory

        Variant           RAM     FLASH
        nRF52840-QIAA    256 kB  1024 kB <-- PCA10056 board.

### [nRF52 device comparision](http://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.nrf52%2Fdita%2Fnrf52%2Fnrf52_series.html)

At present, just dealing with `nRF52832`.

Nordic Compiler Options
-----------------------
### Common to Nordic boards

    CFLAGS += -DCONFIG_GPIO_AS_PINRESET
    CFLAGS += -DFLOAT_ABI_HARD
    CFLAGS += -DSWI_DISABLE0
    CFLAGS += -DNRF_SD_BLE_API_VERSION=5
    CFLAGS += -DSOFTDEVICE_PRESENT

### nRF52832 Compiler Options

    CFLAGS += -DBOARD_PCA10040
    CFLAGS += -DNRF52
    CFLAGS += -DNRF52832_XXAA
    CFLAGS += -DNRF52_PAN_74
    CFLAGS += -DS132

### nRF52840 Compiler Options

    CFLAGS += -DBOARD_PCA10056
    CFLAGS += -DNRF52840_XXAA
    CFLAGS += -DS140

Nordic Linker Scripts and Sections
----------------------------------

### FLASH based sections

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

#### `.sdh_soc_observers`

+ Nordic 'SoC' events are enumerated with `enum NRF_SOC_EVTS` in file
  `nRF5_SDK_14.2.0_17b948a/components/softdevice/s132/headers/nrf_soc.h`.
+ Within the class structure of this code base these events can be observered
  through the use of classes:

        | file                      |   class                  |
        |---------------------------|--------------------------|
        | nordic_soc_observable.h   | nordic::soc_observable   |
        | nordic_soc_observer.h     | nordic::soc_observer     |

#### `.sdh_ble_observers`

+ All possible BLE events can be observed on this interface.
+ The `typedef struct ble_evt_t` is used to aggregate all of these event types.
+ Within the class structure of this code base these events are demultiplexed into
  their individual GAP, GATTS and GATTC event observables and observers:

        | file                                  | class                                 |
        |---------------------------------------|---------------------------------------|
        | nordic_ble_event_observable.h         | nordic::ble_common_event_observable   |
        | nordic_ble_event_observable.h         | nordic::ble_gap_event_observable      |
        | nordic_ble_event_observable.h         | nordic::ble_gattc_event_observable    |
        | nordic_ble_event_observable.h         | nordic::ble_gatts_event_observable    |
        | nordic_ble_event_observer.h           | nordic::ble_common_event_observer     |
        | nordic_ble_event_observer.h           | nordic::ble_gap_event_observer        |
        | nordic_ble_event_observer.h           | nordic::ble_gattc_event_observer      |
        | nordic_ble_event_observer.h           | nordic::ble_gatts_event_observer      |

+ The processing point for handling Nordic BLE events is found in `../ble/nordic_ble_event_observables.cc`.
  The `struct nrf_sdh_ble_evt_observer_t` instance `sdh_ble_event_observer` is
  located in FLASH segment `.sdh_ble_observers` and is called by the Nordic softdevice.

These observers use Nordic softdevice specific event structures.
The Nordic events are mapped into generic C++ interfaces:

        | file                      | class                             |
        |---------------------------|-----------------------------------|
        | gap_event_observer.h      | ble::gap::event_observer          |
        | gattc_event_observer.h    | ble::gattc::event_observer        |
        | gatts_event_observer.h    | ble::gatts::event_observer        |

It is up to the user implemented BLE classes to join the Nordic events with the generic events.
For now, the implementation is Nordic; the expectation is that there might be TI, NXP
and SiLabs implementations from which to chose. It would be up to the user to decide when
building their top level class implementations.

Examples of this can be found in the `ble_peripheral_class` project directory.
See files: `ble_gap_connection.cc, .h`, `ble_gatts_observer.cc, .h`.
The private data members in the namespace `nordic` and their usage provide the pattern
for linking Nordic events with generic C++ interface based events.



Notes:

+ The L2CAP events are not handled. These may be very interesting and looked at in the future.
+ The Nordic BLE common event are handled within `nordic_ble_common_event_observer.cc` and
  terminate here with error log messsages. User memory allocations are not expected in these
  C++ class constructs.

#### `.sdh_req_observers`

+ Nordic requests for the softdevice state change can be observed on this interface.
+ The Noridc event `enum nrf_sdh_req_evt_t` is found in
  `nRF5_SDK_14.2.0_17b948a/components/softdevice/common/nrf_sdh.h`.
+ There are only 2 event types on this interface:

        NRF_SDH_EVT_ENABLE_REQUEST,     //!< Request to enable the SoftDevice.
        NRF_SDH_EVT_DISABLE_REQUEST,    //!< Request to disable the SoftDevice.

+ Within the class structure of this code base these events can be observered
  through the use of classes:

        | file                      |   class                  |
        |---------------------------|--------------------------|
        | nordic_req_observable.h   | nordic::req_observable   |
        | nordic_req_observer.h     | nordic::req_observer     |

+ An observer of these events can block a softdevice state change;
  effectively denying the state change request.

#### `.sdh_state_observers`

+ Nordic softdevice state changes can be observed on this interface.
+ If all of the registered `.sdh_req_observers` (above) allow the softdevice state
  change to proceed then the `.sdh_state_observers` will receive notifications about
  the state change.
+ The Noridc event `enum nrf_sdh_state_evt_t` is found in
  `nRF5_SDK_14.2.0_17b948a/components/softdevice/common/nrf_sdh.h`.
+ There are 4 event types on this interface:

        NRF_SDH_EVT_STATE_ENABLE_PREPARE,   //!< SoftDevice is going to be enabled.
        NRF_SDH_EVT_STATE_ENABLED,          //!< SoftDevice is enabled.
        NRF_SDH_EVT_STATE_DISABLE_PREPARE,  //!< SoftDevice is going to be disabled.
        NRF_SDH_EVT_STATE_DISABLED,         //!< SoftDevice is disabled.

+ Within the class structure of this code base these events can be observered
  through the use of classes:

        | file                      |   class                  |
        |---------------------------|--------------------------|
        | nordic_state_observable.h | nordic::state_observable |
        | nordic_state_observer.h   | nordic::state_observer   |


#### `.sdh_stack_observers`
This one ginormous observer for all stack events and is not used within this
code base. Each of these individual events are observered on more specifically
targeted interfaces.

#### `.pwr_mgmt_data`
This is not expected to be used. I do not like what Noric has done here.

### RAM based sections

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

Nordic S132 Softdevice
----------------------
The [S132 Softdevice Specification](http://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.nrf52%2Fdita%2Fnrf52%2Fnrf52_series.html)
describes which peripherals:

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

### Interrupt levels

In SDK `components/libraries/util/app_util_platform.h`:

There are the following definitions which give us insight
into the interrupt levels reserved for the softdevice 'SD'
and application 'APP' interrupt priority assignments.

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

### Softdevice Differences
Placeholder for examining the differences between S140 and S132 softdevices.

`sdk/examples/ble_central/ble_app_multilink_central`

     diff pca10040/s132/armgcc/ble_app_multilink_central_gcc_nrf52.ld
          pca10056/s140/armgcc/ble_app_multilink_central_gcc_nrf52.ld

    <   FLASH (rx) : ORIGIN =    0x23000, LENGTH = 0x5d000
    <   RAM  (rwx) : ORIGIN = 0x20004190, LENGTH =  0xbe70
    ---
    >   FLASH (rx) : ORIGIN =    0x22000, LENGTH = 0xde000
    >   RAM  (rwx) : ORIGIN = 0x20004170, LENGTH = 0x3be90

Nordic Interrupt Clearing
-------------------------

The nRF52832 Product Specification v1.4
Section 15.8.1 Peripheral Interface, Interrupt clearing

Explains why a volatile dummy read is required after clearing an interrupt.

Noridc SDK Version
------------------
+ Nordic SDK Version = `nRF5_SDK_14.2.0_17b948a`

    Note: In my opinion, SDK-15 was a step in the wrong direction.
    More interdependencies with even more horrific macros.

+ Set as symlink at top level dir:

        $ ln -s <path to Nordic SDK> sdk

Nordic SDK modifications
------------------------
+ A bash script in `nrf/nordic/sdk-modified/replace_sdk_files` can be used to
  replace SDK files which require changes.
+ The script must be run from the directory `nrf/nordic/sdk-modified`.
+ The link `nrf/sdk` must point to the Nordic SDK 14.2 root:
  `sdk@ -> ../../nRF/SDK/nRF5_SDK_14.2.0_17b948a`

The following files have been modified from the original:

+ `sdk/components/libraries/util/nrf_assert.h`
    + Rename it to `sdk/components/libraries/util/nrf_assert.orig.h`.
    + This will remove it from compilation.
    + The file `nrf/nordic/nrf_assert.h` will be used instead.
+ `sdk/components/ble/ble_db_discovery/ble_db_discovery.h`
    + Rename it to `sdk/components/ble/ble_db_discovery/ble_db_discovery.orig.h`
    + Replace it with `nrf/nordic/sdk-modified/ble_db_discovery.h`
    + The `ble_db_discovery_t _name[_cnt] = {{ ... }}` field initializations
      are incomplete, causing C++ compiler errors.
+ `sdk/components/libraries/util/app_error_weak.c`
    + Rename it to `sdk/components/libraries/util/app_error_weak.orig.c`
    + Replace it with `nrf/nordic/sdk-modified/app_error_weak.c`
    + The line: `#include "app_util_platform.h"` is added.
      Nordic did not properly include this header dependency.
+ `sdk/components/toolchain/cmsis/include/cmsis_gcc.h`
    + Rename it to `sdk/components/toolchain/cmsis/include/cmsis_gcc.orig.h`
    + Replace it with `nrf/nordic/sdk-modified/cmsis_gcc.h`
    + Added C++ guards for C language declarations within this file.
    + Removed `register` keyword instances.
      These are deprecated and cause compiler warnings.

