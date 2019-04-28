###
# nrf/Makefile
# Top level Makefile to call all of the others.
# Copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
###

# Tell make to use multiple jobs
MAKE_JOBS := -j

sub_make_files	= $(shell find . -maxdepth 2 -name Makefile -not \( -wholename ./Makefile -or -wholename ./external/Makefile \))
sub_make_dirs	= $(patsubst %/Makefile,%,$(sub_make_files))

.PHONY:  all relink clean mrproper info

all:
	make -C external
	@for make_dir in $(sub_make_dirs); do		\
		printf "make -C $$make_dir: $@\n";	\
		make -C $$make_dir $@ $(MAKE_JOBS);	\
	done
	make -C nordic/tests $@

relink:
	@for make_dir in $(sub_make_dirs); do		\
		printf "make -C $$make_dir: $@\n";	\
		make -C $$make_dir $@ $(MAKE_JOBS);	\
	done
	make -C nordic/tests $@

clean:
	@for make_dir in $(sub_make_dirs); do		\
		printf "make -C $$make_dir: $@\n";	\
		make -C $$make_dir $@;			\
	done
	make -C nordic/tests $@

mrproper:
	make -C external $@

info:
	@printf "sub_make_files        = $(sub_make_files)\n"
	@printf "sub_make_dirs         = $(sub_make_dirs)\n"
	@printf "NUMBER_OF_PROCESSORS  = $(NUMBER_OF_PROCESSORS)\n"
	@printf "MAKE_JOBS             = $(MAKE_JOBS)\n"

