Debugging and Flashing nRF5x
============================

### Jlink Commander (on OSX JLinkExe)

  See script nrf/scripts/jlink-nrf

+ OSX:      `/Applications/SEGGER/JLink/JLinkExe`
+ Linux:    `/opt/JLink` (or where ever ...)

### JLinkRTTClient
Note that JLinkRTTClient is a telnet program. Telnet seems to work just as well.

To get the Jlink RTT Client running, JLink Commander (JLinkExe) or
the JLink GDB Server (JLinkGDBServer) needs to be running first.

A script `nrf/scripts/jlink-nrf` provides a short cut for the command line invocation.
After running JLinkExe the JLinkRTTClient can be started and will find the telnet
port to connect. Starting JLinkRTTClient first does not seem to work.

If JLinkExe is terminated then JLinkClient will need to restart.

### JLinkRTTLogger
JLinkRTTLogger works in a similar manner as JLinkClient, however the default channel
of '1' is incorrect as configured in these projects. These projects use channel 0.
JLinkRTTLogger seems to reconnect properly with the starting and stoppping of
JLinkExe; unlike JLinkClient.

### jlink_rules.mak
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

### gdb
+ `jlink_rules.mak`

  Provides a rules for programming and running the gdb server on the NRF board.
  Again, right now the `nRF52832/PCA10040` is set with symbols in this file as
  is the softdevice `s132`.

  The `jlink-rules.mak` contains gdb specific settings in the `JLINK_GDB_OPTS` symbol.
  Change these as needed.

+ gdb initialization
  The gdb server setting must be matched with the gdb client settings.
  Using a gdb initialization file makes connecting and setting up gdb faster
  and easier. Example:

        target remote localhost:2331
        file _build/<targetname.out>
        load
        monitor reset

  Obviously the port must match the port defined in jlink-rules.mak

        gdb-arm --quiet --command=./gdb-init

### Multiple GDB and JLinkRTTClient debug sessions.
The projects ble_peripheral and ble_central are set up to be debugged
simultaneously. Different ports are set within each project Makefile.
The correct serial numbers will need to be set as the variable SEGGER_SN.

See my githug gist [JLinkGDBServer Multiple Sessions](https://gist.github.com/natersoz/076cee47d47f87fd67b99c9de61c4d86)
for more details.
