/**
 * @file nordic_ble_stack_init.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once

#include <stdint.h>

namespace nordic
{

/**
 * Initialize the Nordic Softdevice.
 *
 * @param ram_base_address The Nordic softdevice requires its data allocation
 *        start at 0x20000000 and the allocation will vary based on the
 *        gatt parameters provided. The ram_base_address signifies the
 *        softdevice allocation as the size: ram_base_address - 0x20000000.
 *        If the softdevice requires more that the amount allocated this
 *        function will return NRF_ERROR_NO_MEM (4u).
 * @param total_link_count The number of connections; both peripheral and
 *        central required to be maintained by the softdevice.
 * @param peripheral_link_count The number of peripheral connections that
 *        must be managed by the softdevice.
 * @param mtu_size The maximum MTU size required by the connections.
 * @param gatt_uuid_count The number of GATT entries.
 * @param gatt_table_size The size of the GATT database.
 * @param service_changed true if the service parameters will be changed
 *        once a connection is established.
 *
 * @return ret_code_t The error or success code.
 * @retval NRF_SUCCESS The initialization succeded.
 * @retval NRF_ERROR_NO_MEM (4u) The ram_base_address parameter was too low;
 *         not enough memory was reserved for use by the Nordic softdevice.
 *
 * @param conn_cfg_tag
 * It appears that conn_cfg_tag is associated with an advertising profile
 * and a conection configuration (in this function).
 *
 * @note Must be different for all connection configurations added and
 * not @ref BLE_CONN_CFG_TAG_DEFAULT.
 *
 * @todo There are lots of questions to be answered regarding the GATT database
 * and GATT uuid per service requirements.
 */
uint32_t softdevice_init(uintptr_t    ram_base_address,
                         uint8_t      total_link_count,
                         uint8_t      peripheral_link_count,
                         uint16_t     mtu_size,
                         uint8_t      gatt_uuid_count,
                         uint32_t     gatt_table_size,
                         bool         service_changed);

} // namespace nordic

