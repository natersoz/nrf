Softdevice 6.0 notes:
=====================

+ Notes on the SDK 15 vs. 14.2 differences.
+ Focussed on the Softdevice header files 5.0 -> 6.0.

Softdevice 5.0, 6.0 header file differences:
--------------------------------------------

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



