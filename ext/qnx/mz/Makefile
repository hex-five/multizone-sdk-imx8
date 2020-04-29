CROSS_COMPILE := $(QNX_HOST)/usr/bin/aarch64-unknown-nto-qnx7.0.0-
CC:=$(CROSS_COMPILE)gcc
AS:=$(CROSS_COMPILE)as
LD:=$(CROSS_COMPILE)ld
OBJCOPY:=$(CROSS_COMPILE)objcopy
OBJDUMP:=$(CROSS_COMPILE)objdump

TARGET:=mz
C_SRCS:=mz.c
OBJS:=$(C_SRCS:%.c=%.o) 
CFLAGS:= -g3
LDFLAGS:=

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

clean:
	-rm -f $(OBJS) $(TARGET)

.PHONY: all clean
.SECONDARY: $(OBJS)