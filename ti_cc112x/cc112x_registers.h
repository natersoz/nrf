/**
 * @file cc112x_registers.h
 * @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
 * @see enum CC112X_REGISTER
 */

#pragma once

/**
 * @enum CC112X_REGISTER_ACCESS
 * CC112x register access modifiers.
 */
enum CC112X_REGISTER_ACCESS
{
    CC112X_REGISTER_ACCESS_READ         = 0x80,     ///< Register read.
    CC112X_REGISTER_ACCESS_WRITE        = 0x00,     ///< Register write.
    CC112X_REGISTER_ACCESS_BURST        = 0x40,     ///< Register burst read or write.
    CC112X_REGISTER_ACCESS_SINGLE       = 0x00,     ///< Single regsiter reado or write.
};

/**
 * @enum CC112X_REGISTER
 *
 * @cite SWRU295 CC112X/CC1175 Section 3.2 SPI Access Types
 * 0x00 - 0x2e  Configuration Registers
 * 0x2f         Exteneded Register Space
 *              @see enum CC112X_REGISTER_EXT
 * 0x30 - 0x3d  Command Strobes
 * 0x3e         FIFO addressing 0x00 - 0xff (not expected to be used)
 * 0x3f         FIFO Access, push/pop (this access method is used)
 *
 * The extended register set will be given a value 0x100 | extended register
 * within the context of this enum.
 */
enum CC112X_REGISTER
{
    CC112X_REGISTER_IOCFG3              = 0x00,
    CC112X_REGISTER_IOCFG2              = 0x01,
    CC112X_REGISTER_IOCFG1              = 0x02,
    CC112X_REGISTER_IOCFG0              = 0x03,
    CC112X_REGISTER_SYNC3               = 0x04,
    CC112X_REGISTER_SYNC2               = 0x05,
    CC112X_REGISTER_SYNC1               = 0x06,
    CC112X_REGISTER_SYNC0               = 0x07,
    CC112X_REGISTER_SYNC_CFG1           = 0x08,
    CC112X_REGISTER_SYNC_CFG0           = 0x09,
    CC112X_REGISTER_DEVIATION_M         = 0x0A,
    CC112X_REGISTER_MODCFG_DEV_E        = 0x0B,
    CC112X_REGISTER_DCFILT_CFG          = 0x0C,
    CC112X_REGISTER_PREAMBLE_CFG1       = 0x0D,
    CC112X_REGISTER_PREAMBLE_CFG0       = 0x0E,
    CC112X_REGISTER_FREQ_IF_CFG         = 0x0F,
    CC112X_REGISTER_IQIC                = 0x10,
    CC112X_REGISTER_CHAN_BW             = 0x11,
    CC112X_REGISTER_MDMCFG1             = 0x12,
    CC112X_REGISTER_MDMCFG0             = 0x13,
    CC112X_REGISTER_SYMBOL_RATE2        = 0x14,
    CC112X_REGISTER_SYMBOL_RATE1        = 0x15,
    CC112X_REGISTER_SYMBOL_RATE0        = 0x16,
    CC112X_REGISTER_AGC_REF             = 0x17,
    CC112X_REGISTER_AGC_CS_THR          = 0x18,
    CC112X_REGISTER_AGC_GAIN_ADJUST     = 0x19,
    CC112X_REGISTER_AGC_CFG3            = 0x1A,
    CC112X_REGISTER_AGC_CFG2            = 0x1B,
    CC112X_REGISTER_AGC_CFG1            = 0x1C,
    CC112X_REGISTER_AGC_CFG0            = 0x1D,
    CC112X_REGISTER_FIFO_CFG            = 0x1E,
    CC112X_REGISTER_DEV_ADDR            = 0x1F,
    CC112X_REGISTER_SETTLING_CFG        = 0x20,
    CC112X_REGISTER_FS_CFG              = 0x21,
    CC112X_REGISTER_WOR_CFG1            = 0x22,
    CC112X_REGISTER_WOR_CFG0            = 0x23,
    CC112X_REGISTER_WOR_EVENT0_MSB      = 0x24,
    CC112X_REGISTER_WOR_EVENT0_LSB      = 0x25,
    CC112X_REGISTER_PKT_CFG2            = 0x26,
    CC112X_REGISTER_PKT_CFG1            = 0x27,
    CC112X_REGISTER_PKT_CFG0            = 0x28,
    CC112X_REGISTER_RFEND_CFG1          = 0x29,
    CC112X_REGISTER_RFEND_CFG0          = 0x2A,
    CC112X_REGISTER_PA_CFG2             = 0x2B,
    CC112X_REGISTER_PA_CFG1             = 0x2C,
    CC112X_REGISTER_PA_CFG0             = 0x2D,
    CC112X_REGISTER_PKT_LEN             = 0x2E,
    CC112X_REGISTER_EXTENDED_ADDRESS    = 0x2F,
};

