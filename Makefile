CC = riscv32-unknown-linux-gnu-gcc
LD = riscv32-unknown-linux-gnu-ld
AS = riscv32-unknown-linux-gnu-as

ARCH=rv32imacf_zicsr
ABI=ilp32f

CFLAGS  = -Wall -Wextra -O0 -ffreestanding -march=$(ARCH) -mabi=$(ABI) -I./include -g
ASFLAGS = -march=$(ARCH) -mabi=$(ABI) -g

C_SRCS := $(shell find . -name '*.c')
S_SRCS := $(shell find . -name '*.S')
OBJS := $(C_SRCS:.c=.o) $(S_SRCS:.S=.o)
OBJS := $(patsubst ./%,target/%,$(OBJS))

default: kernel.elf

kernel.elf: $(OBJS)
	$(LD) -m elf32lriscv -T linker.ld -o $@ $^ -g

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

run:
	@echo "Ctrl-A C for QEMU console, then quit to exit"
	qemu-system-riscv32 -nographic -serial mon:stdio -machine virt -bios kernel.elf

clean:
	rm -rf target kernel.elf

