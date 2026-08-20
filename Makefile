CC = riscv64-unknown-elf-gcc
OBJCOPY = riscv64-unknown-elf-objcopy
LD = riscv64-unknown-elf-ld

CFLAGS = -nostdlib -ffreestanding -mcmodel=medany

all: kernel.elf

user.elf: user.S user.ld
	$(CC) $(CFLAGS) -T user.ld -o $@ user.S

user.bin: user.elf
	$(OBJCOPY) -O binary $< $@

userbin.o: user.bin
	$(LD) -r -b binary $< -o $@

kernel.elf: entry.S trap.S main.c trap.c uart.c userbin.o
	$(CC) $(CFLAGS) -T kernel.ld entry.S trap.S main.c trap.c uart.c userbin.o -o $@

clean:
	rm -f user.elf user.bin userbin.o kernel.elf