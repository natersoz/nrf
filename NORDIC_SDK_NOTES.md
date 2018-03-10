SDK directory contents and notes
================================

### sdk/components/boards

	+ pcaXXXXX.h	Nice header which contains the boared mapping to gpio pins.
    + board.c		board LED functions and GPIO pin indexing, cofiguration.
    +

### sdk/components/drivers_nrf/nrf_soc_nosd

	+ nrf_nvic.h
		+ These are really usedful generic ARM functions related to the interrupt controller:
		  enable/disable interrupts, NVIC reset, etc.
		+ Unfortunately, #include "nrf.h" is unnecessary?

### sdk/components/libraries/bsp

	Attempt to avoid using these completely.
	These are application specific events - looks like they are common to ble
	related means for Nordic example apps.

### sdk/components/drivers_ext

	Lots of interesting stuff in here.

	+ TLS: [Transport Layer Security, formerly SSL](https://en.wikipedia.org/wiki/Transport_Layer_Security)
      This could be really useful.

	+ mcp4725 microchip DAC driver.
    + ili9341 IS AN LCD display
    + ds1624  Thermometer

### sdk/components/device

	+ The ARM device (CMSIS) header file which defines all AHB, APB register mappings.
    + Use with the reference manual to really understand the peripheral operation.

### sdk/components/drivers_nrf

	All of the Nordic peripheral drivers as well as things like the timers, wdt, etc.

	+ nrf_drv_gpiote.h, .c		Very nicely done. Clean.
    + nrf_drv_spi.c				The API is very clean. The implementation, not so much.
    + hal						This directory contains a very nice thin layer for accessing
								the CMSIS raw file. Very nice.
	+ common					This appears to be yet another layer on top of hal,
								and I don't like it.

### sdk/components/libraries/util

	Again, this appears to yet another layer above their HAL.
	I like the HAL, I do not like this.

### sdk/components/iot

	First glance: This might be IP over BLE. Looks interesting.

### sdk/components/experimental_802_15_4

	[IEEE 802.15.4](https://en.wikipedia.org/wiki/IEEE_802.15.4)
	[6LoWPAN](https://en.wikipedia.org/wiki/6LoWPAN)

	Should have a look at this.

### sdk/components/libraries/fstorage

	nrf_fstorage_sd.c
		Softdevice based FLASH storage

	nrf_fstorage_nvmc.c
		Simpler, Non-volatile memory controller direct FLASH storage

### sdk/components/libraries/balloc

	A fixed size block allocator.

