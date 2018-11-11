Notes on BLE Services
=====================

Custom Services within this repository of BLE classes will use the base UUID:
0000-0000-BB9B-494C-86C6-052628E7D83F

Reminder: UUIDs are big-endian. They are reversed to little endian when used
with the Noridc softdevice.

Coding the base UUID:

	static constexpr boost::uuids::uuid const base
	{{
		0x00, 0x00,		// Increment for each service.
		0x00, 0x00,		// Increment for characteristics within services.
		0xBB, 0x9B,		// Fixed ...
		0x49, 0x4C,
		0x86, 0xC6,
		0x05, 0x26, 0x28, 0xE7, 0xD8, 0x3F
	}};


UUID Service Assignments
------------------------
0x0001	Sensor Service


UUID Characteristic Assignments
-------------------------------
0x0001	ADC samples characteristic
0x0002	ADC scaling characteristic
0x0003	ADC enable  characteristic

