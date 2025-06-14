.option norvc
.section .data

.section .text.init
.global _start
_start:
	csrw	satp, zero

	csrr	t0, mhartid
	bnez	t0, 3f
.option push
.option norelax
	la		gp, _global_pointer
.option pop

	la		a0, _bss_start
	la		a1, _bss_end
	bgeu	a0, a1, 2f
1:

	sd		zero, (a0)
	addi	a0, a0, 8
	bltu	a0, a1, 1b
2:

	la		sp, _stack_end
	li		t0, (0b11 << 11)
	csrw	mstatus, t0
	la		t1, kinit
	csrw	mepc, t1
	csrw	mie, zero
	la		ra, 2f
	mret
2:
	li		t0, (0b01 << 11) | (1 << 7) | (1 << 5)
	csrw	mstatus, t0
	la		t2, m_trap_vector
	csrw	mtvec, t2
	la		t1, kmain
	csrw	mepc, t1
	li		t2, (1 << 11) | (1 << 7) | (1 << 3)
	csrw	mie, t2
	mret
3:
	la		sp, _stack_end
	csrr	a0, mhartid
	li		t0, 0x10000
	mul		t0, t0, a0
	sub		sp, sp, t0

	li		t0, (0b11 << 11) | (1 << 7)
	csrw	mstatus, t0
	li		t3, (1 << 3)
	csrw	mie, t3
	la		t1, kinit_hart
	csrw	mepc, t1
	la		t2, m_trap_vector
	csrw	mtvec, t2
	la		ra, 4f
	mret
4:
	wfi
	j		4b

