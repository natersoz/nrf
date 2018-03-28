

// Rf settings for CC1120
RF_SETTINGS code rfSettings = {
    0xB0,  // IOCFG3              GPIO3 IO Pin Configuration
    0x06,  // IOCFG2              GPIO2 IO Pin Configuration
    0xB0,  // IOCFG1              GPIO1 IO Pin Configuration
    0x40,  // IOCFG0              GPIO0 IO Pin Configuration
    0x08,  // SYNC_CFG1           Sync Word Detection Configuration Reg. 1
    0x99,  // DEVIATION_M         Frequency Deviation Configuration
    0x0D,  // MODCFG_DEV_E        Modulation Format and Frequency Deviation Configur..
    0x15,  // DCFILT_CFG          Digital DC Removal Configuration
    0x18,  // PREAMBLE_CFG1       Preamble Length Configuration Reg. 1
    0x3A,  // FREQ_IF_CFG         RX Mixer Frequency Configuration
    0x00,  // IQIC                Digital Image Channel Compensation Configuration
    0x02,  // CHAN_BW             Channel Filter Configuration
    0x05,  // MDMCFG0             General Modem Parameter Configuration Reg. 0
    0x99,  // SYMBOL_RATE2        Symbol Rate Configuration Exponent and Mantissa [1..
    0x99,  // SYMBOL_RATE1        Symbol Rate Configuration Mantissa [15:8]
    0x99,  // SYMBOL_RATE0        Symbol Rate Configuration Mantissa [7:0]
    0x3C,  // AGC_REF             AGC Reference Level Configuration
    0xEF,  // AGC_CS_THR          Carrier Sense Threshold Configuration
    0xA9,  // AGC_CFG1            Automatic Gain Control Configuration Reg. 1
    0xC0,  // AGC_CFG0            Automatic Gain Control Configuration Reg. 0
    0x00,  // FIFO_CFG            FIFO Configuration
    0x12,  // FS_CFG              Frequency Synthesizer Configuration
    0x45,  // PKT_CFG1            Packet Configuration Reg. 1
    0x20,  // PKT_CFG0            Packet Configuration Reg. 0
    0x3F,  // PA_CFG2             Power Amplifier Configuration Reg. 2
    0x79,  // PA_CFG0             Power Amplifier Configuration Reg. 0
    0xFF,  // PKT_LEN             Packet Length Configuration
    0x00,  // IF_MIX_CFG          IF Mix Configuration
    0x0A,  // TOC_CFG             Timing Offset Correction Configuration
    0x72,  // FREQ2               Frequency Configuration [23:16]
    0x60,  // FREQ1               Frequency Configuration [15:8]
    0x00,  // FS_DIG1             Frequency Synthesizer Digital Reg. 1
    0x5F,  // FS_DIG0             Frequency Synthesizer Digital Reg. 0
    0x40,  // FS_CAL1             Frequency Synthesizer Calibration Reg. 1
    0x0E,  // FS_CAL0             Frequency Synthesizer Calibration Reg. 0
    0x03,  // FS_DIVTWO           Frequency Synthesizer Divide by 2
    0x33,  // FS_DSM0             FS Digital Synthesizer Module Configuration Reg. 0
    0x17,  // FS_DVC0             Frequency Synthesizer Divider Chain Configuration ..
    0x50,  // FS_PFD              Frequency Synthesizer Phase Frequency Detector Con..
    0x6E,  // FS_PRE              Frequency Synthesizer Prescaler Configuration
    0x14,  // FS_REG_DIV_CML      Frequency Synthesizer Divider Regulator Configurat..
    0xAC,  // FS_SPARE            Frequency Synthesizer Spare
    0xB4,  // FS_VCO0             FS Voltage Controlled Oscillator Configuration Reg..
    0x0E,  // XOSC5               Crystal Oscillator Configuration Reg. 5
    0x03,  // XOSC1               Crystal Oscillator Configuration Reg. 1
};
