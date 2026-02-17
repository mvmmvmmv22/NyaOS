ARCH=i386
CC=i686-elf-gcc
LD=i686-elf-ld
AS=nasm

CFLAGS=-ffreestanding  -m32 -fno-pic -fno-stack-protector -nostdlib -nostdinc -g -O0
LDFLAGS=-T kernel/linker.ld -nostdlib

all: os.img

boot.bin:
	$(AS) -f bin boot/boot.asm -o boot.bin

kernel.elf:
	$(AS) -f elf32 kernel/kernel_entry.asm -o kernel_entry.o
	$(AS) -f elf32 kernel/isr.asm -o isr.o
	$(CC) $(CFLAGS) -c kernel/kernel.c -o kernel.o
	$(CC) $(CFLAGS) -c kernel/idt.c -o idt.o
	$(CC) $(CFLAGS) -c kernel/pit.c -o pit.o
	$(CC) $(CFLAGS) -c kernel/shell.c -o shell.o
	$(CC) $(CFLAGS) -c kernel/keyboard.c -o keyboard.o
	$(CC) $(CFLAGS) -c kernel/ramfs.c -o ramfs.o
	$(CC) $(CFLAGS) -c kernel/scheduler.c -o scheduler.o
	$(CC) $(CFLAGS) -c kernel/tasks.c -o tasks.o
	$(CC) $(CFLAGS) -c kernel/exceptions.c -o exceptions.o
	$(CC) $(CFLAGS) -c kernel/kb_buf.c -o kb_buf.o
	$(LD) -T kernel/linker.ld -o kernel.elf kernel_entry.o isr.o kernel.o idt.o pit.o keyboard.o shell.o ramfs.o scheduler.o tasks.o exceptions.o kb_buf.o

kernel.bin: kernel.elf
	objcopy -O binary kernel.elf kernel.bin

os.img: boot.bin kernel.bin
	dd if=/dev/zero of=os.img bs=512 count=2880
	dd if=boot.bin of=os.img conv=notrunc
	dd if=kernel.bin of=os.img bs=512 seek=1 conv=notrunc

run:
	qemu-system-i386 -drive format=raw,file=os.img

clean:
	rm -f *.bin *.elf *.o os.img