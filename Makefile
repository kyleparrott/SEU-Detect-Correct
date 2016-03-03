TARGET:=FreeRTOS
# old root for reference TOOLCHAIN_ROOT ?= ~/stm/gcc-arm-none-eabi-4_9-2015q3
ifeq (,$(TOOLCHAIN_ROOT))
TOOLCHAIN_ROOT := /usr/lib
endif
TOOLCHAIN_ROOT := $(abspath $(TOOLCHAIN_ROOT))
TOOLCHAIN_BIN := $(TOOLCHAIN_ROOT)/bin
TOOLCHAIN_PREFIX := arm-none-eabi

OPTLVL:=0
#DBG:=-g
DBG:=

FREERTOS:=$(CURDIR)/FreeRTOS
STARTUP:=$(CURDIR)/hardware
REED_SOLOMON:=$(abspath $(CURDIR)/../Reed-Solomon-Packed)

NUM_TEXT_SECTIONS = 4 #Number of .textX sections declared in linker script

INCLUDE=-I$(CURDIR)/hardware
INCLUDE+=-I$(FREERTOS)/include
INCLUDE+=-I$(FREERTOS)/portable/GCC/ARM_CM4F
INCLUDE+=-I$(CURDIR)/Libraries/CMSIS/Device/ST/STM32F4xx/Include
INCLUDE+=-I$(CURDIR)/Libraries/CMSIS/Include
INCLUDE+=-I$(CURDIR)/Libraries/STM32F4xx_StdPeriph_Driver/inc
INCLUDE+=-I$(CURDIR)/config
INCLUDE+=-Iseu/include
INCLUDE+=-I$(REED_SOLOMON)/include

BUILD_DIR = $(CURDIR)/build
BIN_DIR = $(CURDIR)/binary
SEU_DIR = seu
SEU_SRC_DIR = $(SEU_DIR)/src
SEU_GEN_DIR = $(SEU_DIR)/gen

ASRC=startup_stm32f4xx.s

vpath %.c $(CURDIR)/Libraries/STM32F4xx_StdPeriph_Driver/src \
	  $(CURDIR)/Libraries/syscall $(CURDIR)/hardware $(FREERTOS) \
	  $(FREERTOS)/portable/MemMang $(FREERTOS)/portable/GCC/ARM_CM4F

UNCHECKED_SRC := stm32f4xx_it.c #compiled without finstrument-function
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

CRC_SRCS = util/CRC_Generator/crcmodel.c util/CRC_Generator/main.c

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

INITIAL_LINKERSCRIPT=-Tseu/initial_seu_link.ld
SECONDARY_LINKERSCRIPT=-Tseu/gen/secondary_seu_link.ld

TRACE_FILES = seu/src/trace_functions.c
TRACE_OBJ = build/trace_functions.o

REED_SOLOMON_OBJS := $(BUILD_DIR)/alpha_to.o $(BUILD_DIR)/index_of.o $(BUILD_DIR)/genpoly.o

# don't count on having the tools in the PATH...
CC := $(TOOLCHAIN_BIN)/$(TOOLCHAIN_PREFIX)-gcc
LD := $(TOOLCHAIN_BIN)/$(TOOLCHAIN_PREFIX)-gcc
OBJCOPY := $(TOOLCHAIN_BIN)/$(TOOLCHAIN_PREFIX)-objcopy
AS := $(TOOLCHAIN_BIN)/$(TOOLCHAIN_PREFIX)-as
AR := $(TOOLCHAIN_BIN)/$(TOOLCHAIN_PREFIX)-ar
GDB := $(TOOLCHAIN_BIN)/$(TOOLCHAIN_PREFIX)-gdb
READELF := $(TOOLCHAIN_BIN)/$(TOOLCHAIN_PREFIX)-readelf

GCC=gcc #Standard Desktop GCC

PYTHON = python3

OBJ = $(SRC:%.c=$(BUILD_DIR)/%.o)
REED_SOLOMON_SRC := $(REED_SOLOMON)/src/alpha_to.c $(REED_SOLOMON)/src/index_of.c $(REED_SOLOMON)/src/genpoly.c
REED_SOLOMON_OBJ := $(BUILD_DIR)/alpha_to.o $(BUILD_DIR)/index_of.o $(BUILD_DIR)/genpoly.o

all: utils SECONDARY_PROFILER

