###
# nrf/unit_tests/test_int_to_string.mak
###

VERBOSE		?= @

TARGET_NAME	= test_int_to_string

BUILD_PATH	= _build

INCLUDE_PATH	+= -I .
INCLUDE_PATH	+= -I ../utilities

vpath %.cc .
vpath %.cc ../utilities

WARNINGS += -Wall
WARNINGS += -Wmissing-field-initializers
WARNINGS += -Wpointer-arith
WARNINGS += -Wuninitialized
WARNINGS += -Winit-self
WARNINGS += -Wstrict-overflow
WARNINGS += -Wundef
#WARNINGS += -Wlogical-op
WARNINGS += -Wshadow
WARNINGS += -Wno-c++1z-extensions

CXXFLAGS  = -g $(WARNINGS) $(DEFINES) -std=c++14
CFLAGS    = -g $(WARNINGS) $(DEFINES) -std=c99

SRC += test_int_to_string.cc
SRC += int_to_string.cc

OBJ_CXX	= $(SRC:.cc=.o)
OBJ_C	= $(OBJ_CXX:.c=.o)
OBJ_O	= $(OBJ_C:%.o=$(BUILD_PATH)/%.o)
DEPS	= $(OBJ_C:%.o=$(BUILD_PATH)/%.dep)

all: $(BUILD_PATH) $(BUILD_PATH)/$(TARGET_NAME)
	$(BUILD_PATH)/$(TARGET_NAME)

clean:
	rm -rf $(BUILD_PATH)

relink:
	rm -f $(BUILD_PATH)/$(TARGET_NAME)
	make all

info:
	@echo "DEFINES           = '$(DEFINES)'"
	@echo "SRC               = '$(SRC)'"
	@echo "DEPS              = '$(DEPS)'"
	@echo "INCLUDE_PATH      = '$(INCLUDE_PATH)'"

$(BUILD_PATH)/$(TARGET_NAME): $(OBJ_O)
	@echo "Linking $@"
	$(VERBOSE) g++ $(CXXFLAGS)  $^ -o $@

$(BUILD_PATH):
	mkdir $(BUILD_PATH)

###
# Implicit Rules
###
$(BUILD_PATH)/%.o : %.cc
	@echo "Compiling $@"
	$(VERBOSE) $(CXX) -c $(CXXFLAGS) $(INCLUDE_PATH) $< -o $@
	$(VERBOSE) $(CXX) -c $(CXXFLAGS) $(INCLUDE_PATH) -MM -MT $@ -MF $(@:.o=.dep) $<

$(BUILD_PATH)/%.o : %.c
	@echo "Compiling $@"
	$(VERBOSE) $(CC) -c $(CFLAGS) $(INCLUDE_PATH) $< -o $@
	$(VERBOSE) $(CC) -c $(CFLAGS) $(INCLUDE_PATH) -MM -MT $@ -MF $(@:.o=.dep) $<


-include $(DEPS)
