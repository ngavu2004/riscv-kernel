CC = riscv64-unknown-elf-gcc
OBJCOPY = riscv64-unknown-elf-objcopy
LD = riscv64-unknown-elf-ld

USER_DIR := user_programs

EXPERIMENT_DIR := experiment_monitor
KERNEL_C := $(wildcard *.c)
KERNEL_H := $(wildcard *.h)
LIB_C := $(wildcard my_lib/*.c)
LIB_H := $(wildcard my_lib/*.h)
KERNEL_ASM := boot.S kernel_entry.S trap.S restore_context_asm.S
USER_OBJS := user1elf.o user2elf.o uart_userelf.o
EXPERIMENT_OBJS := observed_programelf.o monitorelf.o

KERNEL_CFLAGS = -g -nostdlib -ffreestanding -fstack-protector-strong -mcmodel=medany
USER_CFLAGS = -g -nostdlib -ffreestanding -fno-stack-protector -mcmodel=medany

all: kernel.elf

user1.elf: $(USER_DIR)/user1.S $(USER_DIR)/user1.ld
	$(CC) $(USER_CFLAGS) -T $(USER_DIR)/user1.ld -o $@ $(USER_DIR)/user1.S

user2.elf: $(USER_DIR)/user2.S $(USER_DIR)/user2.ld
	$(CC) $(USER_CFLAGS) -T $(USER_DIR)/user2.ld -o $@ $(USER_DIR)/user2.S

uart_user.elf: $(USER_DIR)/uart_entry.S $(USER_DIR)/uart_user.c $(USER_DIR)/uart_user.h $(USER_DIR)/uart_user.ld
	$(CC) $(USER_CFLAGS) -I$(USER_DIR) -T $(USER_DIR)/uart_user.ld -o $@ $(USER_DIR)/uart_entry.S $(USER_DIR)/uart_user.c

observed_program.elf: $(EXPERIMENT_DIR)/observed_program.S $(EXPERIMENT_DIR)/observed_program.c $(EXPERIMENT_DIR)/observed_program.ld
	$(CC) $(USER_CFLAGS) -T $(EXPERIMENT_DIR)/observed_program.ld -o $@ $(EXPERIMENT_DIR)/observed_program.S $(EXPERIMENT_DIR)/observed_program.c

monitor.elf: $(EXPERIMENT_DIR)/monitor.S $(EXPERIMENT_DIR)/monitor.c $(EXPERIMENT_DIR)/monitor.ld
	$(CC) $(USER_CFLAGS) -T $(EXPERIMENT_DIR)/monitor.ld -o $@ \
		$(EXPERIMENT_DIR)/monitor.S $(EXPERIMENT_DIR)/monitor.c

user1elf.o: user1.elf
	$(LD) -r -b binary $< -o $@

user2elf.o: user2.elf
	$(LD) -r -b binary $< -o $@

uart_userelf.o: uart_user.elf
	$(LD) -r -b binary $< -o $@

observed_programelf.o: observed_program.elf
	$(LD) -r -b binary $< -o $@

monitorelf.o: monitor.elf
	$(LD) -r -b binary $< -o $@

kernel.elf: $(KERNEL_ASM) $(KERNEL_C) $(LIB_C) $(LIB_H) $(KERNEL_H) kernel.ld $(USER_OBJS) $(EXPERIMENT_OBJS)
	$(CC) $(KERNEL_CFLAGS) -T kernel.ld  $(KERNEL_ASM) $(KERNEL_C) $(LIB_C) $(EXPERIMENT_OBJS) -o $@

run: kernel.elf
	qemu-system-riscv64 -machine virt -smp 2 -bios none -kernel $< -nographic

debug: kernel.elf
	qemu-system-riscv64 -machine virt -smp 2 -bios none -kernel $< -nographic -S -s

gdb:
	gdb-multiarch kernel.elf -ex "target remote localhost:1234"

clean:
	rm -f user1.elf user2.elf uart_user.elf observed_program.elf monitor.elf $(USER_OBJS) kernel.elf