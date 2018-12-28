/**
 * @file ble/nordic_ble_event_strings.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "ble/nordic_ble_event_strings.h"

#include "ble_gap.h"
#include "ble_gatts.h"
#include "ble_gattc.h"

// A really cheezy macro that converts a case statement enum-ish type
// into a string literal return value
#define CASE_STR(item)      case item: return #item;

namespace nordic
{

char const* ble_event_string(enum BLE_COMMON_EVTS ble_event)
{
    switch (ble_event)
    {
        CASE_STR(BLE_EVT_USER_MEM_REQUEST);
        CASE_STR(BLE_EVT_USER_MEM_RELEASE);
        default:    return "BLE_EVT_INVALID";
    }
}

char const* ble_gap_event_string(enum BLE_GAP_EVTS ble_gap_event)
{
    switch (ble_gap_event)
    {
        CASE_STR(BLE_GAP_EVT_CONNECTED);
        CASE_STR(BLE_GAP_EVT_DISCONNECTED);
        CASE_STR(BLE_GAP_EVT_CONN_PARAM_UPDATE);
        CASE_STR(BLE_GAP_EVT_SEC_PARAMS_REQUEST);
        CASE_STR(BLE_GAP_EVT_SEC_INFO_REQUEST);
        CASE_STR(BLE_GAP_EVT_PASSKEY_DISPLAY);
        CASE_STR(BLE_GAP_EVT_KEY_PRESSED);
        CASE_STR(BLE_GAP_EVT_AUTH_KEY_REQUEST);
        CASE_STR(BLE_GAP_EVT_LESC_DHKEY_REQUEST);
        CASE_STR(BLE_GAP_EVT_AUTH_STATUS);
        CASE_STR(BLE_GAP_EVT_CONN_SEC_UPDATE);
        CASE_STR(BLE_GAP_EVT_TIMEOUT);
        CASE_STR(BLE_GAP_EVT_RSSI_CHANGED);
        CASE_STR(BLE_GAP_EVT_ADV_REPORT);
        CASE_STR(BLE_GAP_EVT_SEC_REQUEST);
        CASE_STR(BLE_GAP_EVT_CONN_PARAM_UPDATE_REQUEST);
        CASE_STR(BLE_GAP_EVT_SCAN_REQ_REPORT);
        CASE_STR(BLE_GAP_EVT_PHY_UPDATE_REQUEST);
        CASE_STR(BLE_GAP_EVT_PHY_UPDATE);
        CASE_STR(BLE_GAP_EVT_DATA_LENGTH_UPDATE_REQUEST);
        CASE_STR(BLE_GAP_EVT_DATA_LENGTH_UPDATE);
        CASE_STR(BLE_GAP_EVT_QOS_CHANNEL_SURVEY_REPORT);
        CASE_STR(BLE_GAP_EVT_ADV_SET_TERMINATED);
        default:    return "BLE_GAP_EVT_INVALID";
    }
}

char const* ble_gatts_event_string(enum BLE_GATTS_EVTS ble_gatts_event)
{
    switch (ble_gatts_event)
    {
        CASE_STR(BLE_GATTS_EVT_WRITE);
        CASE_STR(BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST);
        CASE_STR(BLE_GATTS_EVT_SYS_ATTR_MISSING);
        CASE_STR(BLE_GATTS_EVT_HVC);
        CASE_STR(BLE_GATTS_EVT_SC_CONFIRM);
        CASE_STR(BLE_GATTS_EVT_EXCHANGE_MTU_REQUEST);
        CASE_STR(BLE_GATTS_EVT_TIMEOUT);
        CASE_STR(BLE_GATTS_EVT_HVN_TX_COMPLETE);
        default:    return "BLE_GATTS_EVT_INVALID";
    }
}

char const* ble_gattc_event_string(enum BLE_GATTC_EVTS ble_gattc_event)
{
    switch (ble_gattc_event)
    {
        CASE_STR(BLE_GATTC_EVT_PRIM_SRVC_DISC_RSP);
        CASE_STR(BLE_GATTC_EVT_REL_DISC_RSP);
        CASE_STR(BLE_GATTC_EVT_CHAR_DISC_RSP);
        CASE_STR(BLE_GATTC_EVT_DESC_DISC_RSP);
        CASE_STR(BLE_GATTC_EVT_ATTR_INFO_DISC_RSP);
        CASE_STR(BLE_GATTC_EVT_CHAR_VAL_BY_UUID_READ_RSP);
        CASE_STR(BLE_GATTC_EVT_READ_RSP);
        CASE_STR(BLE_GATTC_EVT_CHAR_VALS_READ_RSP);
        CASE_STR(BLE_GATTC_EVT_WRITE_RSP);
        CASE_STR(BLE_GATTC_EVT_HVX);
        CASE_STR(BLE_GATTC_EVT_EXCHANGE_MTU_RSP);
        CASE_STR(BLE_GATTC_EVT_TIMEOUT);
        CASE_STR(BLE_GATTC_EVT_WRITE_CMD_TX_COMPLETE);
        default:    return "BLE_GATTC_EVT_INVALID";
    }
}

char const* ble_l2cap_event_string(enum BLE_L2CAP_EVTS ble_l2cap_event)
{
    switch (ble_l2cap_event)
    {
        CASE_STR(BLE_L2CAP_EVT_CH_SETUP_REQUEST);
        CASE_STR(BLE_L2CAP_EVT_CH_SETUP_REFUSED);
        CASE_STR(BLE_L2CAP_EVT_CH_SETUP);
        CASE_STR(BLE_L2CAP_EVT_CH_RELEASED);
        CASE_STR(BLE_L2CAP_EVT_CH_SDU_BUF_RELEASED);
        CASE_STR(BLE_L2CAP_EVT_CH_CREDIT);
        CASE_STR(BLE_L2CAP_EVT_CH_RX);
        CASE_STR(BLE_L2CAP_EVT_CH_TX);
        default:    return "BLE_L2CAP_EVT_INVALID";
    }
}

char const* nrf_soc_event_string(enum NRF_SOC_EVTS nrf_soc_event)
{
    switch (nrf_soc_event)
    {
        CASE_STR(NRF_EVT_HFCLKSTARTED);
        CASE_STR(NRF_EVT_POWER_FAILURE_WARNING);
        CASE_STR(NRF_EVT_FLASH_OPERATION_SUCCESS);
        CASE_STR(NRF_EVT_FLASH_OPERATION_ERROR);
        CASE_STR(NRF_EVT_RADIO_BLOCKED);
        CASE_STR(NRF_EVT_RADIO_CANCELED);
        CASE_STR(NRF_EVT_RADIO_SIGNAL_CALLBACK_INVALID_RETURN);
        CASE_STR(NRF_EVT_RADIO_SESSION_IDLE);
        CASE_STR(NRF_EVT_RADIO_SESSION_CLOSED);
        CASE_STR(NRF_EVT_NUMBER_OF_EVTS);
        default:    return "NRF_SOC_EVT_INVALID";
    }
}

char const* event_string(uint16_t nordic_event)
{
    if ((nordic_event >= BLE_EVT_BASE) &&
        (nordic_event <= BLE_EVT_LAST))
    {
        auto const ble_common_event = static_cast<BLE_COMMON_EVTS>(nordic_event);
        return nordic::ble_event_string(ble_common_event);
    }
    else if ((nordic_event >= BLE_GAP_EVT_BASE) &&
             (nordic_event <= BLE_GAP_EVT_LAST))
    {
        auto const ble_gap_event = static_cast<BLE_GAP_EVTS>(nordic_event);
        return nordic::ble_gap_event_string(ble_gap_event);
    }
    else if ((nordic_event >= BLE_GATTC_EVT_BASE) &&
             (nordic_event <= BLE_GATTC_EVT_LAST))
    {
        auto const ble_gattc_event = static_cast<BLE_GATTC_EVTS>(nordic_event);
        return nordic::ble_gattc_event_string(ble_gattc_event);
    }
    else if ((nordic_event >= BLE_GATTS_EVT_BASE) &&
             (nordic_event <= BLE_GATTS_EVT_LAST))
    {
        auto const ble_gatts_event = static_cast<BLE_GATTS_EVTS>(nordic_event);
        return nordic::ble_gatts_event_string(ble_gatts_event);
    }
    else
    {
        return "NORDIC_INVALID_EVENT";
    }
}

} // namespace nordic
