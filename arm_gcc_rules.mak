###
# File: arm_gcc_rules.mak
#
# Makefile definitions and rules for the ARM GCC toolchain.
# There are some Nordic specifics in here.
#
# Include a set of C/Assembler file definitions that in the end will define
# the following:
#	TARGET_NAME		The build target executable name.
#	LINKER_SCRIPT		The .ld file to use at link time.
#				If not specified then $(TARGET_NAME).ld is used.
#	INCLUDE_PATHS		C   Language Include paths (without -I prefix)
#	SOURCE_FILES		All Language Source files (with path)
#				C (.c), C++ (.cc) and Assember (.s)
#	LD_PATHS		Linker path file include specification;
#				tells the linker where to find libraries
#				if necessary.
###

# By default do not print out the toolchain details.
VERBOSE		?= @

###
# The build type: Debug or Release (case sensetive)
# This will be the artifact (.bin, .hex, .map, .o, etc.) output directory
###
BUILD_TYPE	?= Debug

###
# The build output directory
###
BUILD_PATH	?= _build

###
# If the linker script is not specified then default to TARGET_NAME.ld.
###
LINKER_SCRIPT	?= $(TARGET_NAME).ld

###
# Set up the location of the gcc ARM cross-compiler.
# This will change with the version of compiler being used.
###
GNU_PREFIX	:= arm-none-eabi
GNU_GCC_ROOT	:= /usr/local/gcc-$(GNU_PREFIX)

###
# GCC Toolchain commands
###
CXX		:= "$(GNU_GCC_ROOT)/bin/$(GNU_PREFIX)-g++"
CC		:= "$(GNU_GCC_ROOT)/bin/$(GNU_PREFIX)-gcc"
AS		:= "$(GNU_GCC_ROOT)/bin/$(GNU_PREFIX)-as"
AR		:= "$(GNU_GCC_ROOT)/bin/$(GNU_PREFIX)-ar" -r
LD		:= "$(GNU_GCC_ROOT)/bin/$(GNU_PREFIX)-ld"
NM		:= "$(GNU_GCC_ROOT)/bin/$(GNU_PREFIX)-nm"
OBJDUMP		:= "$(GNU_GCC_ROOT)/bin/$(GNU_PREFIX)-objdump"
OBJCOPY		:= "$(GNU_GCC_ROOT)/bin/$(GNU_PREFIX)-objcopy"
SIZE		:= "$(GNU_GCC_ROOT)/bin/$(GNU_PREFIX)-size"

###
# Other shell commands used
###
MKDIR		:= mkdir -v
RM		:= rm -rf

###
# Function for removing duplicates in a list - taken from Nordic makefile exmaples.
###
remduplicates = $(strip $(if $1,$(firstword $1) $(call remduplicates,$(filter-out $(firstword $1),$1))))

# List of all C/C++ and assembler language file directories.
# This tells the compiler where it can find source files.
V_PATHS	= $(call remduplicates, $(dir $(SOURCE_FILES)))

vpath %.c  $(V_PATHS)
vpath %.cc $(V_PATHS)
vpath %.s  $(V_PATHS)

# The complete list of C Language directories: source files + header files.
# Using remduplicates insures that the directory only gets used once.
# Not that it is necessary but it does clean things up.
I_PATHS += $(call remduplicates, $(INCLUDE_PATHS) $(V_PATHS))

# The include path statement used by the compiler.
INCLUDE_PATHS := $(addprefix -I, $(I_PATHS))

# The linker search paths allows the linker to seach for libraries.
# This is not commonly used. Prefere to specify link libraries with their path.
LINKER_PATHS  := $(addprefix -L, $(LD_PATHS))

# Common Optimization flags, C and C++
ifeq ("$(BUILD_TYPE)", "Debug")
	OPT_FLAGS = -Os
	OPT_FLAGS += "-D NDEBUG"
else
	OPT_FLAGS = -Os
	OPT_FLAGS += -flto
endif