/**
 * @enum CC112X_STROBE
 */
enum CC112X_STROBE
{
    CC112X_STROBE_SRES                  = 0x30,
    CC112X_STROBE_SFSTXON               = 0x31,
    CC112X_STROBE_SXOFF                 = 0x32,
    CC112X_STROBE_SCAL                  = 0x33,
    CC112X_STROBE_SRX                   = 0x34,
    CC112X_STROBE_STX                   = 0x35,
    CC112X_STROBE_SIDLE                 = 0x36,
    CC112X_STROBE_SAFC                  = 0x37,
    CC112X_STROBE_SWOR                  = 0x38,
    CC112X_STROBE_SPWD                  = 0x39,
    CC112X_STROBE_SFRX                  = 0x3A,
    CC112X_STROBE_SFTX                  = 0x3B,
    CC112X_STROBE_SWORRST               = 0x3C,
    CC112X_STROBE_SNOP                  = 0x3D,
};

/**
 * @enum CC112X_REGISTER_EXT
 * CC112X extended regsiter address space.
 */
enum CC112X_REGISTER_EXT
{
    CC112X_REGISTER_IF_MIX_CFG          = 0x00,
    CC112X_REGISTER_FREQOFF_CFG         = 0x01,
    CC112X_REGISTER_TOC_CFG             = 0x02,
    CC112X_REGISTER_MARC_SPARE          = 0x03,
    CC112X_REGISTER_ECG_CFG             = 0x04,
    CC112X_REGISTER_CFM_DATA_CFG        = 0x05,
    CC112X_REGISTER_EXT_CTRL            = 0x06,
    CC112X_REGISTER_RCCAL_FINE          = 0x07,
    CC112X_REGISTER_RCCAL_COARSE        = 0x08,
    CC112X_REGISTER_RCCAL_OFFSET        = 0x09,
    CC112X_REGISTER_FREQOFF1            = 0x0A,
    CC112X_REGISTER_FREQOFF0            = 0x0B,
    CC112X_REGISTER_FREQ2               = 0x0C,
    CC112X_REGISTER_FREQ1               = 0x0D,
    CC112X_REGISTER_FREQ0               = 0x0E,
    CC112X_REGISTER_IF_ADC2             = 0x0F,
    CC112X_REGISTER_IF_ADC1             = 0x10,
    CC112X_REGISTER_IF_ADC0             = 0x11,
    CC112X_REGISTER_FS_DIG1             = 0x12,
    CC112X_REGISTER_FS_DIG0             = 0x13,
    CC112X_REGISTER_FS_CAL3             = 0x14,
    CC112X_REGISTER_FS_CAL2             = 0x15,
    CC112X_REGISTER_FS_CAL1             = 0x16,
    CC112X_REGISTER_FS_CAL0             = 0x17,
    CC112X_REGISTER_FS_CHP              = 0x18,
    CC112X_REGISTER_FS_DIVTWO           = 0x19,
    CC112X_REGISTER_FS_DSM1             = 0x1A,
    CC112X_REGISTER_FS_DSM0             = 0x1B,
    CC112X_REGISTER_FS_DVC1             = 0x1C,
    CC112X_REGISTER_FS_DVC0             = 0x1D,
    CC112X_REGISTER_FS_LBI              = 0x1E,
    CC112X_REGISTER_FS_PFD              = 0x1F,
    CC112X_REGISTER_FS_PRE              = 0x20,
    CC112X_REGISTER_FS_REG_DIV_CML      = 0x21,
    CC112X_REGISTER_FS_SPARE            = 0x22,
    CC112X_REGISTER_FS_VCO4             = 0x23,
    CC112X_REGISTER_FS_VCO3             = 0x24,
    CC112X_REGISTER_FS_VCO2             = 0x25,
    CC112X_REGISTER_FS_VCO1             = 0x26,
    CC112X_REGISTER_FS_VCO0             = 0x27,
    CC112X_REGISTER_GBIAS6              = 0x28,
    CC112X_REGISTER_GBIAS5              = 0x29,
    CC112X_REGISTER_GBIAS4              = 0x2A,
    CC112X_REGISTER_GBIAS3              = 0x2B,
    CC112X_REGISTER_GBIAS2              = 0x2C,
    CC112X_REGISTER_GBIAS1              = 0x2D,
    CC112X_REGISTER_GBIAS0              = 0x2E,
    CC112X_REGISTER_IFAMP               = 0x2F,

