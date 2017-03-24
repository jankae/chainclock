MCU = attiny2313
F_CPU = 1000000UL

PROGRAMMER_TYPE = avrispmkII
# extra arguments to avrdude: baud rate, chip type, -F flag, etc.
PROGRAMMER_ARGS = -P usb

CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
AVRSIZE = avr-size
AVRDUDE = avrdude

EFUSE = 0xFF
HFUSE = 0xDF
LFUSE = 0x7F

##########------------------------------------------------------##########
##########                   Makefile Magic!                    ##########
##########         Summary:                                     ##########
##########             We want a .hex file                      ##########
##########        Compile source files into .elf                ##########
##########        Convert .elf file into .hex                   ##########
##########        You shouldn't need to edit below.             ##########
##########------------------------------------------------------##########

## The name of your project (without the .c)
TARGET = main

# Object files: will find all .c/.h files in current directory
#  and in LIBDIR.  If you have any other (sub-)directories with code,
#  you can add them in to SOURCES below in the wildcard statement.
SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)
HEADERS=$(SOURCES:.c=.h)

## Compilation options, type man avr-gcc if you're curious.
CFLAGS = -Os -g -std=gnu99 -Wall -Wextra -DF_CPU=$(F_CPU)

## Use short (8-bit) data types 
CFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums 
## Splits up object files per function
CFLAGS += -ffunction-sections -fdata-sections 
LDFLAGS = -Wl,-Map,$(TARGET).map 
## Optional, but often ends up with smaller code
LDFLAGS += -Wl,--gc-sections 
## Relax shrinks code even more, but makes disassembly messy
## LDFLAGS += -Wl,--relax
## LDFLAGS += -Wl,-u,vfprintf -lprintf_flt -lm  ## for floating-point printf
## LDFLAGS += -Wl,-u,vfprintf -lprintf_min      ## for smaller printf
TARGET_ARCH = -mmcu=$(MCU)

## Explicit pattern rules:
all: $(TARGET).hex
	$(AVRSIZE) -C --mcu=$(MCU) $(TARGET).elf

##  To make .o files from .c files 
%.o: %.c $(HEADERS) Makefile
	 $(CC) $(CFLAGS) $(TARGET_ARCH) -c -o $@ $<;

$(TARGET).elf: $(OBJECTS)
	$(CC) $(LDFLAGS) $(TARGET_ARCH) $^ $(LDLIBS) -o $@

%.hex: %.elf
	 $(OBJCOPY) -j .text -j .data -O ihex $< $@

## These targets don't have files named after them
.PHONY: all clean flash fuses

clean:
	rm -f $(TARGET).elf $(TARGET).hex $(TARGET).obj \
	$(TARGET).o $(TARGET).d $(TARGET).eep $(TARGET).lst \
	$(TARGET).lss $(TARGET).sym $(TARGET).map $(TARGET)~ \
	$(TARGET).eeprom $(OBJECTS)

flash: $(TARGET).hex 
	@echo Calling avrdude to flash the firmware...
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS) -U flash:w:$(TARGET).hex

fuses:
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS) -u -U hfuse:w:$(HFUSE):m -U lfuse:w:$(LFUSE):m -U efuse:w:$(EFUSE):m

reset:
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS)

