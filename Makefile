###
# nrf/Makefile
# Top level Makefile to call all of the others.
# Copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
###

# Tell make to use multiple jobs
MAKE_JOBS := -j

sub_make_dirs := ./ble_peripheral ./ble_central ./nordic/tests ./unit_tests

.PHONY: all external relink clean scrub info

all: external
	@for make_dir in $(sub_make_dirs); do	\
		printf "make -C $$make_dir: $@\n";	\
		make -C $$make_dir $@ $(MAKE_JOBS);	\
	done

external:
	make -C external

checks:
	@for make_dir in $(sub_make_dirs); do	\
		printf "make -C $$make_dir: $@\n";	\
		make -C $$make_dir $@ $(MAKE_JOBS);	\
	done
	make -C nordic/tests $@

clean-checks:
	@for make_dir in $(sub_make_dirs); do	\
		printf "make -C $$make_dir: $@\n";	\
		make -C $$make_dir $@ $(MAKE_JOBS);	\
	done
	make -C nordic/tests $@

relink:
	@for make_dir in $(sub_make_dirs); do	\
		printf "make -C $$make_dir: $@\n";	\
		make -C $$make_dir $@ $(MAKE_JOBS);	\
	done
	make -C nordic/tests $@

clean:
	@for make_dir in $(sub_make_dirs); do	\
		printf "make -C $$make_dir: $@\n";	\
		make -C $$make_dir $@;				\
	done
	make -C nordic/tests $@

scrub: clean
	make -C external $@

info:
	@echo "sub_make_dirs         = $(sub_make_dirs)"
	@echo "MAKE_JOBS             = $(MAKE_JOBS)"
