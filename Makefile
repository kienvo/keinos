
CC=i686-elf-gcc

PROJECTDIR=
OBJDIR=obj
_OBJS = boot.o kernel.o io.o tty.o descriptor_table.o interrupt.o\
load_DT.o
OBJS = $(patsubst %,$(OBJDIR)/%,$(_OBJS))
BINDIR=bin


.PHONY: all build run clean rebuild debug
.SUFFIXES: .o .c .S .h
 

all: $(BINDIR)/myos.bin run
build: $(BINDIR)/myos.bin
rebuild: clean $(BINDIR)/myos.bin 

#.c.o:
$(OBJDIR)/%.o: %.c
	@echo
	@echo
	@echo "BUILD $< -> $@:"
	$(CC) -c $< -o $@ -g -std=gnu99 -ffreestanding -O2 -Wall -Wextra -Wchkp -Werror=implicit-function-declaration -mno-80387
#.S.o:
$(OBJDIR)/%.o: %.S
	@echo
	@echo
	@echo "BUILD $< -> $@:"
	$(CC) -c $< -o $@ -O2 -g
$(BINDIR)/myos.bin: $(OBJS) linker.ld
	@echo
	@echo
	@echo "BUILD $@:" 
	mkdir -p $(OBJDIR)/
	mkdir -p $(BINDIR)/
	$(CC) -T linker.ld -o $@ -ffreestanding -O2 -nostdlib $(OBJS) -lgcc -Xlinker -Map=kernel.map
	
	echo '\012\012 /*********************** ELF INFOR ***********************/' >$(BINDIR)/list.s
	printf '\012' >>$(BINDIR)/list.s
	readelf $(BINDIR)/myos.bin -a >> $(BINDIR)/list.s
	objdump -S -d -mi386 $(BINDIR)/myos.bin >>$(BINDIR)/list.s
	echo '\012\012 /*********************** HEX ***********************/' >>$(BINDIR)/list.s
	printf '\012' >>$(BINDIR)/list.s
	xxd $(BINDIR)/myos.bin >>$(BINDIR)/list.s
clean:
	@echo
	@echo
	@echo Clean:
	rm -f $(OBJDIR)/*.o
	rm -f myos.bin
run:
	qemu-system-i386 -kernel $(BINDIR)/myos.bin 
debug: 
	qemu-system-i386 -kernel $(BINDIR)/myos.bin -s -S
serialdebug:
	qemu-system-i386 -kernel bin/myos.bin -serial telnet:127.0.0.1:4444,server,nowait | telnet 127.0.0.1 4444
