Nordic SAADC device driver
==========================

The Nordic Successive Approximation Analog to Digital Converter (SAADC) has been tested on the PCA10040 nrf52 development board. This uses the nRF52832-QFAA device.
Reference the Nordic [nRF52832 Product Specification v1.4](http://infocenter.nordicsemi.com/pdf/nRF52832_PS_v1.4.pdf), sections:

	37: SAADC
	22: PPI
	24: TIMER
	25: RTC

The order listed is the with respect to SAADC implementation relevance.

The SAADC test code can be found in the saadc_test directory.
The test code uses the SAADC driver triggered by the TIMER1 using a TIMER1 comparator register (CC) EVENT through the PPI. The SAADC is triggered by hardware without software intervention when the TIMER1 CC expiration expires. Using hardware triggers through the PPI elimitnates SAADC sample jitter due to ISR latency.

SAADC sample trigger
--------------------

### SAADC sample tigger using PPI

The SAADC requires 2 events to be handled:
+ The initial trigger event. This starts the NRF_SAADC_Type::TASKS_START task.
+ The transition from EVENTS_STARTED -> TASKS_SAMPLE.
  It is strange that Nordic has not hard-wired this hardware functionality into the SAADC module. Requiring software intervention at this point would also introduce sample jitter. The SAADC module coded here uses a PPI channel to get software out of the way and have hardware do the job.

In order to trigger the SAADC using the TIMER1 peripheral a dedicated CC register is required. If the CC register is not dedicated to SAADC TASKS_START, and another client introduces CC values which cause the CC comparator to match, then spurious events will be generated causing the SAADC to trigger when not expected.

To guard against that events triggered by Nordic RTC and TIMER peripherals can be encapsulated in the class timer_observable_generic<> class. See timer_observable_generic.h. The method attach_exclusive() can be used to acquire an unused timer comparator register, if one exists, for use by the PPI to start SAADC data conversions. This observable class can be used for either an RTC instance or TIMER instance.

Triggering the SAADC to take samples using the PPI involves calling the fucntion saadc_conversion_start() with the event_register_pointer parameter set to the event generation register for a specific peripheral.

+ For a TIMER, that register is TIMERx.EVENTS_COMPARE
+ For an RTC,  that register is RTCx.EVENTS_COMPARE

Other events could be used as well; for example a GPIO.

### SAADC sample trigger using software

The SAADC conversions can be started via software.
This is accomplished by calling the function saadc_conversion_start() with the parameter event_register_pointer set to null.

The transition from EVENTS_STARTED to TASKS_SAMPLE is handled by PPI. This is hard-coded in the SAADC device driver. The device driver can easily be modified to perform this function the the SAADC ISR if necessary; i.e. if the PPI resources are required elsewhere.

### SAADC stopping and de-initialization

The function saadc_deinit() releases the SAADC and associated PPI resources. Interrupts are disabled and the SAADC is disabled from taking new samples. Samples in progress will continue to be converted but events will not be generated. This is something of a race condition: will the event callbacks be called prior to the device being de-initialized?

For a cleaner shutdown of the SAADC, consider calling saadc_conversion_stop() and then handling the saadc_event_conversion_stop event. When handling the saadc_event_conversion_stop event call saadc_deinit(). This should provide a clean shutdown of the SAADC device.

### SAADC analog input configuration

The SAADC inputs must be configured prior to performing SAADC conversions. If the input is configured while a conversion is in progress an ASSERT() will fail.

When the SAADC analog inputs are configured the conversion related information can be queried using the saadc_conversion_info() function. This will return the number of channels being converted and the time required in micorseconds to perform the conversion. The conversion times are determined independently of the SAADC sample conversion set up. During development it is useful to make certain that the buffer size allocated for the conversion and time between conversion events are consistent with the SAADC input configuration.

### SAADC limit event generation

The SAADC module can be configured to generate event callbacks based on conversion limits. This is an interesting feature of the Nordic SAADC module. By default no limit based events are generated. The struct and functions

	struct saadc_limits_t

	saadc_enable_limits_event()
	saadc_enable_upper_limit_event()
	saadc_disable_limit_event()

	saadc_get_channel_limits()

can be used to set/get SAADC channel limit events. By default, limit based events are disabled.

### SAADC Error handling

There are not errors returned or exceptions thrown by the SAADC module. All error conditions are handled by ASSERT() failures.

