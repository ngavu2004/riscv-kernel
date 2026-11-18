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

USER_UART ?= 0 # if i want the uart program to be in the user space
DEBUG ?= 0
PLATFORM ?= qemu
PLATFORM_CFLAGS :=
$(info Platform: $(PLATFORM))
ifeq ($(PLATFORM),qemu)
    KERNEL_BASE := 0x80000000
    UART_START := 0x10000000
    UART_SIZE := 0x100
	USER_BASE_ADDRESS := 0x80400000
	PAGE_ALLOC_START := 0x80400000
	PAGE_ALLOC_END := 0x80800000
	MAX_HART_NUM := 2
	PLATFORM_CFLAGS += -DPLATFORM_QEMU
else ifeq ($(PLATFORM),fpga)
    KERNEL_BASE := 0x40000000
    UART_START := 0x000C0000
    UART_SIZE := 0x1000
	USER_BASE_ADDRESS := 0x40400000
	PAGE_ALLOC_START := 0x40400000
	PAGE_ALLOC_END := 0x40800000
	MAX_HART_NUM := 1
	PLATFORM_CFLAGS += -DPLATFORM_FPGA
else
    $(error Unsupported platform)
endif

PLATFORM_CFLAGS += -DUART0=$(UART_START) -DUART0_SIZE=$(UART_SIZE) -DMAX_HART_NUM=$(MAX_HART_NUM) \
	-DUSER_BASE_ADDRESS=$(USER_BASE_ADDRESS) -DMAIN_HARTID=0 -DDEBUG=$(DEBUG) \
	-DPAGE_ALLOC_START=$(PAGE_ALLOC_START) -DPAGE_ALLOC_END=$(PAGE_ALLOC_END) \
	-DKERNEL_BASE=$(KERNEL_BASE) -DUSER_UART=$(USER_UART)
ARCH_CFLAGS :=

ifeq ($(PLATFORM),fpga)
    ARCH_CFLAGS := -march=rv64ima_zicsr -mabi=lp64
endif

LDFLAGS = -T kernel.ld -Wl,--defsym=KERNEL_BASE=$(KERNEL_BASE)
KERNEL_CFLAGS = $(ARCH_CFLAGS) -g -nostdlib -ffreestanding -fstack-protector-strong -mcmodel=medany $(PLATFORM_CFLAGS)
USER_CFLAGS = $(ARCH_CFLAGS) -g -nostdlib -ffreestanding -fno-stack-protector -mcmodel=medany $(PLATFORM_CFLAGS)

PROGRAMS ?= user1

# The hart I want for each program (if not they are default to be MAIN_HARTID)
PROGRAM_HART_user1 := MAIN_HARTID
PROGRAM_HART_observed_program := MAIN_HARTID
PROGRAM_HART_monitor := 1

PROGRAM_OBJS := $(addsuffix elf.o,$(PROGRAMS))
PROGRAM_ENTRIES := $(foreach p,$(PROGRAMS),{ _binary_$(subst .,_,$(p))_elf_start, $(PROGRAM_HART_$(p)) },)

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

.PHONY: FORCE
FORCE:

user_programs.generated.h: FORCE
	@{ \
		printf '%s\n' '#ifndef USER_PROGRAMS_GENERATED_H' '#define USER_PROGRAMS_GENERATED_H'; \
		$(foreach p,$(PROGRAMS),printf '%s\n' 'extern unsigned char _binary_$(subst .,_,$(p))_elf_start[] __attribute__((weak));';) \
		printf '%s\n' '#define USER_PROGRAM_TABLE { $(PROGRAM_ENTRIES) }' '#endif'; \
	} > $@

kernel.elf: $(KERNEL_ASM) $(KERNEL_C) $(LIB_C) $(LIB_H) $(KERNEL_H) kernel.ld $(USER_OBJS) $(EXPERIMENT_OBJS) $(PROGRAM_OBJS) user_programs.generated.h
	$(CC) $(KERNEL_CFLAGS) $(LDFLAGS)  $(KERNEL_ASM) $(KERNEL_C) $(LIB_C) $(EXPERIMENT_OBJS) $(PROGRAM_OBJS) -o $@

run: kernel.elf
	qemu-system-riscv64 -machine virt -bios none -kernel $< -nographic

debug: kernel.elf
	qemu-system-riscv64 -machine virt -smp 2 -bios none -kernel $< -nographic -S -s

gdb:
	gdb-multiarch kernel.elf -ex "target remote localhost:1234"

clean:
	rm -f user1.elf user2.elf uart_user.elf observed_program.elf monitor.elf $(USER_OBJS) kernel.elf