CC = riscv64-unknown-elf-gcc
OBJCOPY = riscv64-unknown-elf-objcopy
LD = riscv64-unknown-elf-ld

CFLAGS = -nostdlib -ffreestanding -mcmodel=medany

all: kernel.elf

user.elf: user.S user.ld
	$(CC) $(CFLAGS) -T user.ld -o $@ user.S

userelf.o: user.elf
	$(LD) -r -b binary $< -o $@

kernel.elf: entry.S trap.S main.c trap.c uart.c userelf.o
	$(CC) $(CFLAGS) -T kernel.ld entry.S trap.S main.c trap.c uart.c userelf.o -o $@

clean:
	rm -f user.elf user.bin kernel.elf