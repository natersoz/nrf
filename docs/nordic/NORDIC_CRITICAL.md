Nordic nRF5x Criticism
======================

## 0. The Nordic BLE SoC series is well thought out.
  Let's start with the best part of Nordic Semicondcutor: Their hardware.
  + They have DMA everywhere. All silicon vendors need to get a clue here:
    Nordic has properly set the bar. Don't even bother adding a SPI peripheral
	with proper DMA. The same can be said for the USART and I2C (TWI in Nordic
	speak). Although not as important for I2C, if you're working in low power
	(keeping the CPU asleep during I2C cycles) then DMA is your friend.
  + Double-buffered peripherals. More Genius!
    How about we not lose that real-time data that is so precious.
	Double buffering relaxes your real-time requirements by a factor of 2.
  + PPI (Programmable Peripheral Interface). This is pure genuis.
    You can read my praise for it here: https://natersoz.github.io/

As time goes on and I get more familiar with their hardware,
I will add more here.

## 1. GATT Clients need prior knownledge of UUIDs.
   GATT Clients can only discover services (and characteristics) by informing
   the softdevice ahead of time by calling the softdevice function
   sd_ble_uuid_vs_add().

   This is by far the worst kink in Nordic's software. What good is service
   discovery when you have to know the services ahead of time?
   In fact, it serves its purppose for many applications. But for a generic
   application which could be used as a competing too to something Cypress
   Semiconductor's [CySmart](http://www.cypress.com/documentation/software-and-drivers/cysmart-bluetooth-le-test-and-debug-tool)
   it is completely usesless.

## 2. Overloading peripherals in the ARM vector table.

The ARM Cortex vector table contains overloaded entries.

There are 240 possible interrupt numbers which can be assigned inthe Cortex-M4
devices. Overloading vector entries with multiple peripherals like this:

    SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQHandler
	SPIM1_SPIS1_TWIM1_TWIS1_SPI1_TWI1_IRQHandler
	SPIM2_SPIS2_SPI2_IRQHandler

Creates a situation that forces the device driver writer use macros to
enable/disable potions of software. Consider the implementation of a SPI driver,
a I2C driver and a USART driver. Within the driver code you cannot place the
interrupt service handler for the IRQ number assigned to any of this within
the source code without guarding the ISR implementation with an `#if defined()`
block.

What are the alternatives?

  + You could define the single instance of the IRQ number ISR handler in a
    single file (aka module, translation unit) link that in, and at run time
	read device registers to decide how the ISR was assigned and then call the
	appropriate device driver ISR dispatch handler within the driver.
	This would be a performance killer, period.

	Also realize that when a function is defined as static within a module the
	compiler is free to inline optimize the separate ISR function with the
	driver ISR handler.

  + You could change the ARM startup code to change the vector table.
    And yes, I may do just that. These peripheral assignments are not part of
	the Nordic softdevice and above IRQ number 40, there are no assignments made.
	This is likely a course I will follow in the future being careful not to
	move entries for which the softdevice has dependencies.

## 3. The SDK is macro hell.
Work with the softdevice directly.

Example: Look the macro defined as `NRF_BLE_GATT_DEF(_name)` found in

    nRF5_SDK_15.2.0_9412b96/components/ble/nrf_ble_gatt/nrf_ble_gatt.h

	#define NRF_BLE_GATT_DEF(_name)                                                                     \
	static nrf_ble_gatt_t _name;                                                                        \
	NRF_SDH_BLE_OBSERVER(_name ## _obs,                                                                 \
						 NRF_BLE_GATT_BLE_OBSERVER_PRIO,                                                \
						 nrf_ble_gatt_on_ble_evt, &_name)

