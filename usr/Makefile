
# Arduino IDE installation location
ARDUINO_HOME := /Applications/Arduino.app/Contents/Resources/Java

# Configurable options
OPTIONS = -DF_CPU=48000000 

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


# CPPFLAGS = compiler options for C and C++
CPPFLAGS := -Wall -g -Os -e start -mcpu=cortex-m4 -mthumb -nostdlib -MMD $(OPTIONS) -I. -I$(TEENSY_HOME) $(LIBRARY_INCLUDES)

# compiler options for C only
CFLAGS :=

# linker options
LDFLAGS := -Os -e start -mcpu=cortex-m4 -nostartfiles -mthumb -T./main.ld

# additional libraries to link
LIBS := -lm -larm_cortexM4l_math

# names for the compiler programs
CC := $(abspath $(COMPILERPATH))/arm-none-eabi-gcc
CXX := $(abspath $(COMPILERPATH))/arm-none-eabi-g++
OBJCOPY := $(abspath $(COMPILERPATH))/arm-none-eabi-objcopy
SIZE := $(abspath $(COMPILERPATH))/arm-none-eabi-size

all: main.hex

%.hex: %.elf
	$(SIZE) $<
	$(OBJCOPY) -O ihex -R .eeprom $< $@

main.elf: $(OBJ_DIR) $(OBJ_DIR)/main.o ./main.ld 
	@echo "[LINK] $@"
	@$(CC) $(LDFLAGS) -o $@ $(OBJ_DIR)/main.o 

$(OBJ_DIR)/%.o: %.c
	@echo "[CC] $<"
	@$(CC) -c $(CPPFLAGS) $(CFLAGS) -o "$@" "$<"
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# compiler generated dependency info
#-include $(OBJS:.o=.d)

clean:
	$(RM) -rf $(OBJ_DIR)
	$(RM) -f main.elf main.hex
.PHONY: clean


