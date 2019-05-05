###
# File: checker_rules.mak
# @copyright (c) 2018, natersoz. Distributed under the Apache 2.0 license.
#
# Makefile definitions for running clang-tidy
#
# Include a set of C/Assembler file definitions that in the end will define
# the following:
#	INCLUDE_PATHS		C   Language Include paths (without -I prefix)
#	SOURCE_FILES		All Language Source files (with path)
#				C (.c), C++ (.cc)
#
# todo: Improve on this by separating the dependency:
# For now, this file is going to assume that arm_gcc_rules.mak is already
# included in the project Makefile.
#
# The arm_gcc_rules.mak will give us:
# CC_SOURCE = $(filter %.cc, $(SOURCE_FILE_NAMES))
# C_SOURCE  = $(filter %.c,  $(SOURCE_FILE_NAMES))
# S_SOURCE  = $(filter %.s,  $(SOURCE_FILE_NAMES))
#
#
###

# By default do not print out the toolchain details.
VERBOSE		?= @

###
# The check results output directory.
###
CHECK_PATH	= _checks

###
# The checker executable. Must be in the user path.
###
CHECKER		= clang-tidy

###
# Clang-tidy needs to know where to find system header files.
###
COMPILER_SYSTEM_INCLUDE_PATH = \
	$(shell echo "" |c++ -xc - -v -E 2>&1| \
	awk '/include <...> search starts here/{flag=1; next}/End of search list/{flag=0} flag'| \
	sed "s/(framework directory)//p")

CHECKER_INCLUDE_PATH =
CHECKER_INCLUDE_PATH += $(addprefix -isystem , $(COMPILER_SYSTEM_INCLUDE_PATH))

# Do not check Nordic SDK files
CHECKER_FILES = $(filter-out $(SDK_ROOT)%, $(SOURCE_FILES))

CHECK_CC_SOURCE = $(notdir $(filter %.cc, $(CHECKER_FILES)))
CHECK_C_SOURCE  = $(notdir $(filter %.c,  $(CHECKER_FILES)))

CHECK_FILES += $(addprefix $(CHECK_PATH)/, $(CHECK_CC_SOURCE:.cc=.cc.check))
CHECK_FILES += $(addprefix $(CHECK_PATH)/, $(CHECK_C_SOURCE:.c=.c.check))

$(CHECK_FILES): | $(CHECK_PATH)

.PHONY: checks check-info

checks: $(CHECK_FILES)

clean-checks:
	rm -rf $(CHECK_PATH)

## Create checker directories
$(CHECK_PATH):
	$(VERBOSE)$(MKDIR) $@

###
# Setup the checking filters
# See https://clang.llvm.org/extra/clang-tidy/#using-clang-tidy
###
CHECK_FILTERS  =
# CHECK_FILTERS += "-header-filter=.*"
CHECK_FILTERS += "-checks=-clang-analyzer-alpha.*"
#CHECK_FILTERS += "-line-filter=[{'name':'../sdk/components/softdevice/s132/headers/ble_gap.h','lines':[[1,5000]]}]"

# Nordic macros required to integrate with clang-tidy
CHECK_DEFINES =
CHECK_DEFINES += -D SVCALL_AS_NORMAL_FUNCTION
CHECK_DEFINES += -D SWI5_IRQn SWI5_EGU5_IRQn
CHECK_DEFINES += -D __STATIC_INLINE=inline
#CHECK_DEFINES += --enable-targets=arm
#CHECK_DEFINES += --target=thumbv7-eabi

# Check C++ SRC files
$(CHECK_PATH)/%.cc.check: %.cc
	@echo Checking: $< to $@
	$(VERBOSE)$(CHECKER) -quiet $(CHECK_FILTERS) $< -- \
	$(CXXFLAGS) $(CHECK_DEFINES) $(INCLUDE_PATHS) $(CHECKER_INCLUDE_PATH) \
	|tee $@

# Check C SRC files
$(CHECK_PATH)/%.c.check: %.c
	@echo Checking: $< to $@
	$(VERBOSE)$(CHECKER) $< $(CHECK_FILTERS) -- \
	$(CFLAGS) $(CHECK_DEFINES) $(INCLUDE_PATHS) $(CHECKER_INCLUDE_PATH) \
	|tee $@

# Link
# $(CHECK_PATH)/$(TARGET_NAME).check: $(CHECK_FILES) $(LIBS) $(LINKER_SCRIPT)
#	@echo Linking: $@
#	$(VERBOSE)$(CXX) $(ARM_FLAGS) $(LDFLAGS) $(CHECK_FILES) $(LIBS) -T $(LINKER_SCRIPT) -o $@

check-info:
	@echo "COMPILER_SYSTEM_INCLUDE_PATH = $(COMPILER_SYSTEM_INCLUDE_PATH)"
	@echo $(COMPILER_SYSTEM_INCLUDE_PATH)
	@echo "CHECKER_INCLUDE_PATH = $(CHECKER_INCLUDE_PATH)"
#	@echo $(COMPILER_SYSTEM_INCLUDE_PATH)

extra:
	@echo
	@echo "CHECKER_FILES = "
	@echo $(CHECKER_FILES)	| tr ' ' '\n'
	@echo
	@echo "CHECK_CC_SOURCE = "
	@echo $(CHECK_CC_SOURCE)| tr ' ' '\n'
	@echo
	@echo "CHECK_C_SOURCE = "
	@echo $(CHECK_C_SOURCE) | tr ' ' '\n'
	@echo
	@echo "CHECK_FILES = "
	@echo $(CHECK_FILES)	| tr ' ' '\n'