Another macro: `NRF_SDH_BLE_OBSERVER`, found in

	nRF5_SDK_15.2.0_9412b96/components/softdevice/common/nrf_sdh_ble.h

	#define NRF_SDH_BLE_OBSERVER(_name, _prio, _handler, _context)                                      \
	STATIC_ASSERT(NRF_SDH_BLE_ENABLED, "NRF_SDH_BLE_ENABLED not set!");                                 \
	STATIC_ASSERT(_prio < NRF_SDH_BLE_OBSERVER_PRIO_LEVELS, "Priority level unavailable.");             \
	NRF_SECTION_SET_ITEM_REGISTER(sdh_ble_observers, _prio, static nrf_sdh_ble_evt_observer_t _name) =  \
	{                                                                                                   \
		.handler   = _handler,                                                                          \
		.p_context = _context                                                                           \
	}

Hey - what does `_prio` do? Because it never gets used in any of the macros
which get expanded from here. Here is what I think it is intended to do:
I think if you register multiple handlers within the specific section the order
in which they get handled might be determined. It might be a nice thought, but
since the symbol is never used, it does nothing.

Our journey into macro expansion continues as we expand yet another macro:

	#define NRF_SECTION_SET_ITEM_REGISTER(_name, _priority, _var)                                       \
		NRF_SECTION_ITEM_REGISTER(CONCAT_2(_name, _priority), _var)

into one more macro:

	#define NRF_SECTION_ITEM_REGISTER(section_name, section_var) \
		section_var __attribute__ ((section("." STRINGIFY(section_name)))) __attribute__((used))

What on earth does all of this do?
It does this (in gcc-arm terms):

	static nrf_sdh_ble_evt_observer_t sdh_ble_event_observer
	__attribute__((section(".sdh_ble_observers"))) __attribute((used))
	{
		.handler    = nordic_ble_event_handler,
		.p_context  = &ble_observables_instance
	};

Now let's make this a little more readable by employing **judicious** use of a
single macro:

In gcc-arm:

    #define IN_SECTION(section_name) __attribute__((section(section_name))) __attribute((used))

Of course this IN_SECTION() macro can be defined differently when using a
different tool chain to hide the ugliness and make compiler dependencies less
painful. But just this one line.

The code now becomes:

	static nrf_sdh_ble_evt_observer_t sdh_ble_event_observer
		IN_SECTION(".sdh_ble_observers") =
	{
		.handler    = nordic_ble_event_handler,
		.p_context  = &ble_observables_instance
	};

And that is it: Many macros, many files, obfuscated expansions all come down to
that simple implementation.

### Examples of how this section based observer code can be are found here:

[ble/nordic_ble_stack_observer.cc](https://github.com/natersoz/nrf/blob/master/ble/nordic_ble_stack_observer.cc)
This forms the root of Nordic BLE event notifications. This is my version of
something Nordic is also doing in their softdevice.
So yes, I'm wasting some code space linking this in, however, I now know what is
going on. It is an interesting read and revealing for anyone interested in how
the Nordic softdevice deals with event notifications.

Which, by the way, is a cool way to get notifications out of a proprietary
library like softdevice.

[ble/nordic_ble_event_observables.cc](https://github.com/natersoz/nrf/blob/master/ble/nordic_ble_event_observables.cc)
While the code above is the source of all BLE events emitted from the Nordic
softdevice, it is used to encapsulate BLE events which then can be handled
from a different FLASH section allocated for BLE events which contain the full
data structure found in the softdevice headers. That is what this file does;
observes the BLE event types, demultiplexes them and sends them out to C++
interface based observers.

## 4. The SDK is macro hell: `sdk_config.h`
Same headline, different reason.
Let's say you want to use Nordic's SDK library `fds` found in

	nRF5_SDK_15.2.0_9412b96/components/libraries/fds/fds.c

Let's add that C file to the Makefile list of dependencies. Will it compile?
Yes. Will it do anything? No, not unless a macro is defined to enable it.
Remember the good ole days when module compilation/linkage yielded
functionality? Kiss those days goodbye if you're using the SDK. You are going
be unnesting macros all the way into the [insane asylum](https://www.imdb.com/title/tt0073486/videoplayer/vi3943282969).

Or maybe you should just use Nordic's "App Wizard" and not have a clue as to
what is going on.

So how would a modern software implementation avoid *macro hell* and aggregate
interfaces into something meaningful? That is called [dependency injection](https://en.wikipedia.org/wiki/Dependency_injection).


