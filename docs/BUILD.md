Building the projects
=====================

[Arm GNU Toolchain](https://developer.arm.com/Tools%20and%20Software/GNU%20Toolchain)
-------------------
The ARM cross compiler is defined in the file Toolchain-gcc-arm-none-eabi.cmake.
You may need to tweak the path to point to where your ARM GCC cross-compiler is located:

	```
	set(ARM_GCC_PATH /opt/gcc-arm-none-eabi)
	```

+ An alias for `gdb` is helpful. Something like:

  `alias gdb-arm='/opt/gcc-arm-none-eabi/arm-none-eabi/bin/arm-none-eabi-gdb --quiet'`

Building
--------
The easiest way to build all projects is to run `make` at the top level.
This will iterate through all sub-make files, invoking make in each subdirectory.
`Makefile`s wrap calls to cmake within each subdirectory.

There is a top level CMakeLists.txt, but that is not very useful since it places
all executables in the top level `_build` directory. Since each elf executable needs
to be loaded onto the nRF52 device, it is more useful to have each executable
built in its respective project directory.

There is a `external_content.cmake` file that can be used with CMake.
However, CMake will download these external libraries each time the
built target files are removed. This leads to far more downloads than necessary.

Therefore, using the `external/Makefile` provides more control than the CMake method.
Do it once, and only re-download when necessary. Downloading the external libraries
is part of the top level Makefile execution.

### Build ARM targets:
From the top directory `nrf`:
	```bash
	cmake -S . -B ./_build --toolchain Toolchain-gcc-arm-none-eabi.cmake
	cmake --build ./_build
	```

All projects should compile and link without errors or warnings.
Each project directory contains a `Makefile` is included that wraps calls to cmake.

Each ARM executable project can be built from within its subdirectory as a stand-alone project.

### Build unit tests:
From within the `unit_tests` directory:
	```bash
	make run
	```
Will build and the unit tests.

External Libraries
------------------
### [Nordic SDK](https://www.nordicsemi.com/Products/Development-software/nRF5-SDK/Download#infotabs)

The Nordic SDK v17.1.0 will be installed when `make` is invoked in the `external` directory.
The installation directory is:

    nrf/external/nRF5_SDK_17.1.0_ddde560

The symbolic link `nrf/sdk` points to this location.

+ A few SDK files need replaced or moved out of the way.
  If `make` execution is successful this also should be done automatically.
  The file replacement script can be found in:

		nrf/nordic/sdk-modified/replace_sdk_files

### [Boost](https://www.boost.org/releases/latest/)

The Boost libraries are used in header only form.
The Boost libraries are installed when `make` is invoked in the `external` directory.
