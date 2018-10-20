/**
 * @file doc/nordic_ble_conn_cfg.h
 * Document Nordic's ble_cfg_t union type.
 *
 * It is frustrating that Nordic does not provide adequate documentation
 * of their softdevice.
 *
 *
 * see softdevice header 'ble.h'
 *
 *  union ble_cfg_t {
 *      .conn_cfg =
 *      struct ble_conn_cfg_t {
 *
 *          conn_cfg_tag represents a configuration id.
 *          The GAP, GATTS, GATTC, ad GATT can be set within the union of
 *          the config type ble_cfg_t::ble_conn_cfg_t.
 *          It appears that this is how Nordic is aggregating these
 *          configurations within a table (likely indexed by conn_cfg_tag).
 *
 *          uint8_t conn_cfg_tag
 *
 *          .params =
 *          union {
 *              .gap_conn_cfg =
 *              struct ble_gap_conn_cfg_t {
 *
 *                  The number of concurrent connections the application can create.
 *                  The default and minimum value is @ref BLE_GAP_CONN_COUNT_DEFAULT.
 *
 *                  uint8_t conn_count;
 *
 *                  The time set aside for this connection on every connection interval
 *                  in 1.25 ms units.
 *                  The default value is @ref BLE_GAP_EVENT_LENGTH_DEFAULT,
 *                  the minimum value is @ref BLE_GAP_EVENT_LENGTH_MIN.
 *                  The event length and the connection interval are the primary parameters
 *                  for setting the throughput of a connection.
 *
 *                  uint16_t event_length;
 *              }
 *              .gattc_conn_cfg =
 *              struct ble_gattc_conn_cfg_t {
 *
 *                  The guaranteed minimum number of Write without Response that
 *                  can be queued for transmission.
 *                  The default value is @ref BLE_GATTC_WRITE_CMD_TX_QUEUE_SIZE_DEFAULT
 *
 *                  uint8_t write_cmd_tx_queue_size;
 *              }
 *              .gatts_conn_cfg =
 *              struct ble_gatts_conn_cfg_t {
 *
 *                  The minimum guaranteed number of Handle Value Notifications
 *                  (indications and notifications) that can be queued for transmission.
 *                  The default value is @ref BLE_GATTS_HVN_TX_QUEUE_SIZE_DEFAULT.
 *
 *                  uint8_t hvn_tx_queue_size;
 *              }
 *              .gatt_conn_cfg =
 *              struct ble_gatt_conn_cfg_t {
 *
 *                  The maximum ATT packet length the SoftDevice can send or receive.
 *                  The default and minimum value is @ref BLE_GATT_ATT_MTU_DEFAULT.
 *
 *                  uint16_t att_mtu;
 *              }
 *          }
 *      }
 *
 *      .common_cfg =
 *      union ble_common_cfg_t {
 *          .vs_uuid_cfg =
 *          struct ble_common_cfg_vs_uuid_t {
 *              Number of 128-bit Vendor Specific UUID bases to allocate memory for.
 *              Default value is @ref BLE_UUID_VS_COUNT_DEFAULT. Maximum value is
 *              @ref BLE_UUID_VS_COUNT_MAX.
 *
 *              uint8_t vs_uuid_count;
 *          }
 *      }
 *
 *      .gap_cfg =
 *      union ble_gap_cfg_t {
 *          .role_count_cfg =
 *          struct ble_gap_cfg_role_count_t {
 *
 *              Maximum number of connections concurrently acting as a peripheral.
 *              Default value is @ref BLE_GAP_ROLE_COUNT_PERIPH_DEFAULT.
 *
 *              uint8_t periph_role_count;
 *
 *              Maximum number of connections concurrently acting as a central.
 *              Default value is @ref BLE_GAP_ROLE_COUNT_CENTRAL_DEFAULT.
 *
 *              uint8_t central_role_count;
 *
 *              Number of SMP instances shared between all connections acting as a central.
 *              Default value is @ref BLE_GAP_ROLE_COUNT_CENTRAL_SEC_DEFAULT.
 *
 *              uint8_t central_sec_count;
 *          }
 *
 *          .device_name_cfg =
 *          struct ble_gap_cfg_device_name_t {
 *              ble_gap_conn_sec_mode_t  write_perm;
 *              uint8_t                  vloc:2;
 *              uint8_t                 *p_value;
 *              uint16_t                 current_len;
 *              uint16_t                 max_len;
 *          }
 *
 *      .gatts_cfg =
 *      union ble_gatts_cfg_t {
 *          .service_changed =
 *          struct ble_gatts_cfg_service_changed_t {
 *
 *              If 1, include the Service Changed characteristic in the Attribute Table.
 *              Default is @ref BLE_GATTS_SERVICE_CHANGED_DEFAULT.
 *
 *              uint8_t : 1 service_changed;
 *          }
 *
 *          .attr_tab_size =
 *          struct ble_gatts_cfg_attr_tab_size_t {
 *
 *              GATT attribute table size - in bytes?, in associated entries?
 *
 *              uint32_t attr_tab_size;
 *          }
 *      }
 *  }
 *
 */

