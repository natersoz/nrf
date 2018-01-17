Nordic nRF related notes
========================

Development Boards:
-------------------
+ nRF52 Development board PCA10040
  nRF52832 QFAAB0 1606AA
  Softdevice: S132

+ nRf52 Development board PCA10056
  nRF52840


Compiler Options:
-----------------
-std=c99
-MP
-MD
-c
-o
_build/nrf52832_xxaa/nrf_log_frontend.c.o
../../../../../../components/libraries/experimental_log/src/nrf_log_frontend.c
-O3
-g3
-DBOARD_PCA10040
-DCONFIG_GPIO_AS_PINRESET
-DFLOAT_ABI_HARD
-DNRF52
-DNRF52832_XXAA
-DNRF52_PAN_74
-DSWI_DISABLE0
-mcpu=cortex-m4
-mthumb
-mabi=aapcs
-Wall
-Werror
-mfloat-abi=hard
-mfpu=fpv4-sp-d16
-ffunction-sections
-fdata-sections
-fno-strict-aliasing
-fno-builtin
-fshort-enums

-I../../..
-I../../../../../../components
-I../../../../../../components/boards
-I../../../../../../components/device
-I../../../../../../components/drivers_nrf/common
-I../../../../../../components/drivers_nrf/delay
-I../../../../../../components/drivers_nrf/gpiote
-I../../../../../../components/drivers_nrf/hal
-I../../../../../../components/drivers_nrf/nrf_soc_nosd
-I../../../../../../components/drivers_nrf/spi_master
-I../../../../../../components/drivers_nrf/uart
-I../../../../../../components/libraries/atomic
-I../../../../../../components/libraries/balloc
-I../../../../../../components/libraries/bsp
-I../../../../../../components/libraries/button
-I../../../../../../components/libraries/experimental_log
-I../../../../../../components/libraries/experimental_log/src
-I../../../../../../components/libraries/experimental_memobj
-I../../../../../../components/libraries/experimental_section_vars
-I../../../../../../components/libraries/scheduler
-I../../../../../../components/libraries/strerror
-I../../../../../../components/libraries/timer
-I../../../../../../components/libraries/util
-I../../../../../../components/toolchain
-I../../../../../../components/toolchain/cmsis/include
-I../../../../../../components/toolchain/gcc
-I../../../../../../external/fprintf
-I../../../../../../external/segger_rtt
-I../config

