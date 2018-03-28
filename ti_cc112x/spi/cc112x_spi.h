/**
 * @file cc112x_spi.h
 * @copyright (c) 2016, natersoz, LLC
 *
 * Declare the cc112x_spi interface.
 * CC112X device SPI read/write
 * @cite SWRU295 CC112X/CC1175
 *
 * SCLK Max:    10.0 MHz, f_xosc = 40 MHz
 *               6.1 MHz, f_xosc = 32 MHz
 * CC112X/CC1175 Figure 3: Configuration Registers Write and Read Operations
 * CC1101 Figure 15 Configuration Registers Write and Read Operations
 *   The SPI timing diagram is identical:
 *   CPOL = 0: SCLK rising edge is leading edge,
 *   CPHA = 0: Inputs latch data in on rising edge.
 *             Outputs transition on falling edge.
 */

#pragma once
#include <stdint.h>
#include <stddef.h>
#include "cc112x_registers.h"

/**
 * The asynchronous SPI transfer completion callback function prototype.
 * This function type is used to provide notifications that a SPI
 * asynchronous transfer has completed.
 */
typedef void (* cc112x_spi_completion)(void);

/**
 * Initialize the CC112x SPI interface
 */
void cc112x_spi_init(void);

/**
 * Write a single byte to a CC112x SPI register.
 *
 * @param reg_addr  The register to read from.
 * @param reg_value The value to write to the register.
 */
void cc112x_spi_write_reg(enum CC112X_REGISTER reg_addr, uint8_t reg_value);

/**
 * Read the byte value from a CC112x SPI register
 *
 * @param reg_addr  The register to read from.
 * @return uint8_t  The value read from the register.
 */
uint8_t cc112x_spi_read_reg(enum CC112X_REGISTER reg_addr);

/**
 * Write a single byte to an extended regsister.
 *
 * @param reg_ext_addr   The extended register to write.
 * @param value The value to write to the register.
 *
 * @return uint8_t The status byte received from the write operation.
 */
uint8_t cc11xx_spi_write_reg_ext(enum CC112X_REGISTER_EXT reg_ext_addr, uint8_t value);

/**
 * Read the byte value from a CC112x extended register
 *
 * @param reg_ext_addr  The register to read from.
 * @return uint8_t  The value read from the register.
 */
uint8_t cc112x_spi_read_reg_ext(enum CC112X_REGISTER_EXT reg_ext_addr);

/**
 * Write and Read buffers of data starting from the specified base
 * address register. This transfer is synchronous. When this function
 * returns the transfer is complete.
 *
 * @param reg_addr  The base address to begin the transfer of data
 *                  to/from the cc112x device.
 * @param data_mosi The data to write to the cc112x device.
 * @note            The parameter data_mosi can be NULL in which case
 *                  no data is written to the device.
 * @param data_miso The data to read from the cc112x device.
 * @note            The parameter data_miso can be NULL in which case
 *                  no data is read from the device.
 * @param length    The number of bytes to write/read to/from the device.
 * @note            The length of the transfer does not include the register.
 */
void cc112x_spi_write_read(enum CC112X_REGISTER         reg_addr,
                           enum CC112X_REGISTER_ACCESS  access,
                           void const                   *data_mosi,
                           void                         *data_miso,
                           size_t                       length);

/**
 * Write and Read buffers of data starting from the specified base
 * address register. This transfer is asynchronous.
 *
 * @param reg_addr  The base address to begin the transfer of data
 *                  to/from the cc112x device.
 * @param data_mosi The data to write to the cc112x device.
 * @note            The parameter data_mosi can be NULL in which case
 *                  no data is written to the device.
 * @param data_miso The data to read from the cc112x device.
 * @note            The parameter data_miso can be NULL in which case
 *                  no data is read from the device.
 * @param length    The number of bytes to write/read to/from the device.
 * @note            The length of the transfer does not include the register.
 * @param completion The callback that is called with the asynchronous transfer
 *                  has completed.
 */
void cc112x_spi_write_read_async(enum CC112X_REGISTER           reg_addr,
                                 enum CC112X_REGISTER_ACCESS    access,
                                 void const                     *data_mosi,
                                 void                           *data_miso,
                                 size_t                         length,
                                 cc112x_spi_completion          completion);


