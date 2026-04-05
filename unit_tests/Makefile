###
# nrf/ble_peripheral/Makefile
#
# Copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
###

# Find all the Makefiles in subdirectories to build:
sub_make_files	= $(shell find . -maxdepth 3 -name Makefile -not \( -wholename \*/_build/\* -or -wholename ./Makefile -or -wholename ./external/Makefile \))
sub_make_dirs	= $(patsubst %/Makefile,%,$(sub_make_files))

PROJECT_ROOT		?= $(shell git rev-parse --show-toplevel)
BUILD_PATH			?= _build

.PHONY: all clean

all:
	cmake -S . -B $(BUILD_PATH)
	cmake --build $(BUILD_PATH)
	@for make_dir in $(sub_make_dirs); do	\
		printf "make -C $$make_dir: $@\n";	\
		make -C $$make_dir $@ $(MAKE_JOBS);	\
	done

run: all
	ctest --test-dir $(BUILD_PATH)
	@for make_dir in $(sub_make_dirs); do	\
		printf "make -C $$make_dir: $@\n";	\
		make -C $$make_dir $@ $(MAKE_JOBS);	\
	done

clean:
	rm -rf $(BUILD_PATH)
	@for make_dir in $(sub_make_dirs); do	\
		printf "make -C $$make_dir: $@\n";	\
		make -C $$make_dir $@ $(MAKE_JOBS);	\
	done

relink:
	@echo "relink ignored"

info:
	@echo "PROJECT_ROOT          : $(PROJECT_ROOT)"
	@echo "BUILD_PATH            : $(BUILD_PATH)"
	@echo "sub_make_files        : $(sub_make_files)"
	@echo "sub_make_dirs         : $(sub_make_dirs)"
