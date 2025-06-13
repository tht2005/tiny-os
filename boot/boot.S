.option norvc
.section .data

.section .text.init
.global _start
_start:
	csrr	t0, mhartid
	bnez	t0, 3f
	csrw	satp, zero
.option push
.option norelax
	la		gp, _global_pointer
.option pop

	la		a0, _bss_start
	la		a1, _bss_end
	bgeu	a0, a1, 2f
1:

	sw		zero, (a0)
	addi	a0, a0, 4
	bltu	a0, a1, 1b
2:

	la		sp, _stack_end
	# li		t0, (0b11 << 11) | (1 << 7) | (1 << 3)
	li		t0, (0b11 << 11)
	csrs	mstatus, t0
	la		t1, kinit
	csrw	mepc, t1
	la		t2, m_trap_vector
	csrw	mtvec, t2
	csrw	mie, zero
	# li		t3, (1 << 3) | (1 << 7) | (1 << 11)
	# csrw	mie, t3
	la		ra, supervisor_entry
	mret
supervisor_entry:
	li		t0, (1 << 8) | (1 << 5) | (1 << 19)
	csrw	sstatus, t0
	la		t1, kmain
	csrw	sepc, t1
	li		t2, (1 << 1) | (1 << 5) | (1 << 9)
	csrs	mideleg, t2
	csrw	sie, t2
	la		t3, s_trap_vector
	csrw	stvec, t3

	csrw	satp, a0
	sfence.vma

	sret
3:
	
4:
	wfi
	j		4b