###
# ARM_FLAGS are used by the C, C++, compilers, Linker and assembler.
#
# Common ARM flags specific to Cortex M4.
# Other ARM Cortex-M will vary.
# https://gcc.gnu.org/onlinedocs/gcc/ARM-Options.html
#
# -mthumb           Generate Thumb instructions
# -mabi=aapcs       Use the ARM ABI procedure call specified:
# http://infocenter.arm.com/help/topic/com.arm.doc.ihi0042f/IHI0042F_aapcs.pdf
# -mfloat-abi=hard  uses FPU-specific calling conventions.
# -mfpu=fpv4-sp-d16 VFPv4 instructions, 16 double-precision registers
#
###
ARM_FLAGS += -mthumb
ARM_FLAGS += -mabi=aapcs
ARM_FLAGS += -mcpu=cortex-m4
ARM_FLAGS += -mfloat-abi=hard
ARM_FLAGS += -mfpu=fpv4-sp-d16

###
# ARM GCC (sort of) compiler options.
# Some of these are optimizations,
# TODO document these
#
# -ffunction-sections, -fdata-sections
#  Place each function or data item into its own section.
#  The linker can remove them if they are unused.
###
ARM_GCC_FLAGS += $(ARM_FLAGS)
ARM_GCC_FLAGS += $(OPT_FLAGS)
ARM_GCC_FLAGS += -ffunction-sections
ARM_GCC_FLAGS += -fdata-sections
ARM_GCC_FLAGS += -fno-strict-aliasing
ARM_GCC_FLAGS += -fno-builtin
ARM_GCC_FLAGS += -fshort-enums

###
# Common Warnings
###
WARNING_FLAGS += -Wall
WARNING_FLAGS += -Werror

###
# GNU C Language compiler options
###
CFLAGS += --std=gnu99
CFLAGS += -nostdlib
CFLAGS += $(WARNING_FLAGS)
CFLAGS += $(ARM_GCC_FLAGS)
CFLAGS += -g3

###
# GNU C++ Language compiler options
###
CXXFLAGS += -std=c++14
CXXFLAGS += -nostdlib
CXXFLAGS += $(WARNING_FLAGS)
CXXFLAGS += $(ARM_GCC_FLAGS)
CXXFLAGS += -g3
CXXFLAGS += -fno-rtti
CXXFLAGS += -fno-exceptions

###
# GNU/ARM Assembler flags
# -x assembler-with-cpp option enforces C language pre-processing directives
#    within the .s file.
###
ASFLAGS += $(ARM_FLAGS)
ASFLAGS += -g3
ASFLAGS += -x assembler-with-cpp

###
# Note that the startup file gcc_startup_nrf52.s will accept
# __STACK_SIZE and __HEAP_SIZE as compile time definitions.
#
# If not specified then the hard-coded defaults are used:
# stack size: 8192 bytes
# heap  size:    0 bytes.
###
# ASFLAGS += -D__HEAP_SIZE=0
# ASFLAGS += -D__STACK_SIZE=8192

### Uknown / obsolete options. todo: figure them out or delete them.

###
# Configure the linker
#
# Keep every function in separate section.
# This allows the linker to dump unused functions and data.
# use newlib in nano version
###
LDFLAGS += -Xlinker -Map=$(BUILD_PATH)/$(TARGET_NAME).map
LDFLAGS += $(LINKER_PATHS) -T $(LINKER_SCRIPT)
LDFLAGS += --specs=nano.specs -lc -lnosys
LDFLAGS += $(ARM_FLAGS)
LDFLAGS += $(OPT_FLAGS)
LDFLAGS += -Wl,--gc-sections

###
# Replace source file paths with their build target object files.
# path/filename.c  -> _build/filename.c.o
# path/filename.cc -> _build/filename.cc.o
# path/filename.s  -> _build/filename.s.o
#
# Note: this naming convention allows for files to share filename with a
# different suffix and both will get compiled and linked.
###
SOURCE_FILE_NAMES = $(notdir $(SOURCE_FILES))

CC_SOURCE = $(filter %.cc, $(SOURCE_FILE_NAMES))
C_SOURCE  = $(filter %.c,  $(SOURCE_FILE_NAMES))
S_SOURCE  = $(filter %.s,  $(SOURCE_FILE_NAMES))

OBJECT_FILES += $(addprefix $(BUILD_PATH)/, $(CC_SOURCE:.cc=.cc.o))
OBJECT_FILES += $(addprefix $(BUILD_PATH)/, $(C_SOURCE:.c=.c.o))
OBJECT_FILES += $(addprefix $(BUILD_PATH)/, $(S_SOURCE:.s=.s.o))

