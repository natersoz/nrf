###
# nrf/Makefile
# Top level Makefile to call all of the others.
# Copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
###

OS_NAME=$(shell uname)

ifeq ($(OS_NAME), Darwin)
	NUMBER_OF_PROCESSORS = $(shell sysctl -n hw.ncpu)
endif

ifeq ($(OS_NAME), Linux)
	NUMBER_OF_PROCESSORS := $(shell awk '/^processor/ {++n} END {print n}' /proc/cpuinfo)
endif

ifdef NUMBER_OF_PROCESSORS
	MAKE_JOBS := -j $(NUMBER_OF_PROCESSORS)
else
	MAKE_JOBS :=
endif

sub_make_files	= $(shell find . -maxdepth 2 -name Makefile -not -wholename ./Makefile)
sub_make_dirs	= $(patsubst %/Makefile,%,$(sub_make_files))

.PHONY:  all relink clean info

all:
	@for make_dir in $(sub_make_dirs); do		\
		printf "make -C $$make_dir: $@\n";	\
		make -C $$make_dir $@ $(MAKE_JOBS);	\
	done

relink:
	@for make_dir in $(sub_make_dirs); do		\
		printf "make -C $$make_dir: $@\n";	\
		make -C $$make_dir $@ $(MAKE_JOBS);	\
	done

clean:
	@for make_dir in $(sub_make_dirs); do		\
		printf "make -C $$make_dir: $@\n";	\
		make -C $$make_dir $@;			\
	done

info:
	@printf "sub_make_files        = $(sub_make_files)\n"
	@printf "sub_make_dirs         = $(sub_make_dirs)\n"
	@printf "NUMBER_OF_PROCESSORS  = $(NUMBER_OF_PROCESSORS)\n"
	@printf "MAKE_JOBS             = $(MAKE_JOBS)\n"

