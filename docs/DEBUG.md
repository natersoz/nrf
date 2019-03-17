Debugging and Flashing nRF5x
============================

Downloading the JLink Utilities
-------------------------------
The [JLink Software and Documntation Pack](https://www.segger.com/downloads/jlink/#J-LinkSoftwareAndDocumentationPack)
is the place to get Segger debug utilities at the time of this writing.
OSX, Linux, Windows, even ARM Linux versions are there. Quite impressive.
I use the OSX version. The Linux version is nearly identical.

Jlink Commander (on OSX and Linux this is called JLinkExe)
----------------------------------------------------------

  See script nrf/scripts/jlink-nrf

+ OSX:      `/Applications/SEGGER/JLink/JLinkExe`
+ Linux:    `/opt/segger/jlink` (or where ever ...)

JLinkRTTClient
--------------

Note that JLinkRTTClient is a telnet program. Telnet seems to work just as well.

To get the Jlink RTT Client running, JLink Commander `JLinkExe` or
the JLink GDB Server `JLinkGDBServer` needs to be running first.

A script `nrf/scripts/jlink-nrf` provides a short cut for the command line invocation.
After running JLinkExe the JLinkRTTClient can be started and will find the telnet
port to connect. Starting JLinkRTTClient first does not seem to work.

If `JLinkExe` is terminated then `JLinkClient` will need to restart.

JLinkRTTLogger
--------------

`JLinkRTTLogger` works in a similar manner as `JLinkClient`, however
`JLinkRTTLogger` uses the default Segger RTT channel 1 and these projects
use channel 0. `JLinkRTTLogger` seems to reconnect properly with the starting
and stoppping of `JLinkExe`; unlike `JLinkClient`.

jlink_rules.mak
---------------

+ Provides several rules for programming the nRF52 development boards.
    + Only the `nRF52832/PCA10040` is supported right now.
    + It should not be hard to support the `nRF52840/PCA10056`.
+ `make jlink-help` provides rule specific command information.
+ To program the application onto the nRF52 board: `make flash`

    Note: This rule has a dependency on `TARGET_NAME.hex`
    (where `TARGET_NAME` is project name being built.)
    So building this target will update the compile object files,
    elf `.out` and `.hex` files, but not the `.lst` and `.dump.txt` files.
    The makefile rules could use a little tweaking here.
    Use the rule `make all` to insure all of the final targets are up to date.

Note: Attempting to flash the nRF boards while the jlink RTT client is attached
causes the NRF boards to hang. If this happens, drop the RTT client and disconnect
the board USB. Reconnect the USB and start over.

### .segger_sn file
When initiating multipe gdb servers through the use of `JLinkGDBServer` the
target device may be specified on the command line using Segger SWD interface
hardware serial number. To make this process more conenient I incorporate
files `.segger_sn` in the `ble_central` and `ble_peripheral` working
directories. Since these files will be different for each user they are not
checked in. Instead you will find them in the `.gitignore` file for this
project. If the files do not exist then the `jlink_rules.mak` will ignore them
by not including them. If they do exist then the Segger serial number specified
by the `.segger_sn` file will be the target of the debug session.

The SWD interface hardware Serial Number is a 9 digit numeric value usually
prominently displayed on Segger hardware. This value may be obtained from the
JLink Commander `JLinkExe` tool. With only target connected, run `JLinkExe` and
it will connect and display the serial number. The `JLinkExe` command `conf`
will show the serial number again.

Typical .segger_sn contents are:

	# nrf/ble_central/.segger_sn
	# Assign the SEGGER Serial Number used for debugging the ble_central project.
	SEGGER_SN=123456789

The format is Makefile syntax.

More information is available in my github gist:
[JLinkGDBServer Multiple Sessions](https://gist.github.com/natersoz/076cee47d47f87fd67b99c9de61c4d86)

gdb
---

+ `jlink_rules.mak`

  Provides a rules for programming and running the gdb server on the NRF board.
  Again, right now the `nRF52832/PCA10040` is set with symbols in this file as
  is the softdevice `s132`.

  The `jlink-rules.mak` contains gdb specific settings in the `JLINK_GDB_OPTS` symbol.
  Change these as needed.

+ gdb initialization

  The gdb server setting must be matched with the gdb client settings.
  Using a gdb initialization file makes connecting and setting up gdb faster
  and easier. Each project which runs under gdb contains a gdb-init file
  which is checked into git. An example:

        target remote localhost:2331
        file _build/<targetname.out>
        load
        monitor reset

  It should be noted that the ble_central/gdb-init file differs from the others;
  its port assignment is 2431. This matches the GDB_PORT value set in the Makefile
  which is used by jlink-rules.mak. This allows for simultaneous debugging of the
  ble_peripheral and ble_central projects.

  There is a Makefile target `gdb-server` which can be invoked:

		$ make gdb-server

  Which will start the gdb server.
  Once started, the server can be attached to using:

		$ <path>/gcc-arm-none-eabi/bin/arm-none-eabi-gdb --quiet --command=./gdb-init

JLinkRTTClient
--------------

The logger class within the `nrf` projects uses the JLinkRTTClient for outputting
debug, info, warning and error information. There is a Makefile target `rttc`
which should allow you to easily get the RTTC output in a shell:

	$ make rttc


