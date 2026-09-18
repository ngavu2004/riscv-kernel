CC = riscv64-unknown-elf-gcc
OBJCOPY = riscv64-unknown-elf-objcopy
LD = riscv64-unknown-elf-ld

CFLAGS = -nostdlib -ffreestanding -fstack-protector-strong -mcmodel=medany

all: kernel.elf

user1.elf: user1.S user1.ld
	$(CC) $(CFLAGS) -T user1.ld -o $@ user1.S

user2.elf: user2.S user2.ld
	$(CC) $(CFLAGS) -T user2.ld -o $@ user2.S

user1elf.o: user1.elf
	$(LD) -r -b binary $< -o $@

user2elf.o: user2.elf
	$(LD) -r -b binary $< -o $@

kernel.elf: entry.S trap.S main.c trap.c uart.c stack_smash_protector.c process.c elf.c csr.h user1elf.o user2elf.o
	$(CC) $(CFLAGS) -T kernel.ld entry.S trap.S main.c trap.c uart.c stack_smash_protector.c user1elf.o user2elf.o -o $@

run: kernel.elf
	qemu-system-riscv64 -machine virt -bios none -kernel $< -nographic

debug: kernel.elf
	qemu-system-riscv64 -machine virt -bios none -kernel $< -nographic -S -s

clean:
	rm -f user1.elf user2.elf user1elf.o user2elf.o kernel.elf