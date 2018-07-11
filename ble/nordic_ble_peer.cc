/**
 * @file ble/nordic_ble_peer.cc
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#include "ble/nordic_ble_peer.h"
#include "peer_manager.h"
#include "logger.h"

#include "peer_manager.h"
#include "ble_conn_state.h"
#include "fds.h"

/**@brief Function for handling Peer Manager events.
 *
 * @param[in] p_evt  Peer Manager event.
 */
static void pm_evt_handler(pm_evt_t const * p_evt)
{
    switch (p_evt->evt_id)
    {
        case PM_EVT_BONDED_PEER_CONNECTED:
        {
            logger &logger = logger::instance();
            logger.info("Connected to a previously bonded device.");
        } break;

        case PM_EVT_CONN_SEC_SUCCEEDED:
        {
            logger &logger = logger::instance();
            logger.info("Connection secured: role: %d, conn_handle: 0x%x, procedure: %d.",
                        ble_conn_state_role(p_evt->conn_handle),
                        p_evt->conn_handle,
                        p_evt->params.conn_sec_succeeded.procedure);
        } break;

        case PM_EVT_CONN_SEC_FAILED:
        {
            /* Often, when securing fails, it shouldn't be restarted, for security reasons.
             * Other times, it can be restarted directly.
             * Sometimes it can be restarted, but only after changing some Security Parameters.
             * Sometimes, it cannot be restarted until the link is disconnected and reconnected.
             * Sometimes it is impossible, to secure the link, or the peer device does not support it.
             * How to handle this error is highly application dependent. */
        } break;

        case PM_EVT_CONN_SEC_CONFIG_REQ:
        {
            // Reject pairing request from an already bonded peer.
            pm_conn_sec_config_t conn_sec_config = {.allow_repairing = false};
            pm_conn_sec_config_reply(p_evt->conn_handle, &conn_sec_config);
        } break;

        case PM_EVT_STORAGE_FULL:
        {
            // Run garbage collection on the flash.
            ret_code_t const error_code = fds_gc();
            if (error_code == FDS_ERR_BUSY || error_code == FDS_ERR_NO_SPACE_IN_QUEUES)
            {
                // Retry.
            }
            else
            {
                ASSERT(error_code == NRF_SUCCESS);
            }
        } break;

        case PM_EVT_PEERS_DELETE_SUCCEEDED:
        {
            /// @todo restart advertising.
//            advertising_start(false);
        } break;

        case PM_EVT_LOCAL_DB_CACHE_APPLY_FAILED:
        {
            // The local database has likely changed, send service changed indications.
            pm_local_database_has_changed();
        } break;

        case PM_EVT_PEER_DATA_UPDATE_FAILED:
        {
            ASSERT(p_evt->params.peer_data_update_failed.error == NRF_SUCCESS);
        } break;

        case PM_EVT_PEER_DELETE_FAILED:
        {
            ASSERT(p_evt->params.peer_delete_failed.error == NRF_SUCCESS);
        } break;

        case PM_EVT_PEERS_DELETE_FAILED:
        {
            ASSERT(p_evt->params.peers_delete_failed_evt.error == NRF_SUCCESS);
        } break;

        case PM_EVT_ERROR_UNEXPECTED:
        {
            ASSERT(p_evt->params.error_unexpected.error == NRF_SUCCESS);
        } break;

        case PM_EVT_CONN_SEC_START:
        case PM_EVT_PEER_DATA_UPDATE_SUCCEEDED:
        case PM_EVT_PEER_DELETE_SUCCEEDED:
        case PM_EVT_LOCAL_DB_CACHE_APPLIED:
        case PM_EVT_SERVICE_CHANGED_IND_SENT:
        case PM_EVT_SERVICE_CHANGED_IND_CONFIRMED:
        default:
            break;
    }
}

void ble_peer_init()
{
    ret_code_t error_code = pm_init();
    ASSERT(error_code == NRF_SUCCESS);

    ble_gap_sec_params_t sec_param;
    memset(&sec_param, 0, sizeof(ble_gap_sec_params_t));

    // Security parameters to be used for all security procedures.
    sec_param.bond           = true;            // bonding enable
    sec_param.mitm           = false;           // man-in-the-middle protection
    sec_param.lesc           = false;           // secure connection required
    sec_param.keypress       = false;           // enable keypress notifications
    sec_param.io_caps        = BLE_GAP_IO_CAPS_NONE;
    sec_param.oob            = false;           // out of band authentication
    sec_param.min_key_size   =  7u;
    sec_param.max_key_size   = 16u;
    sec_param.kdist_own.enc  = true;            // long term key and master id
    sec_param.kdist_own.id   = true;            // identity resolving key/address
    sec_param.kdist_peer.enc = true;
    sec_param.kdist_peer.id  = true;

    error_code = pm_sec_params_set(&sec_param);
    ASSERT(error_code == NRF_SUCCESS);

    error_code = pm_register(pm_evt_handler);
    ASSERT(error_code == NRF_SUCCESS);
}

