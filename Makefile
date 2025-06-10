CC = riscv64-unknown-elf-gcc
LD = riscv64-unknown-elf-ld
AS = riscv64-unknown-elf-as
AR = riscv64-unknown-elf-ar

#ARCH=rv64imafd_zicsr
ARCH=rv64imafd_zicsr_zifencei
ABI=lp64d

CFLAGS  = -Wall -Wextra -ffreestanding -march=$(ARCH) -mabi=$(ABI) -I./include -I./libc/printf
CFLAGS += -g3 -O0
CFLAGS += -mcmodel=medany
#CFLAGS += -DPRINTF_DISABLE_SUPPORT_FLOAT -DPRINTF_DISABLE_SUPPORT_LONG_LONG
ASFLAGS = -march=$(ARCH) -mabi=$(ABI) -g

C_SRCS := $(shell find . -name '*.c')
S_SRCS := $(shell find . -name '*.S')

OBJS := $(C_SRCS:.c=.o) $(S_SRCS:.S=.o)
OBJS := $(patsubst ./%,target/%,$(OBJS))

TARGET := kernel.elf

default: $(TARGET)

$(TARGET): $(OBJS)
	$(LD) -m elf64lriscv -T virt.lds -o $@ $^ -g

debug: default
	qemu-system-riscv64 -machine virt -nographic -bios $(TARGET) -serial mon:stdio -s -S

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

# libcore.a: $(OBJS)
# 	$(AR) rcs $@ $^

run:
	@echo "Ctrl-A C for QEMU console, then quit to exit"
	qemu-system-riscv64 -nographic -serial mon:stdio -machine virt -bios $(TARGET)

clean:
	rm -rf target $(TARGET)

.PHONY: clean run

