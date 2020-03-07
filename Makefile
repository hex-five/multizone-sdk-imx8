# Copyright(C) 2018 Hex Five Security, Inc. - All Rights Reserved


#############################################################
# Platform definitions
#############################################################

BOARD ?= STM32F767
ifeq ($(BOARD),STM32F767)
	ARM_ARCH := armv7e-m
	ARM_CPU := cortex-m7
	ARM_FLOAT := soft
	ARM_FPU := fpv5-sp-d16
else ifeq ($(BOARD),IMX8QM)
	ARM_ARCH := armv7e-m
	ARM_CPU := cortex-m7
	ARM_FLOAT := soft
	ARM_FPU := fpv5-sp-d16
else 
	$(error Unsupported board $(BOARD))
endif


#############################################################
# Arguments/variables available to all submakes
#############################################################

export BOARD
export ARM_ARCH
export ARM_CPU
export ARM_FLOAT
export ARM_FPU

#############################################################
# Toolchain definitions
#############################################################

ifndef GNU_ARM
$(error GNU_ARM (GNU Arm Embedded Toolchain) not set)
endif

export CROSS_COMPILE := $(abspath $(GNU_ARM))/arm-none-eabi-
export CC      := $(CROSS_COMPILE)gcc
export OBJDUMP := $(CROSS_COMPILE)objdump
export OBJCOPY := $(CROSS_COMPILE)objcopy
export GDB     := $(CROSS_COMPILE)gdb
export AR      := $(CROSS_COMPILE)ar
export LD      := $(CROSS_COMPILE)gcc


#############################################################
# Rules for building multizone
#############################################################

.PHONY: all 
all: clean
	$(MAKE) -C zone1
	$(MAKE) -C zone2
	$(MAKE) -C zone3
	$(MAKE) -C zone4
	java -jar multizone.jar \
	-k ../hexfive-kern-arm/build/IMX8QM/kernel.hex \
	-a $(BOARD) \
	-c bsp/$(BOARD)/multizone.cfg \
	zone1/zone1.elf \
	zone2/zone2.elf \
	zone3/zone3.elf \
	zone4/zone4.elf 


.PHONY: clean
clean: 
	$(MAKE) -C zone1 clean
	$(MAKE) -C zone2 clean
	$(MAKE) -C zone3 clean
	$(MAKE) -C zone4 clean
	rm -f multizone.hex
	
