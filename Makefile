SRCS = main.c stm32f4xx_it.c system_stm32f4xx.c
CFILES = src/main.c src/stm32f4xx_it.c src/system_stm32f4xx.c
TRACEFILE = src/trace_functions.c
TRACEOBJ = trace_functions.o

CRC_SRCS = util/CRC_Generator/crcmodel.c util/CRC_Generator/main.c

PROJ_NAME=example

ARM_CC=arm-none-eabi-gcc #ARM cross compiler
GCC=gcc				     #Standard desktop GCC for building crcGenerator utility
READELF=arm-none-eabi-readelf
PYTHON=python

BASE_FLAGS  = -g -O0 -Wall -ffunction-sections -fdata-sections
BASE_FLAGS += -mlittle-endian -mthumb -mcpu=cortex-m4 -mthumb-interwork
BASE_FLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
BASE_FLAGS += -Iinc -Ilib -Ilib/inc -Iobjs -Igen -Isrc
BASE_FLAGS += -Ilib/inc/core -Ilib/inc/peripherals
BASE_FLAGS += -I/usr/local/include -I/usr/local/include/sys

CFLAGS=$(BASE_FLAGS)
CFLAGS += -Tstm32_flash.ld
CFLAGS += -finstrument-functions

SECONDARY_CFLAGS=$(BASE_FLAGS)
SECONDARY_CFLAGS += -Tgen/secondary_seu_link.ld

TRACEFLAGS=$(BASE_FLAGS)

vpath %.c src
vpath %.a lib

SRCS += lib/startup_stm32f4xx.s

OBJS = $(SRCS:.c=.o)

################################################################################################################

all: lib FinalSourceCompilation utils

lib:
	$(MAKE) -C lib

$(OBJS): $(SRCS)
	$(ARM_CC) -c $(SRCS)

utils:
	$(GCC) $(CRC_SRCS) -o crcGenerator

initialCompilation: utils
	$(MAKE) -C lib
	$(ARM_CC) $(TRACEFLAGS) -c $(TRACEFILE)
	$(ARM_CC) $(CFLAGS) -c $(CFILES)
	$(ARM_CC) $(CFLAGS) $(OBJS) $(TRACEOBJ) -o initial_seu_binary.elf -Llib -lstm32f4

initialProfiler: initialCompilation
	mkdir -p gen
	$(READELF) -s initial_seu_binary.elf | grep "FUNC" | awk '{print $$2 " " $$3 " " $$8}' > gen/addressDump.data
	$(PYTHON) initial_profiler.py
	$(ARM_CC) $(CFLAGS) -c gen/secondary_seu_headers.c

SecondarySourceCompilation: initialProfiler
	$(ARM_CC) $(SECONDARY_CFLAGS) $(OBJS) $(TRACEOBJ) secondary_seu_headers.o -o secondary_seu_binary.elf -Llib -lstm32f4

SecondaryProfiler: SecondarySourceCompilation
	$(READELF) -x .text secondary_seu_binary.elf | sed '3q;d' | awk '{print $$1}' > gen/startAddr.data
	$(READELF) -x .text secondary_seu_binary.elf | awk '{print $$2 $$3 $$4 $$5}' > gen/textHexDump.hex
	$(READELF) -s secondary_seu_binary.elf | grep "FUNC" | awk '{print $$2 " " $$3 " " $$8}' > gen/addressDump.data
	$(PYTHON) secondary_profiler.py

FinalSourceCompilation: SecondaryProfiler
	$(ARM_CC) $(CFLAGS) -c gen/secondary_seu_headers.c
	$(ARM_CC) $(SECONDARY_CFLAGS) $(OBJS) $(TRACEOBJ) secondary_seu_headers.o -o final_seu_binary.elf -Llib -lstm32f4
	echo "\nFinal Link complete, outputFile: final_seu_binary.elf"

clean:
	$(MAKE) -C lib clean
	rm -f initial_seu_binary.elf
	rm -f secondary_seu_binary.elf
	rm -f final_seu_binary.elf
	rm -f crcGenerator
	rm -f *.o
	rm -rf gen/