utils:
	@echo [CC] crcGenerator.c
	@test -d $(BUILD_DIR) || mkdir -p $(BUILD_DIR)
	$(GCC) $(CRC_SRCS) $(REED_SOLOMON_SRC) $(INCLUDE) -I$(REED_SOLOMON)/include -g -o $(BUILD_DIR)/crcGenerator

REED_SOLOMON_OBJS: $(REED_SOLOMON_OBJ)

$(BUILD_DIR)/alpha_to.o: $(REED_SOLOMON)/src/alpha_to.c $(REED_SOLOMON)/include/*
	@echo [CC] $(notdir $<)
	@$(CC) $(CFLAGS) $< -c -o $@

$(BUILD_DIR)/index_of.o: $(REED_SOLOMON)/src/index_of.c $(REED_SOLOMON)/include/*
	@echo [CC] $(notdir $<)
	@$(CC) $(CFLAGS) $< -c -o $@

$(BUILD_DIR)/genpoly.o: $(REED_SOLOMON)/src/genpoly.c $(REED_SOLOMON)/include/*
	@echo [CC] $(notdir $<)
	@$(CC) $(CFLAGS) $< -c -o $@

$(BUILD_DIR)/%.o: %.c
	@echo [CC] $(notdir $<)
	@test -d $(BUILD_DIR) || mkdir -p $(BUILD_DIR)
	@$(CC) $(CFLAGS) $(SEUFLAG) $< -c -o $@

UNCHECKED_OBJS: $(OBJ)
	@$(CC) $(CFLAGS) hardware/stm32f4xx_it.c -c -o build/stm32f4xx_it.o
	@$(CC) $(CFLAGS) $(TRACE_FILES) -c -o $(TRACE_OBJ)

INITIAL_COMPILATION: UNCHECKED_OBJS $(REED_SOLOMON_OBJS)
	@echo "[AS] $(ASRC)"
	@$(AS) -o $(ASRC:%.s=$(BUILD_DIR)/%.o) $(STARTUP)/$(ASRC)
	@echo [LD] $(TARGET).elf
	@test -d $(BIN_DIR) || mkdir -p $(BIN_DIR)
	@$(CC) -o $(BIN_DIR)/initial$(TARGET).elf $(INITIAL_LINKERSCRIPT) $(LDFLAGS) $(OBJ) $(REED_SOLOMON_OBJS) $(TRACE_OBJ) $(ASRC:%.s=$(BUILD_DIR)/%.o) $(LDLIBS)

INITIAL_PROFILER: INITIAL_COMPILATION
	@echo "Starting Initial Profiler"
	@test -d $(SEU_GEN_DIR) || mkdir -p $(SEU_GEN_DIR)
	@$(READELF) --wide -s binary/initialFreeRTOS.elf| grep " FUNC    " | awk '{print $$3 " " $$8 }' | sort -k 2 | uniq -u  > seu/gen/fullMap.data
	@awk '/\*-{6}\*/{x++}{print >"seu/gen/template_half_" x ".ld" }' x=0 seu/initial_seu_link.ld #Split Linker script in half
	@$(PYTHON) $(SEU_DIR)/initial_profiler.py
	@echo "initial Profiler Completed"

SECONDARY_COMPILATION: INITIAL_PROFILER
	@echo "Starting Secondary Complilation"
	@$(CC) -Wl,-Map,$(TARGET).map -o $(BIN_DIR)/final$(TARGET).elf $(SECONDARY_LINKERSCRIPT) $(LDFLAGS) $(OBJ) $(REED_SOLOMON_OBJS) $(TRACE_OBJ) $(HEADER_OBJ) $(ASRC:%.s=$(BUILD_DIR)/%.o) $(LDLIBS)
	@echo "Secondary Complilation Completed"

SECONDARY_PROFILER: SECONDARY_COMPILATION
	@echo "Starting Final Profiler"
	$(eval textOffset:=$(shell $(READELF) -S $(BIN_DIR)/final$(TARGET).elf | grep ".text  " | awk '{print $$6}'))
	@echo "Sarting CRC_Generator/elf modifier"
	./build/crcGenerator $(BIN_DIR)/final$(TARGET).elf $(textOffset)

.PHONY: clean

clean:
	@echo [RM] BUILD DIR
	@rm -f $(BUILD_DIR)/*
	@echo [RM] GEN DIR
	@rm -f $(SEU_GEN_DIR)/*
	@echo [RM] BIN DIR
	@rm -f $(BIN_DIR)/*