DEPEND_FILES  = $(OBJECT_FILES:.o=.dep)

$(OBJECT_FILES): | $(BUILD_PATH)

## Create build directories
$(BUILD_PATH):
	$(VERBOSE)$(MKDIR) $@

# Compile C++ SRC files
$(BUILD_PATH)/%.cc.o: %.cc
	@echo Compiling file: $<
	$(VERBOSE)$(CXX) $(CXXFLAGS) $(INCLUDE_PATHS) -c -o $@ $<
	$(VERBOSE)$(CXX) $(CXXFLAGS) $(INCLUDE_PATHS) -c -MM -MT $@ -MF $(@:.o=.dep) $<

# Compile C SRC files
$(BUILD_PATH)/%.c.o: %.c
	@echo Compiling file: $<
	$(VERBOSE)$(CC) $(CFLAGS) $(INCLUDE_PATHS) -c -o $@ $<
	$(VERBOSE)$(CC) $(CFLAGS) $(INCLUDE_PATHS) -c -MM -MT $@ -MF $(@:.o=.dep) $<

# Compile Assembly files
$(BUILD_PATH)/%.s.o: %.s
	@echo Assembling file: $<
	$(VERBOSE)$(CC) $(ASFLAGS) $(INCLUDE_PATHS) -c -o $@ $<

# Link
$(BUILD_PATH)/$(TARGET_NAME).out: $(OBJECT_FILES)
	@echo Linking target: $@
	$(VERBOSE)$(CC) $(LDFLAGS) $(OBJECT_FILES) $(LIBS) -o $@
	$(VERBOSE)$(SIZE) $@

## Create binary .bin file from the .out file
$(BUILD_PATH)/%.bin: $(BUILD_PATH)/%.out
	@echo Creating: $@
	$(VERBOSE)$(OBJCOPY) -O binary $< $@

## Create binary .hex file from the .out file
$(BUILD_PATH)/%.hex: $(BUILD_PATH)/%.out
	@echo Creating: $@
	$(VERBOSE)$(OBJCOPY) -O ihex $< $@

## Create .lst file from the .out file
$(BUILD_PATH)/%.lst: $(BUILD_PATH)/%.out
	@echo Creating: $@
	$(VERBOSE)$(OBJDUMP) -S $< >$@

## Create .dump.txt file from the .out file
## This is a better map file than the normal linker generated map.
$(BUILD_PATH)/%.dump.txt: $(BUILD_PATH)/%.out
	@echo Creating: $@
	$(VERBOSE)$(OBJDUMP) -tC $<  >$@
	$(VERBOSE)echo "Sizes:"     >>$@
	$(VERBOSE)$(SIZE)        $< >>$@

.PHONY:  echosize arm_gcc_info

echosize: $(BUILD_PATH)/$(TARGET_NAME).out
	-@echo ""
	$(VERBOSE)$(SIZE) $<
	-@echo ""

arm-gcc-info:
	@echo
	@echo "INCLUDE_PATHS = "
	@echo $(INCLUDE_PATHS)	| tr ' ' '\n\t'
	@echo
	@echo "V_PATHS = "
	@echo $(V_PATHS)	| tr ' ' '\n\t'
	@echo
	@echo "INCLUDE_PATHS = "
	@echo $(INCLUDE_PATHS)	| tr ' ' '\n\t'
	@echo
#	@echo "DEPEND_FILES = "
#	@echo $(DEPEND_FILES)	| tr ' ' '\n\t'
#	@echo
	@echo "SOURCE_FILES = "
	@echo $(SOURCE_FILES)	| tr ' ' '\n\t'
	@echo
	@echo "LDFLAGS = "
	@echo $(LDFLAGS)	| tr ' ' '\n\t'
	@echo
	@echo "LINKER_SCRIPT = $(LINKER_SCRIPT)"
	@echo
	@echo  "CXX     = $(CXX)"
	@echo  "CC      = $(CC)"
	@echo  "AS      = $(AS)"
	@echo  "AR      = $(AR)"
	@echo  "LD      = $(LD)"
	@echo  "NM      = $(NM)"
	@echo  "OBJDUMP = $(OBJDUMP)"
	@echo  "OBJCOPY = $(OBJCOPY)"
	@echo  "SIZE    = $(SIZE)"


-include $(DEPEND_FILES)

