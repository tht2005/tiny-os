CC = riscv64-unknown-elf-gcc
LD = riscv64-unknown-elf-ld
AS = riscv64-unknown-elf-as
AR = riscv64-unknown-elf-ar

ARCH=rv64imafd_zicsr
# ARCH=rv64imafd_zicsr_zifencei
ABI=lp64d

CFLAGS  = -Wall -Wextra -ffreestanding -march=$(ARCH) -mabi=$(ABI) -I./include -I./libc/printf
CFLAGS += -mcmodel=medany
ASFLAGS = -march=$(ARCH) -mabi=$(ABI)

C_SRCS := $(shell find . -name '*.c')
S_SRCS := $(shell find . -name '*.S')

OBJS := $(C_SRCS:.c=.o) $(S_SRCS:.S=.o)
OBJS := $(patsubst ./%,target/%,$(OBJS))

TARGET := kernel.elf

default: CFLAGS += -O2
default: $(TARGET)

$(TARGET): $(OBJS)
	$(LD) -m elf64lriscv -T virt.lds -o $@ $^

target/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

target/%.o: %.S
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) -o $@ $<

# Ensure target dir exists first
$(OBJS): | target
target:
	mkdir -p target

run: default
	@echo "Ctrl-A C for QEMU console, then quit to exit"
	qemu-system-riscv64 -machine virt -cpu rv64,pmp=false -smp 1 -nographic -bios none -kernel $(TARGET)

debug: CFLAGS += -g -O0
debug: ASFLAGS += -g
debug: default
	qemu-system-riscv64 -machine virt -cpu rv64,pmp=false -smp 1 -s -S -nographic -bios none -kernel $(TARGET)

clean:
	rm -rf target $(TARGET)

.PHONY: clean run

