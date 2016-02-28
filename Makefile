TARGET:=FreeRTOS
# old root for reference TOOLCHAIN_ROOT ?= ~/stm/gcc-arm-none-eabi-4_9-2015q3
ifeq (,$(TOOLCHAIN_ROOT))
TOOLCHAIN_ROOT := /usr/lib
endif
TOOLCHAIN_ROOT := $(TOOLCHAIN_ROOT)
TOOLCHAIN_BIN := $(TOOLCHAIN_ROOT)/bin
TOOLCHAIN_PREFIX := arm-none-eabi

OPTLVL:=0
DBG:=-g

FREERTOS:=$(CURDIR)/FreeRTOS
STARTUP:=$(CURDIR)/hardware

BUILD_DIR = $(CURDIR)/build
BIN_DIR = $(CURDIR)/binary
SEU_DIR = $(CURDIR)/seu
SEU_SRC_DIR = $(SEU_DIR)/src
SEU_GEN_DIR = $(SEU_DIR)/gen
UTIL_DIR = $(CURDIR)/util
CRC_SRC_DIR = $(UTIL_DIR)/CRC_Generator
CRC_BUILD_DIR = $(BUILD_DIR)/crc_build

NUM_TEXT_SECTIONS = 4 #Number of .textX sections declared in linker script

INCLUDE=-I$(CURDIR)/hardware
INCLUDE+=-I$(FREERTOS)/include
INCLUDE+=-I$(FREERTOS)/portable/GCC/ARM_CM4F
INCLUDE+=-I$(CURDIR)/Libraries/CMSIS/Device/ST/STM32F4xx/Include
INCLUDE+=-I$(CURDIR)/Libraries/CMSIS/Include
INCLUDE+=-I$(CURDIR)/Libraries/STM32F4xx_StdPeriph_Driver/inc
INCLUDE+=-I$(CURDIR)/config
INCLUDE+=-I$(SEU_DIR)/include

ASRC=startup_stm32f4xx.s

vpath %.c $(CURDIR)/Libraries/STM32F4xx_StdPeriph_Driver/src \
	  $(CURDIR)/Libraries/syscall $(CURDIR)/hardware $(FREERTOS) \
	  $(FREERTOS)/portable/MemMang $(FREERTOS)/portable/GCC/ARM_CM4F \
	  $(CRC_SRC_DIR) $(SEU_SRC_DIR)

SRC+=system_stm32f4xx.c
SRC+=main.c
SRC+=syscalls.c

SRC+=port.c
SRC+=list.c
SRC+=queue.c
SRC+=tasks.c
SRC+=event_groups.c
SRC+=timers.c
SRC+=heap_4.c

SRC+=stm32f4xx_syscfg.c
SRC+=misc.c
SRC+=stm32f4xx_adc.c
SRC+=stm32f4xx_dac.c
SRC+=stm32f4xx_dma.c
SRC+=stm32f4xx_exti.c
SRC+=stm32f4xx_flash.c
SRC+=stm32f4xx_gpio.c
SRC+=stm32f4xx_i2c.c
SRC+=stm32f4xx_rcc.c
SRC+=stm32f4xx_spi.c
SRC+=stm32f4xx_tim.c
SRC+=stm32f4xx_usart.c
SRC+=stm32f4xx_rng.c

