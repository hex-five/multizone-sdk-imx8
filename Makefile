# Copyright(C) 2018 Hex Five Security, Inc. - All Rights Reserved


#############################################################
# Platform definitions
#############################################################

BOARD ?= IMX8QM
ifeq ($(BOARD),IMX8QM)
	ARM_ARCH := armv7e-m
	ARM_CPU := cortex-m4
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
	--arch $(BOARD) \
	--config bsp/$(BOARD)/multizone.cfg \
	zone1/zone1.elf \
	zone2/zone2.elf \
	zone3/zone3.elf \
	zone4/zone4.elf
	@$(OBJCOPY) -S -Iihex -Obinary multizone.hex multizone.bin > /dev/null

	@./ext/mkimage/imx-mkimage/mkimage_imx8 -soc QM -rev B0 -append \
	./ext/secofw/firmware-imx-8.1/firmware/seco/mx8qm-ahab-container.img \
	-c -flags 0x01210000 \
	-scfw ./ext/scfw/packages/imx-scfw-porting-kit-1.2.7.1/src/scfw_export_mx8qm_b0/build_mx8qm_b0/scfw_tcm.bin \
	-ap ./ext/qnx/ipl-imx8qm-cpu-mek.bin a53 0x80000000 \
	-p3 -m4 ./multizone.bin 0 0x34FE0000 \
	-out multizone.imx > /dev/null 2>&1
 
.PHONY: clean
clean: 
	$(MAKE) -C zone1 clean
	$(MAKE) -C zone2 clean
	$(MAKE) -C zone3 clean
	$(MAKE) -C zone4 clean
	rm -f multizone.hex multizone.bin *.imx
	
