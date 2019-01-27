###
# nrf/unit_tests/Makefile
# Makefile for compiling google test based unit tests.
# Modified from googletest 1.8.1 googletest/make/Makefile
###

VERBOSE	?= @
OBJ_PATH	= _build

# Points to the root of Google Test.
BOOST_ROOT	= /opt/boost
GTEST_DIR	= ../external/googletest/googletest

INCLUDE_PATH     = -I .
INCLUDE_PATH    += -I ..
INCLUDE_PATH    += -I ../utilities
INCLUDE_PATH    += -I ../logger
INCLUDE_PATH    += -I ../nordic/peripherals
INCLUDE_PATH    += -I $(BOOST_ROOT)/include
INCLUDE_PATH    += -I $(GTEST_DIR)/include/gtest
INCLUDE_PATH    += -I $(GTEST_DIR)/include/gtest/internal

vpath %.cc .
vpath %.cc ../utilities
vpath %.cc ../logger
vpath %.cc ../ble
vpath %.cc ../ble/service
vpath %.cc $(GTEST_DIR)/src/
vpath %.c  $(GTEST_DIR)/src/

WARNINGS += -Wall
WARNINGS += -Wmissing-field-initializers
WARNINGS += -Wpointer-arith
WARNINGS += -Wuninitialized
WARNINGS += -Winit-self
WARNINGS += -Wstrict-overflow
# WARNINGS += -Wextra
# WARNINGS += -Wundef
# WARNINGS += -Wshadow

CXXFLAGS  = $(WARNINGS) $(DEFINES) -std=c++17 -g -O0 -pthread
CFLAGS    = $(WARNINGS) $(DEFINES) -std=c99   -g -O0 -pthread

# Flags passed to the preprocessor.
CPPFLAGS += -isystem $(GTEST_DIR)/include

# All tests produced by this Makefile.  Remember to add new tests you
# created to the list.
TARGET_NAME = all_tests

SRC =
SRC += battery_service.cc
SRC += gatt_attribute.cc
SRC += gatt_characteristic.cc
SRC += gatt_declaration.cc
SRC += gatt_service.cc
SRC += gatt_service_container.cc
SRC += gatt_service_discovery_iterator.cc
SRC += gatt_write_ostream.cc

SRC += uuid.cc
SRC += gregorian.cc
SRC += format_conversion.cc
SRC += int_to_string.cc
SRC += logger.cc
SRC += vwritef.cc
SRC += write_data.cc

SRC += test_bit_manip.cc
SRC += test_fixed_allocator.cc
SRC += test_format_conversion.cc
SRC += test_gregorian.cc
SRC += test_int_to_string.cc
SRC += test_make_array.cc
SRC += test_observer.cc
SRC += test_uuid.cc

SRC += test_ble_service.cc
SRC += test_ble_service_container.cc
SRC += gatt_write_ostream.cc
SRC += gatt_enum_types_strings.cc

# Helpers, stubs, fakes.
SRC += test_assert.cc
SRC += rtc_stubs.cc

OBJ_CXX	= $(SRC:.cc=.o)
OBJ_C	= $(OBJ_CXX:.c=.o)
OBJ_O	= $(OBJ_C:%.o=$(OBJ_PATH)/%.o)
DEPS	= $(OBJ_C:%.o=$(OBJ_PATH)/%.dep)

all: $(OBJ_PATH) $(OBJ_PATH)/$(TARGET_NAME)
	$(OBJ_PATH)/$(TARGET_NAME)

clean :
	rm -rf $(OBJ_PATH)

###
# Googletest rules
###
$(OBJ_PATH)/gtest-all.o : gtest-all.cc
	@echo "Compiling $@"
	$(VERBOSE) $(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c $< -o $@

$(OBJ_PATH)/gtest_main.o : gtest_main.cc
	@echo "Compiling $@"
	$(VERBOSE) $(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c $< -o $@

$(OBJ_PATH)/gtest.a : $(OBJ_PATH)/gtest-all.o
	@echo "Archiving $@"
	$(VERBOSE) $(AR) $(ARFLAGS) $@ $^

$(OBJ_PATH)/gtest_main.a : $(OBJ_PATH)/gtest-all.o $(OBJ_PATH)/gtest_main.o
	@echo "Archiving $@"
	$(VERBOSE) $(AR) $(ARFLAGS) $@ $^

$(OBJ_PATH)/$(TARGET_NAME): $(OBJ_O) $(OBJ_PATH)/gtest_main.a
	@echo "Linking $@"
	$(VERBOSE) $(CXX) $(CXXFLAGS)  $^ -o $@

info:
	@echo "DEFINES          = '$(DEFINES)'"
	@echo "SRC              = '$(SRC)'"
	@echo "DEPS             = '$(DEPS)'"
	@echo "INCLUDE_PATH     = '$(INCLUDE_PATH)'"
	@echo "CPPFLAGS         = '$(CPPFLAGS)'"
	@echo "GTEST_DIR        = '$(GTEST_DIR)'"


###
# Implicit Rules
###
$(OBJ_PATH):
	mkdir $@

$(OBJ_PATH)/%.o : %.cc
	@echo "Compiling $@"
	$(VERBOSE) $(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $(INCLUDE_PATH) $< -o $@
	$(VERBOSE) $(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $(INCLUDE_PATH) -MM -MT $@ -MF $(@:.o=.dep) $<

$(OBJ_PATH)/%.o : %.c
	@echo "Compiling $@"
	$(VERBOSE) $(CC) -c $(CFLAGS) $(INCLUDE_PATH) $< -o $@
	$(VERBOSE) $(CC) -c $(CFLAGS) $(INCLUDE_PATH) -MM -MT $@ -MF $(@:.o=.dep) $<


-include $(DEPS)

