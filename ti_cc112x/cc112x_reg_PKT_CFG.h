/**
 * @file cc112x_reg_PKT_CFG.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 */

#pragma once
#include <stdint.h>
#include "bit_manip.h"

/**
 * PKT_CFG2 - Packet Configuration Reg. 2
 * Bit  Name                    Reset   R/W
 * 7:6  PKT_CFG2_NOT_USED       0x00    R
 * 5    PKT_CFG2_RESERVED5      0x00    R/W
 *      For test purposes only. Use SmartRF studio values.
 * 4:2  CCA_MODE                0x01    R/W
 *      CCA mode.  Selects the definition of a clear channel (when to assert the CCA signal).
 *      000 Indicate clear channel always.
 *      001 Indicate clear channel when RSSI is below threshold.
 *      010 Indicate clear channel unless currently receiving a packet.
 *      011 Indicate clear channel when RSSI is below threshold and not
 *          currently receiving a packet.
 *      100 Indicate clear channel when RSSI is below threshold and ETSI LBT requirements are met.
 *     >100 Reserved
 *
 * 1:0  PKT_FORMAT              0x00    R/W
 *      Packet Format Configuration
 *      00 Normal mode/FIFO mode
 *         MDMCFG1.FIFO_EN must be set to 1 and
 *         MDMCFG0.TRANSPARENT_MODE_EN must be set to 0
 *      01 Synchronous serial mode
 *         MDMCFG1.FIFO_EN must be set to 0 and
 *         MDMCFG0.TRANSPARENT_MODE_EN must be set to 0
 *         This mode is only supported for 2’ary modulation formats.
 *      10 Random mode.
 *         Send random data using PN9 generator
 *         Set TXLAST != TXFIRST before strobing STX
 *      11 Transparent serial mode
 *         MDMCFG1.FIFO_EN must be set to 0 and
 *         MDMCFG0.TRANSPARENT_MODE_EN must be set to 1.
 *         This mode is only supported for 2’ary modulation formats
 */
uint8_t cc112x_reg_PKT_CFG2_xxx_set(uint8_t value);
uint8_t cc112x_reg_PKT_CFG2_xxx_get(void);

/**
 * PKT_CFG1 - Packet Configuration Reg. 1
 * Bit  Name                    Reset   R/W
 * 7    PKT_CFG1_NOT_USED       0x00    R
 *
 * 6    WHITE_DATA              0x00    R/W
 *      Whitening enable
 *      0 Data whitening disabled
 *      1 Data whitening enabled
 *
 * 5:4  ADDR_CHECK_CFG          0x00    R/W
 *      Address check configuration. Controls how address check is performed in RX mode
 *      00 No address check
 *      01 Address check, no broadcast
 *      10 Address check, 0x00 broadcast
 *      11 Address check, 0x00 and 0xFF broadcast
 *
 * 3:2  CRC_CFG                 0x01    R/W
 *      CRC configuration
 *      00 CRC disabled for TX and RX
 *      01 CRC calculation in TX mode and CRC check in RX mode enabled.
 *      CRC16 (X16+X15+X2+1). Initialized to 0xFFFF
 *      10 CRC calculation in TX mode and CRC check in RX mode enabled.
 *      CRC16 (X16+X12+X5+1). Initialized to 0x0000
 *      11 Reserved
 *
 * 1    BYTE_SWAP_EN            0x00    R/W
 *      TX/RX data byte swap enable.
 *      In RX, all bits in the received data byte are swapped before written to the RX FIFO.
 *      In TX, all bits in the TX FIFO data byte are swapped before being transmitted
 *      0 Data byte swap disabled
 *      1 Data byte swap enabled
 *
 * 0    APPEND_STATUS           0x01    R/W
 *      Append status bytes to RX FIFO.
 *      The status bytes contain info about CRC, RSSI, and LQI.
 *      When CRC_CFG = 0, the CRC_OK field in the status byte will be 0.
 *      0 Status byte not appended
 *      1 Status byte appended
 */
uint8_t cc112x_reg_PKT_CFG1_xxx_set(uint8_t value);
uint8_t cc112x_reg_PKT_CFG1_xxx_get(void);

/**
 * PKT_CFG1 - Packet Configuration Reg. 0
 * Bit  Name                    Reset   R/W
 * 7    PKT_CFG0_RESERVED7      0x00    R
 *      For test purposes only, use values from SmartRF Studio
 *
 * 6:5  LENGTH_CONFIG           0x00    R/W
 *      Packet Length Configuration
 *      00 Fixed packet length mode.
 *         Packet length configured through the PKT_LEN register
 *      01 Variable packet length mode.
 *         Packet length configured by the first byte received after sync word
 *      10 Infinite packet length mode
 *      11 Variable packet length mode.
 *         Length configured by the 5 LSB of the first byte received after sync word
 *
 * 4:3  PKT_BIT_LEN             0x00    R/W
 *      In fixed packet length mode this field (when not zero) indicates the number of bits
 *      to send/receive after PKT_LEN number of bytes are sent/received.
 *      CRC is not supported when PKT_LEN_BIT != 0
 *
 * 1    UART_MODE_EN            0x00    R/W
 *      UART mode enable.
 *      When enabled, the packet engine will insert/remove a start and stop bit to/from
 *      the transmitted/received bytes
 *      0 UART mode disabled
 *      1 UART mode enabled
 *  0   UART_SWAP_EN            0x00    R/W
 *      Swap start and stop bits values
 *      0 Swap disabled. Start/stop bits values are '1'/'0'
 *      1 Swap  enabled. Start/stop bits values are '0'/'1'
 */
uint8_t cc112x_reg_PKT_CFG0_xxx_set(uint8_t value);
uint8_t cc112x_reg_PKT_CFG0_xxx_get(void);

