##
 # @file  Makefile
 # @brief Script to compile sources and create flat binary using "make" program
 #
 # @author Saint-Genest Gwenael <gwen@cowlab.fr>
 # @copyright Cowlab (c) 2017
##
CROSS=arm-none-eabi-
TARGET=firmware

SRC = main.c uart.c spi.c mcp2515.c can.c delay.c error.c samd21.c
ASRC = startup.s

CC = $(CROSS)gcc
OC = $(CROSS)objcopy
OD = $(CROSS)objdump
EDBG = edbg

CFLAGS  = -mcpu=cortex-m0plus -mthumb
CFLAGS += -nostdlib

LDFLAGS = -nostartfiles -T smallplc.ld -Wl,-Map=$(TARGET).map,--cref,--gc-sections -static

_COBJ =  $(SRC:.c=.o)
COBJ = $(patsubst %, %,$(_COBJ))
_AOBJ =  $(ASRC:.s=.o)
AOBJ = $(patsubst %, %,$(_AOBJ))

## Directives ##################################################################

all: $(AOBJ) $(COBJ)
	@echo "  [LD] $(TARGET)"
	@$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET).elf $(AOBJ) $(COBJ)
	@echo "  [OC] $(TARGET).bin"
	@$(OC) -S $(TARGET).elf -O binary $(TARGET).bin
	@echo "  [OD] $(TARGET).dis"
	@$(OD) -D $(TARGET).elf > $(TARGET).dis

clean:
	@echo "  [RM] $(TARGET).*"
	@rm -f $(TARGET).elf $(TARGET).map $(TARGET).bin $(TARGET).dis
	@echo "  [RM] Temporary object (*.o)"
	@rm -f *.o
	@rm -f *~ doc/*~
	@echo "  [RM] Clean documentation files"
	@rm -rf doc/html

$(AOBJ) : %.o : %.s
	@echo "  [AS] $@"
	@$(CC) $(CFLAGS) -c $< -o $@

$(COBJ) : %.o: %.c
	@echo "  [CC] $@"
	@$(CC) $(CFLAGS) -c $< -o $@

.PHONY: doc
doc:
	@echo "[DOXYGEN] doc/doxygen.cfg"
	@cd doc; doxygen doxygen.cfg

# Use ataradov/edbg program to load the unit-test using CMSIS-DAP probe
load:
	$(EDBG) -bpv -t atmel_cm0p -f $(TARGET).bin
