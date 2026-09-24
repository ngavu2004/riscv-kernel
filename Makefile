CC = riscv64-unknown-elf-gcc
OBJCOPY = riscv64-unknown-elf-objcopy
LD = riscv64-unknown-elf-ld

USER_DIR := user_programs

KERNEL_C := $(wildcard *.c)
KERNEL_H := $(wildcard *.h)
KERNEL_ASM := entry.S trap.S

KERNEL_CFLAGS = -nostdlib -ffreestanding -fstack-protector-strong -mcmodel=medany
USER_CFLAGS = -nostdlib -ffreestanding -fno-stack-protector -mcmodel=medany

all: kernel.elf

user1.elf: $(USER_DIR)/user1.S $(USER_DIR)/user1.ld
	$(CC) $(USER_CFLAGS) -T $(USER_DIR)/user1.ld -o $@ $(USER_DIR)/user1.S

user2.elf: $(USER_DIR)/user2.S $(USER_DIR)/user2.ld
	$(CC) $(USER_CFLAGS) -T $(USER_DIR)/user2.ld -o $@ $(USER_DIR)/user2.S

uart_user.elf: $(USER_DIR)/uart_user.c $(USER_DIR)/uart_user.h $(USER_DIR)/uart_user.ld
	$(CC) $(USER_CFLAGS) -I $(USER_DIR) -T $(USER_DIR)/uart_user.ld -o $@ $(USER_DIR)/uart_user.c

user1elf.o: user1.elf
	$(LD) -r -b binary $< -o $@

user2elf.o: user2.elf
	$(LD) -r -b binary $< -o $@

uart_userelf.o: uart_user.elf
	$(LD) -r -b binary $< -o $@

kernel.elf: $(KERNEL_ASM) $(KERNEL_C) $(KERNEL_H) user1elf.o user2elf.o uart_userelf.o
	$(CC) $(KERNEL_CFLAGS) -T kernel.ld entry.S trap.S main.c trap.c uart.c stack_smash_protector.c message.c elf.c process.c pmp.c user1elf.o user2elf.o uart_userelf.o -o $@

run: kernel.elf
	qemu-system-riscv64 -machine virt -bios none -kernel $< -nographic

debug: kernel.elf
	qemu-system-riscv64 -machine virt -bios none -kernel $< -nographic -S -s

clean:
	rm -f user1.elf user2.elf user1elf.o user2elf.o kernel.elf