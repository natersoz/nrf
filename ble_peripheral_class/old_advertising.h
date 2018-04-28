/**
 * @file ble_peripheral_class/advertising.h
 */

#pragma once

/**
 * @todo pass in the m_adv_uuids?
 *
 * @param ble_connection_tag From Nordic Q&A support:
 * A unique key for keeping track of an advertising configuration.
 *
 * @details In some future release of the SoftDevice you might use
 * sd_ble_gap_adv_set_configure() to set a new advertising configuration and
 * receive a handle for it. Currently (v6.0.0) the maximum number of
 * advertising configurations is one, which means you only have the
 * configuration with handle 1.
 * (But you can change that configuration runtime if you want to.)
 * The maximum number of advertising sets supported by the SoftDevice
 * is found in BLE_GAP_ADV_SET_COUNT_MAX.
 */
void advertising_init(uint8_t ble_connection_tag);

/**
 * Start advertising
 *
 * @param erase_bonds If true, delete bonds from the peer manager before
 * advertising. This should probably not be embedded in here.
 */
void advertising_start(bool erase_bonds);