CRC_SRC := $(CRC_SRC_DIR)/*.c
CRC_SRC := $(filter-out trace_functions.c, $(CRC_SRC)) #trace_functions.c only used on the target, not in CRC_Generator
TRACE_SRC = $(SEU_SRC_DIR)/trace_functions.c
TRACE_OBJ = $(BUILD_DIR)/trace_functions.o

UNCHECKED_SRC := $(STARTUP)/stm32f4xx_it.c #compiled without finstrument-function
UNCHECKED_OBJ = $(BUILD_DIR)/stm32f4xx_it.o

CDEFS=-DUSE_STDPERIPH_DRIVER
CDEFS+=-DSTM32F4XX
CDEFS+=-DHSE_VALUE=8000000
CDEFS+=-D__FPU_PRESENT=1
CDEFS+=-D__FPU_USED=1
CDEFS+=-DARM_MATH_CM4

MCUFLAGS=-mcpu=cortex-m4 -mthumb -mfloat-abi=hard
COMMONFLAGS=-O$(OPTLVL) $(DBG) -Wall -ffunction-sections -fdata-sections
CFLAGS=$(COMMONFLAGS) $(MCUFLAGS) $(INCLUDE) $(CDEFS) -falign-functions=32
LDLIBS=$(TOOLCHAIN_ROOT)/arm-none-eabi/lib/armv7e-m/fpu/libc.a $(TOOLCHAIN_ROOT)/arm-none-eabi/lib/armv7e-m/fpu/libm.a
LDFLAGS=$(COMMONFLAGS) -fno-exceptions -nostartfiles
SEUFLAG=-finstrument-functions

INITIAL_LINKERSCRIPT=-T$(SEU_DIR)/initial_seu_link.ld
SECONDARY_LINKERSCRIPT=-T$(SEU_GEN_DIR)/secondary_seu_link.ld

# don't count on having the tools in the PATH...
CC := $(TOOLCHAIN_BIN)/$(TOOLCHAIN_PREFIX)-gcc
LD := $(TOOLCHAIN_BIN)/$(TOOLCHAIN_PREFIX)-gcc
OBJCOPY := $(TOOLCHAIN_BIN)/$(TOOLCHAIN_PREFIX)-objcopy
AS := $(TOOLCHAIN_BIN)/$(TOOLCHAIN_PREFIX)-as
AR := $(TOOLCHAIN_BIN)/$(TOOLCHAIN_PREFIX)-ar
GDB := $(TOOLCHAIN_BIN)/$(TOOLCHAIN_PREFIX)-gdb
READELF := $(TOOLCHAIN_BIN)/$(TOOLCHAIN_PREFIX)-readelf

GCC := gcc #Standard Desktop GCC
PYTHON := python3

OBJ = $(SRC:%.c=$(BUILD_DIR)/%.o)

all: utils SECONDARY_PROFILER

utils:
	@echo "[GCC] crcGenerator"
	@test -d $(BUILD_DIR) || mkdir -p $(BUILD_DIR)
	@$(GCC) $(INCLUDE) $(CRC_SRC) -o $(BUILD_DIR)/crcGenerator

$(BUILD_DIR)/%.o: %.c
	@echo [CC] $(notdir $<)
	@test -d $(BUILD_DIR) || mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) $(SEUFLAG) $< -c -o $@

UNCHECKED_OBJS: $(OBJ) 
	@$(CC) $(CFLAGS) $(INCLUDE) $(UNCHECKED_SRC) -c -o $(UNCHECKED_OBJ)
	@$(CC) $(CFLAGS) $(TRACE_SRC) -c -o $(TRACE_OBJ)

INITIAL_COMPILATION: UNCHECKED_OBJS
	@echo "[AS] $(ASRC)"
	@$(AS) -o $(ASRC:%.s=$(BUILD_DIR)/%.o) $(STARTUP)/$(ASRC)
	@echo [LD] $(TARGET).elf
	@test -d $(BIN_DIR) || mkdir -p $(BIN_DIR)
	@$(CC) -o $(BIN_DIR)/initial$(TARGET).elf $(INITIAL_LINKERSCRIPT) $(LDFLAGS) $(OBJ) $(TRACE_OBJ) $(ASRC:%.s=$(BUILD_DIR)/%.o) $(LDLIBS)


INITIAL_PROFILER: INITIAL_COMPILATION
	@echo "Starting Initial Profiler"
	@test -d $(SEU_GEN_DIR) || mkdir -p $(SEU_GEN_DIR)
	@$(READELF) --wide -s binary/initialFreeRTOS.elf| grep " FUNC    " | awk '{print $$3 " " $$8 }' | sort -k 2 | uniq -u  > seu/gen/fullMap.data
	@awk '/\*{6}/{x++}{print >"seu/gen/template_half_" x ".ld" }' x=0 seu/initial_seu_link.ld #Split Linker script in half 
	@$(PYTHON) $(SEU_DIR)/initial_profiler.py
	@echo "initial Profiler Completed"

SECONDARY_COMPILATION: INITIAL_PROFILER
	@echo "Starting Secondary Complilation"
	@$(CC) -Wl,-Map,$(TARGET).map -o $(BIN_DIR)/final$(TARGET).elf $(SECONDARY_LINKERSCRIPT) $(LDFLAGS) $(OBJ) $(TRACE_OBJ) $(HEADER_OBJ) $(ASRC:%.s=$(BUILD_DIR)/%.o) $(LDLIBS)
	@echo "Secondary Complilation Completed"

SECONDARY_PROFILER: SECONDARY_COMPILATION 
	@echo "generating hexDumps"
	@x=1; while [[ $$x -le $(NUM_TEXT_SECTIONS)]] ; do \
		$(READELF) -x .text$$x binary/final$(TARGET).elf | awk '{print $$2 " " $$3 " " $$4 " " $$5}' | tail -n+3 > $(SEU_GEN_DIR)/text_hex_dump_$$x.hex; \
		((x = x + 1)); \
	done
	@echo "running Secondary/Final Profiler"
	$(PYTHON) $(SEU_DIR)/secondary_profiler.py

.PHONY: clean

clean:
	@echo [RM] BUILD DIR
	@rm -f $(BUILD_DIR)/*
	@echo [RM] GEN DIR
	@rm -f $(SEU_GEN_DIR)/*
	@echo [RM] BIN DIR
	@rm -f $(BIN_DIR)/*
