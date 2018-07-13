/**@brief Function for handling BLE events.
 *
 * @param  p_ble_evt   Bluetooth stack event.
 * @param  p_context   Unused.
 */
/// @todo This gets removed and replaced by an instance of
/// nordic::ble_gap_event_observer.
static void ble_evt_handler(ble_evt_t const *ble_event, void *context)
{
    ret_code_t error_code = NRF_SUCCESS;
    logger &logger = logger::instance();

    switch (ble_event->header.evt_id)
    {
        case BLE_GAP_EVT_DISCONNECTED:
            logger.info("Disconnected.");
            break;

        case BLE_GAP_EVT_CONNECTED:
            m_conn_handle = ble_event->evt.gap_evt.conn_handle;
            logger.info("Connected, handle: 0x%04x", m_conn_handle);
            break;

#ifndef S140
        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
        {
            logger.debug("PHY update request.");
            ble_gap_phys_t const phys =
            {
                .tx_phys = BLE_GAP_PHY_AUTO,
                .rx_phys = BLE_GAP_PHY_AUTO,
            };
            error_code = sd_ble_gap_phy_update(ble_event->evt.gap_evt.conn_handle, &phys);
            ASSERT(error_code == NRF_SUCCESS);
        } break;
#endif

        case BLE_GATTC_EVT_TIMEOUT:
            // Disconnect on GATT Client timeout event.
            logger.debug("GATT Client Timeout.");
            error_code = sd_ble_gap_disconnect(ble_event->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            ASSERT(error_code == NRF_SUCCESS);
            break;

        case BLE_GATTS_EVT_TIMEOUT:
            // Disconnect on GATT Server timeout event.
            logger.debug("GATT Server Timeout.");
            error_code = sd_ble_gap_disconnect(ble_event->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            ASSERT(error_code == NRF_SUCCESS);
            break;

        case BLE_EVT_USER_MEM_REQUEST:
            error_code = sd_ble_user_mem_reply(ble_event->evt.gattc_evt.conn_handle, NULL);
            ASSERT(error_code == NRF_SUCCESS);
            break;

        case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
        {
            ble_gatts_evt_rw_authorize_request_t  req;
            ble_gatts_rw_authorize_reply_params_t auth_reply;

            req = ble_event->evt.gatts_evt.params.authorize_request;

            if (req.type != BLE_GATTS_AUTHORIZE_TYPE_INVALID)
            {
                if ((req.request.write.op == BLE_GATTS_OP_PREP_WRITE_REQ)     ||
                    (req.request.write.op == BLE_GATTS_OP_EXEC_WRITE_REQ_NOW) ||
                    (req.request.write.op == BLE_GATTS_OP_EXEC_WRITE_REQ_CANCEL))
                {
                    if (req.type == BLE_GATTS_AUTHORIZE_TYPE_WRITE)
                    {
                        auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_WRITE;
                    }
                    else
                    {
                        auth_reply.type = BLE_GATTS_AUTHORIZE_TYPE_READ;
                    }
                    auth_reply.params.write.gatt_status = APP_FEATURE_NOT_SUPPORTED;
                    error_code = sd_ble_gatts_rw_authorize_reply(ble_event->evt.gatts_evt.conn_handle,
                                                               &auth_reply);
                    ASSERT(error_code == NRF_SUCCESS);
                }
            }
        } break; // BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST

        default:
            // No implementation needed.
            break;
    }
}

