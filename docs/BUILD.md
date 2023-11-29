Building the projects
=====================

[Arm GNU Toolchain](https://developer.arm.com/Tools%20and%20Software/GNU%20Toolchain)
-------------------
+ Adjust the nrf/make_fules/arm_gcc_rules.mak file by setting the
`GNU_GCC_ROOT` symbol.

		I'm using [Version 13.2.Rel1](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads) referenced via a symlink:
		/opt/gcc-arm-none-eabi-13.2.Rel1
		A symlink /opt/gcc-arm-none-eabi -> gcc-arm-none-eabi-13.2.Rel1

		Using this installation requires no modification to the make rules.

+ An alias for `gdb` is helpful. Something like:

  `alias gdb-arm='/opt/gcc-arm-none-eabi/arm-none-eabi/bin/arm-none-eabi-gdb --quiet'`

[Boost Libraries](https://www.boost.org/)
-----------------
If you do not like C++, well, you better look elsewhere.
Boost libraries will used as they make sense.
The boost::intrusive is certainly going to be used.
This is an excellent library for embedded work.

Boost 1.70.0 is being used at this time.

		I'm using boost 1.70.0 installed in /opt/boost_1_70_0
		with symlink /opt/boost -> boost_1_70_0

When boost is built and installed its directory structure differs from when it
is merely copied. At this time the 'headers only' boost installation is required.
However, when boost is [built and then installed](https://www.boost.org/doc/libs/1_69_0/more/getting_started/unix-variants.html)
it creates a different directory structure than when merely copied.

Built and Installed Headers location:

	/opt/boost/include/boost/...

Copied Headers location:

	/opt/boost/boost/...

The Makefiles set the symbols `BOOST_ROOT` and `INCLUDE_PATHS` to work within
the **Built and Installed** Boost headers path.

To use the copied version the simplest method for resolving the seach paths is
to set a symbolic link within the Boost copied directory:

	cd /opt/boost
	ln -s . include

### Quick instructions for building boost

Detar or Unzip [boost](https://www.boost.org/users/history/version_1_70_0.html)
into a build directory.

	cd <build_directory>
	./bootstrap.sh bootstrap.sh --prefix=/opt/boost_1.70.0
	./b2
	sudo ./b2 install
	cd /opt
	sudo ln -s boost_1.70.0 boost


[Nordic SDK](https://www.nordicsemi.com/Software-and-Tools/Software/nRF5-SDK)
------------
The Nordic SDK v15.2 will be installed automatically when 'make' is invoked.
The installation director is:

    nrf/external/nRF5_SDK_15.2.0_9412b96

The symbolic link `nrf/sdk` points to this location.

+ A few SDK files need replaced or moved out of the way.
  If `make` execution is successful this also should be done automatically.
  The file replacement scipt can be found in:

		nrf/nordic/sdk-modified/replace_sdk_files

[Google Test](https://github.com/google/googletest)
-------------
GoogleTest version 1.8.1 is downloaded into

	nrf/external/googletest

It is used by the nrf/unit_test modules.

Building
--------
From the top directory `nrf`:

	$ make

It should compile and link all projects without errors or warnings.

Notes:
+ The boost libraries are required to be pre-installed as described above.
+ The ARM cross-compiler must also be installed as described above.
+ GoogleTest and the Nordic SDK will be installed as part of `make` if
  they are not found under the external top level directory.
  A symlink to your own existing installation can be used to avoid
  duplication of files. If you're using your own installation see the
  notes above regarding files which need to be replaced.

clang-tidy checks
-----------------

From the top directory `nrf` (or any of the project files):

	$ make checks

This will run the checker `clang-tidy` using the project Makefiles.
The clang-tidy rules, definitions and targets are located in
`make_rules/checker_rules.mak`. Errors and any unsupressed warnings are
output into the `_checks` directory inside each project.

To find any errors or unsupressed warnings found during a check:

	$ find . -type f -iname '*.check' -not -size 0c -exec cat {} \;

This will find any clang-tidy unsupressed warnings and errors from the
checker runs by look for non-zero length output files.

What about CMake?
-----------------
CMake seems to be a wonderful work in progress.
I think I'll wait until they release their Beta.

In the mean time, here is some stuff to ponder:
+ [Der Uber CMakenfuhrer](https://www.youtube.com/watch?v=rLopVhns4Zs)
+ [Get your hands off CMAKE_CXX_FLAGS](https://na01.safelinks.protection.outlook.com/?url=https%3A%2F%2Fgist.github.com%2Fmbinna%2Fc61dbb39bca0e4fb7d1f73b0d66a4fd1%23get-your-hands-off-cmake_cxx_flags&data=02%7C01%7Cnersoz%40impinj.com%7C7546f96eda4540c0c58208d6879d1766%7C6de70f0f73574529a415d8cbb7e93e5e%7C0%7C0%7C636845506107530417&sdata=lZAPAxWLj%2Fj4Fa9E%2F4sxKfKMa4PjITetDxhM3CScLww%3D&reserved=0)
+ [Use CMake generators instead](https://stackoverflow.com/questions/38578801/target-compile-options-for-only-c-files)
+ [LVM CMake Primer](https://llvm.org/docs/CMakePrimer.html)
