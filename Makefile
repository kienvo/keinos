CC = i686-elf-gcc
AS = i686-elf-gcc
LD = i686-elf-gcc

OBJDIR ?= obj
BINDIR ?= bin
TARGET = $(BINDIR)/keinix.bin
LIST   = $(BINDIR)/keinix.s
MAP   = $(BINDIR)/keinix.map

_OBJS += boot.o 
_OBJS += kernel.o 
_OBJS += io.o 
_OBJS += console.o 
_OBJS += common.o 
_OBJS += interrupt.o
_OBJS += load_DT.o
_OBJS += serial.o
_OBJS += tty-io.o
OBJS = $(patsubst %,$(OBJDIR)/%,$(_OBJS))

CFLAGS += -c 
CFLAGS += -g 
CFLAGS += -std=gnu99
CFLAGS += -ffreestanding 
CFLAGS += -O0
CFLAGS += -Wall 
CFLAGS += -Wextra 
CFLAGS += -Wchkp 
CFLAGS += -Werror=implicit-function-declaration 
CFLAGS += -mno-80387
CFLAGS += -MMD
CFLAGS += -nostdlib  

ASFLAGS += -O0
ASFLAGS += -g 
ASFLAGS += -c

LDFLAGS += -ffreestanding 
LDFLAGS += -O0
LDFLAGS += -nostdlib  
LDFLAGS += -lgcc 
LDFLAGS += -Xlinker 
LDFLAGS += -Map=$(MAP)
LDFLAGS += -T linker.ld 

.PHONY: all run clean rebuild 
.SUFFIXES: .o .c .S .h

all: $(TARGET)
rebuild: clean $(TARGET) 

#.c.o:
$(OBJDIR)/%.o: %.c
	@echo "BUILD $<"
	@mkdir -pv $(dir $@)
	$(CC) $(CFLAGS) $< -o $@ 
#.S.o:
$(OBJDIR)/%.o: %.S
	@echo "BUILD $<"
	@mkdir -pv $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@
$(TARGET): $(OBJS) linker.ld
	@echo "LINKING $@:" 
	@mkdir -pv $(dir $@)
	$(LD) $(LDFLAGS) -o $@  $(OBJS)
	@$(MAKE) --no-print-directory $(LIST)

$(LIST): $(TARGET)
	@echo '\012\012 /*********************** ELF INFOR ***********************/' >$@
	@printf '\012' >>$@
	@readelf $< -a >> $@
	@objdump -S -d -mi386 $< >>$@
	@echo '\012\012 /*********************** HEX ***********************/' >>$@
	@printf '\012' >>$@
	@xxd $< >>$@
clean:
	rm -rf $(OBJDIR)/ $(BINDIR)/
run: $(TARGET)
	qemu-system-i386 -kernel $< -serial stdio
debug: $(TARGET)
	qemu-system-i386 -kernel $< -s -S
serialdebug: $(TARGET)
	qemu-system-i386 -kernel $(TARGET) -serial telnet:127.0.0.1:4444,server,nowait | telnet 127.0.0.1 4444

-include $(OBJS:%.o=%.d)