    CC112X_REGISTER_LNA                 = 0x30,
    CC112X_REGISTER_RXMIX               = 0x31,
    CC112X_REGISTER_XOSC5               = 0x32,
    CC112X_REGISTER_XOSC4               = 0x33,
    CC112X_REGISTER_XOSC3               = 0x34,
    CC112X_REGISTER_XOSC2               = 0x35,
    CC112X_REGISTER_XOSC1               = 0x36,
    CC112X_REGISTER_XOSC0               = 0x37,
    CC112X_REGISTER_ANALOG_SPARE        = 0x38,
    CC112X_REGISTER_PA_CFG3             = 0x39,

    // 0x3a - 0x3e: Not Used
    // 0x3f - 0x40: Resererved
    // 0x41 - 0x63: Not Used

    CC112X_REGISTER_WOR_TIME1           = 0x64,
    CC112X_REGISTER_WOR_TIME0           = 0x65,
    CC112X_REGISTER_WOR_CAPTURE1        = 0x66,
    CC112X_REGISTER_WOR_CAPTURE0        = 0x67,
    CC112X_REGISTER_BIST                = 0x68,
    CC112X_REGISTER_DCFILTOFFSET_I1     = 0x69,
    CC112X_REGISTER_DCFILTOFFSET_I0     = 0x6A,
    CC112X_REGISTER_DCFILTOFFSET_Q1     = 0x6B,
    CC112X_REGISTER_DCFILTOFFSET_Q0     = 0x6C,
    CC112X_REGISTER_IQIE_I1             = 0x6D,
    CC112X_REGISTER_IQIE_I0             = 0x6E,
    CC112X_REGISTER_IQIE_Q1             = 0x6F,
    CC112X_REGISTER_IQIE_Q0             = 0x70,
    CC112X_REGISTER_RSSI1               = 0x71,
    CC112X_REGISTER_RSSI0               = 0x72,
    CC112X_REGISTER_MARCSTATE           = 0x73,
    CC112X_REGISTER_LQI_VAL             = 0x74,
    CC112X_REGISTER_PQT_SYNC_ERR        = 0x75,
    CC112X_REGISTER_DEM_STATUS          = 0x76,
    CC112X_REGISTER_FREQOFF_EST1        = 0x77,
    CC112X_REGISTER_FREQOFF_EST0        = 0x78,
    CC112X_REGISTER_AGC_GAIN3           = 0x79,
    CC112X_REGISTER_AGC_GAIN2           = 0x7A,
    CC112X_REGISTER_AGC_GAIN1           = 0x7B,
    CC112X_REGISTER_AGC_GAIN0           = 0x7C,
    CC112X_REGISTER_CFM_RX_DATA_OUT     = 0x7D,
    CC112X_REGISTER_CFM_TX_DATA_IN      = 0x7E,
    CC112X_REGISTER_ASK_SOFT_RX_DATA    = 0x7F,
    CC112X_REGISTER_RNDGEN              = 0x80,
    CC112X_REGISTER_MAGN2               = 0x81,
    CC112X_REGISTER_MAGN1               = 0x82,
    CC112X_REGISTER_MAGN0               = 0x83,
    CC112X_REGISTER_ANG1                = 0x84,
    CC112X_REGISTER_ANG0                = 0x85,
    CC112X_REGISTER_CHFILT_I2           = 0x86,
};
