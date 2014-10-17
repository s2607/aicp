# The name of your sketch (used to find TARGET.ino and to name the compiled .hex file)
TARGET := shell

# Libraries
USER_LIBRARIES := Servo Serial
CORE_LIBRARIES :=  Servo 
USER_LIB_HOME := ../libraries

# Arduino IDE installation location
ARDUINO_HOME := /Applications/Arduino.app/Contents/Resources/Java

# Configurable options
OPTIONS = -DF_CPU=48000000 -DUSB_SERIAL -DLAYOUT_US_ENGLISH

# Options needed by many Arduino libraries to configure for Teensy 3.1
OPTIONS += -D__MK20DX256__ -DARDUINO=105 -DTEENSYDUINO=118

#************************************************************************
# Location of Teensyduino utilities, Toolchain, and Arduino Libraries.
# To use this makefile without Arduino, copy the resources from these
# locations and edit the pathnames.  The rest of Arduino is not needed.
#************************************************************************

# Arduino/Teensy files, Core libs
ARDUINO_HW_HOME := $(ARDUINO_HOME)/hardware
CORE_LIB_HOME := $(ARDUINO_HOME)/libraries
TEENSY_HOME := $(ARDUINO_HW_HOME)/teensy/cores/teensy3

# path location for Teensy Loader, teensy_post_compile and teensy_reboot
TOOLSPATH := $(ARDUINO_HW_HOME)/tools

# path location for the arm-none-eabi compiler
COMPILERPATH := $(ARDUINO_HW_HOME)/tools/arm-none-eabi/bin

# where to store build output files
OBJ_DIR := obj

#************************************************************************
# Library includes and sources
#************************************************************************

LIBRARY_PATHS := $(foreach lib,$(CORE_LIBRARIES),$(CORE_LIB_HOME)/$(lib)) $(foreach lib,$(USER_LIBRARIES),$(USER_LIB_HOME)/$(lib))
LIBRARY_INCLUDES := $(foreach libdir,$(LIBRARY_PATHS),-I$(libdir))
LIBRARY_C_FILES := $(foreach libdir,$(LIBRARY_PATHS),$(wildcard $(libdir)/*.c))
LIBRARY_CPP_FILES := $(foreach libdir,$(LIBRARY_PATHS),$(wildcard $(libdir)/*.cpp))

VPATH := . $(LIBRARY_PATHS) $(TEENSY_HOME)

#************************************************************************
# Settings below this point usually do not need to be edited
#************************************************************************

# CPPFLAGS = compiler options for C and C++
CPPFLAGS := -Wall -g -Os -mcpu=cortex-m4 -mthumb -nostdlib -MMD -fdata-sections -ffunction-sections $(OPTIONS) -I. -I$(TEENSY_HOME) $(LIBRARY_INCLUDES)

# compiler options for C++ only
CXXFLAGS := -std=gnu++0x -felide-constructors -fno-exceptions -fno-rtti

# compiler options for C only
CFLAGS :=

# linker options
LDFLAGS := -Os -Wl,--gc-sections -mcpu=cortex-m4 -mthumb -T./mk20dx256.ld

# additional libraries to link
LIBS := -lm -larm_cortexM4l_math

# names for the compiler programs
CC := $(abspath $(COMPILERPATH))/arm-none-eabi-gcc
CXX := $(abspath $(COMPILERPATH))/arm-none-eabi-g++
OBJCOPY := $(abspath $(COMPILERPATH))/arm-none-eabi-objcopy
SIZE := $(abspath $(COMPILERPATH))/arm-none-eabi-size

# automatically create lists of the sources and objects
C_FILES := $(wildcard *.c) $(LIBRARY_C_FILES) $(wildcard $(TEENSY_HOME)/*.c)
CPP_FILES := $(wildcard *.cpp) $(LIBRARY_CPP_FILES) $(wildcard $(TEENSY_HOME)/*.cpp)
ABSOLUTE_OBJS := $(TARGET).o $(C_FILES:.c=.o) $(CPP_FILES:.cpp=.o)
OBJS :=  $(addprefix $(OBJ_DIR)/, $(notdir $(ABSOLUTE_OBJS)))
OBJ_DEPS := $(OBJS:.o=.d)

# the actual makefile rules (all .o files built by GNU make's default implicit rules)

all: $(OBJ_DIR) $(TARGET).hex
.PHONY: all

$(TARGET).elf: $(OBJS) $(TEENSY_HOME)/mk20dx256.ld
	@echo "[LINK] $@"
	@$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

$(OBJ_DIR)/$(TARGET).o: $(TARGET).ino
	@echo "[INO] $<"
	@$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o "$@" -c -x c++ -include Arduino.h "$<"

%.hex: %.elf
	$(SIZE) $<
	$(OBJCOPY) -O ihex -R .eeprom $< $@
	@echo "Uploading..."
	@$(abspath $(TOOLSPATH))/teensy_post_compile -file=$(basename $@) -path=$(shell pwd) -tools=$(abspath $(TOOLSPATH))
	@-$(abspath $(TOOLSPATH))/teensy_reboot

$(OBJ_DIR)/%.o: %.c
	@echo "[CC] $<"
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) -o "$@" "$<"

$(OBJ_DIR)/%.o: %.cpp
	@echo "[CXX] $<"
	@$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) -o "$@" "$<"

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# compiler generated dependency info
-include $(OBJS:.o=.d)

clean:
	$(RM) -rf $(OBJ_DIR)
	$(RM) -f $(TARGET).elf $(TARGET).hex
.PHONY: clean

