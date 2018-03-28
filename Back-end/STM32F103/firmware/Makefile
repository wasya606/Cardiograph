# ------------------------------------------------
# Generic Makefile (based on gcc)
#
# ChangeLog :
#	2017-02-10 - Several enhancements + project update mode
#   2015-07-22 - first version
# ------------------------------------------------

######################################
# target
######################################
TARGET = CardiographSTM32F103


######################################
# building variables
######################################
# debug build?
DEBUG = 1
# optimization
OPT = -Og


#######################################
# paths
#######################################
# source path
SOURCES_DIR =  \
src \
drivers/STM32F1xx_HAL_Driver/Src


# firmware library path
PERIFLIB_PATH = 

# Build path
BUILD_DIR = build

######################################
# source
######################################
# C sources
C_SOURCES =  \
src/HAL/adc.c \
src/HAL/dma.c \
src/HAL/gpio.c \
src/HAL/tim.c \
src/HAL/usart.c \
src/HAL/spi.c \
src/HAL/stm32f1xx_it.c \
src/HAL/stm32f1xx_hal_msp.c \
src/HAL/system_stm32f1xx.c \
src/HAL/sys_conf.c \
drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c \
drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio_ex.c \
drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c \
drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash.c \
drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_adc.c \
drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim.c \
drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c \
drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_uart.c \
drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_adc_ex.c \
drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash_ex.c \
drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim_ex.c \
drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c \
drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pwr.c \
drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dma.c \
drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c \
drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_spi.c \
drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_spi_ex.c

# C++sources
CPP_SOURCES =  \
src/main.cpp \
src/Impl/oled_driver.cpp \
src/Impl/cardiograph.cpp \
src/Impl/oledfont.cpp \
src/Impl/timer.cpp

# ASM sources
ASM_SOURCES =  \
src/asm/startup_stm32f103xb.s


######################################
# firmware library
######################################
PERIFLIB_SOURCES = 


#######################################
# binaries
#######################################
BINPATH = ~/Install/STM-32/gcc-arm-none-eabi-6-2017-q2-update/bin
PREFIX = arm-none-eabi-
CC = $(BINPATH)/$(PREFIX)gcc
CXX = $(BINPATH)/$(PREFIX)g++
AS = $(BINPATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(BINPATH)/$(PREFIX)objcopy
AR = $(BINPATH)/$(PREFIX)ar
SZ = $(BINPATH)/$(PREFIX)size
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S
 
#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m3

# fpu
# NONE for Cortex-M0/M0+/M3

# float-abi


# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# macros for gcc
# AS defines
AS_DEFS = 

# C defines
C_DEFS =  \
-DUSE_HAL_DRIVER \
-DSTM32F103xB \
-DUSE_SPI_CRC


# AS includes
AS_INCLUDES = 

# C includes
C_INCLUDES =  \
-Isrc \
-Isrc/HAL \
-Idrivers/STM32F1xx_HAL_Driver/Inc \
-Idrivers/STM32F1xx_HAL_Driver/Inc/Legacy \
-Idrivers/CMSIS/Device/ST/STM32F1xx/Include \
-Idrivers/CMSIS/Include

CPP_INCLUDES = \
-Isrc \
-Isrc/HAL \
-Isrc/Impl \
-Idrivers/STM32F1xx_HAL_Driver/Inc \
-Idrivers/STM32F1xx_HAL_Driver/Inc/Legacy \
-Idrivers/CMSIS/Device/ST/STM32F1xx/Include \
-Idrivers/CMSIS/Include

ARCHFLAGS = -mthumb -mcpu=cortex-m3 -mfix-cortex-m3-ldrd

ALLFLAGS  = -g3 -O2
ALLFLAGS += -Wall -Wextra -Wundef
ALLFLAGS += -fno-common -ffunction-sections -fdata-sections
CWARNFLAGS := -Wimplicit-function-declaration -Wmissing-prototypes -Wstrict-prototypes -Wno-unused-parameter
CXXWARNFLAGS := -Wno-unused-parameter
ALLFLAGS += -ffreestanding
ALLFLAGS += -mlittle-endian
ALLFLAGS += $(ARCHFLAGS)
ALLFLAGS += $(C_DEFS)
ALLFLAGS += -nostdlib
#ALLFLAGS += -I. $(C_INCLUDES)

# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif


# Generate dependency information
CFLAGS   = $(ALLFLAGS) $(C_INCLUDES) $(CWARNFLAGS)
CXXFLAGS = $(ALLFLAGS)  $(CPP_INCLUDES) -std=gnu++11 $(CXXWARNFLAGS) -fno-rtti -fno-exceptions -fno-threadsafe-statics

#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = STM32F103CBTx_FLASH.ld

# libraries
LIBS = -lc -lm -lnosys -lrdimon
LIBDIR =

LDFLAGS = $(MCU)
LDFLAGS += -specs=nano.specs -specs=nosys.specs
LDFLAGS  += --static #-nostartfiles
LDFLAGS += -g3
LDFLAGS += -T$(LDSCRIPT)
LDFLAGS += -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref
LDFLAGS += -Wl,--gc-sections
LDFLAGS += -Wl,--start-group $(LIBS) -Wl,--end-group

# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin

#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))

OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(CPP_SOURCES:.cpp=.o)))
vpath %.cpp $(sort $(dir $(CPP_SOURCES)))

# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

$(info OBJECTS is $(OBJECTS) )

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@
	#$(CC) $(CFLAGS) -c -MMD -o $@ $<

$(BUILD_DIR)/%.o: %.cpp Makefile | $(BUILD_DIR)
	$(CC) -c $(CXXFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.cpp=.lst)) $< -o $@
	#$(CXX) $(CXXFLAGS) -c -MMD -o $@ $<

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $@
	#$(CXX) $(LDFLAGS) $(OBJECTS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@
	
$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@	
	
$(BUILD_DIR):
	mkdir $@		

#######################################
# clean up
#######################################
clean:
	-rm -fR .dep $(BUILD_DIR)
  
#######################################
# dependencies
#######################################
-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)

# *** EOF ***
