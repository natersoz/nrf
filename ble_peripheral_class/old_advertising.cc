/**
 * @file ble_peripheral_class/advertising.cc
 */

#include "nrf_sdh_ble.h"
#include "nrf_sdh_soc.h"
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_srv_common.h"
#include "peer_manager.h"
#include "sdk_errors.h"

#include "logger.h"
#include "project_assert.h"
#include "section_macros.h"

#include "advertising.h"

static_assert(NRF_SDH_BLE_ENABLED);
static_assert(BLE_ADV_BLE_OBSERVER_PRIO < NRF_SDH_BLE_OBSERVER_PRIO_LEVELS);

static_assert(NRF_SDH_SOC_ENABLED);
static_assert(BLE_ADV_BLE_OBSERVER_PRIO < NRF_SDH_SOC_OBSERVER_PRIO_LEVELS);

static ble_advertising_t m_advertising;

static nrf_sdh_ble_evt_observer_t sdh_ble_observer_advertising
    IN_SECTION(".sdh_ble_observers") =
{
    .handler    = ble_advertising_on_ble_evt,
    .p_context  = &m_advertising
};

static nrf_sdh_soc_evt_observer_t sdh_soc_observer_advertising
    IN_SECTION(".sdh_soc_observers") =
{
    .handler    = ble_advertising_on_sys_evt,
    .p_context  = &m_advertising
};

// YOUR_JOB: Use UUIDs for service(s) used in your application.
static ble_uuid_t m_adv_uuids[] =
{
    {BLE_UUID_DEVICE_INFORMATION_SERVICE, BLE_UUID_TYPE_BLE}
};

static void advertising_event_handler(ble_adv_evt_t ble_adv_evt)
{
    ret_code_t error_code;
    logger &logger = logger::instance();

    switch (ble_adv_evt)
    {
    case BLE_ADV_EVT_FAST:
        logger.info("Fast advertising.");
        break;

    case BLE_ADV_EVT_IDLE:
        logger.debug("calling: sd_power_system_off()");
        logger.flush();

        /*
         * Note: when a debugger is attached sd_power_system_off() will return
         * with 0x2006 NRF_ERROR_SOC_POWER_OFF_SHOULD_NOT_RETURN.
         * This is normal. When a debugger is attached the nrf device needs
         * to hold resrouces so debug can continue; therefore not truly powering
         * down. When no debugger is attached this function will not return.
         * An interrupt or event is required, at which point the device will
         * reset. This is probably not the behavior that I am going to want;
         * so bottom line: do not call this unless the absolute lowest power
         * is required.
         */
        error_code = sd_power_system_off();
        if (error_code != NRF_SUCCESS)
        {
            logger.error("error: sd_power_system_off() failed: 0x%x", error_code);
        }
        break;

    default:
        break;
    }
}

void advertising_init(uint8_t ble_connection_tag)
{
    // The advertising interval, 0.625 ms increments.
    uint32_t const advertising_interval  = 300u;

    // The advertising timeout, in seconds.
    uint32_t const advertising_timeout   = 180u;

    ble_advertising_init_t init;
    memset(&init, 0, sizeof(init));

    init.advdata.name_type               = BLE_ADVDATA_FULL_NAME;
    init.advdata.include_appearance      = true;
    init.advdata.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    init.advdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    init.advdata.uuids_complete.p_uuids  = m_adv_uuids;

    init.config.ble_adv_fast_enabled     = true;
    init.config.ble_adv_fast_interval    = advertising_interval;
    init.config.ble_adv_fast_timeout     = advertising_timeout;
    init.evt_handler                     = advertising_event_handler;

    ret_code_t error_code = ble_advertising_init(&m_advertising, &init);
    ASSERT(error_code == NRF_SUCCESS);

    ble_advertising_conn_cfg_tag_set(&m_advertising, ble_connection_tag);
}

/**
 * Function for starting advertising.
 * @todo - from the original template example:
 *  Advertising is started by PM_EVT_PEERS_DELETED_SUCEEDED event.
 *  What does this mean?
 */
void advertising_start(bool erase_bonds)
{
    if (erase_bonds == true)
    {
        logger &logger = logger::instance();
        logger.info("erasing bonds!");

        ret_code_t const error_code = pm_peers_delete();
        if (error_code != NRF_SUCCESS)
        {
            logger.error("error: pm_peers_delete() failed: %u", error_code);
        }
    }
    else
    {
        ret_code_t error_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
        ASSERT(error_code == NRF_SUCCESS);
    }
}

