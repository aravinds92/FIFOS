CC = gcc
CFLAGS = -m32 -fno-builtin -fno-stack-protector -fno-strict-aliasing -fno-delete-null-pointer-checks -nostdinc -I. -g -Wall -std=c99
CPPFLAGS = -Wa,--32 -MMD
OBJS = boot.o fifos.o print.o scheduler.o
PROGS = FIFOS
MNT_POINT=/mnt/C/boot/
INSTALL = install

all: $(PROGS) $(INSTALL)

FIFOS: $(OBJS)
	$(LD) -m elf_i386 -T memos.ld -o $@ $^

%: %.c 

install: $(all)	
	mount /root/FIFOS/fifos.img /mnt/C -text2 -o loop,offset=32256
	cp $(PROGS) $(MNT_POINT)
	sync
	umount /mnt/C
	qemu-system-i386 -kernel FIFOS


	
clean:
	-rm *.o *.d $(PROGS) *~
	rm -f /mnt/C/boot/$(PROGS)
	
		
-include *.d
