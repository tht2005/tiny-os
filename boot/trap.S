.altmacro
.set NUM_GP_REGS,	32
.set NUM_FP_REGS,	32
.set REG_SIZE,		8

.macro save_gp i, basereg=t6
	sd		x\i, ((\i)*REG_SIZE)(\basereg)
.endm
.macro load_gp i, basereg=t6
	ld		x\i, ((\i)*REG_SIZE)(\basereg)
.endm
.macro save_fp i, basereg=t6
	fsd		f\i, ((NUM_GP_REGS+(\i))*REG_SIZE)(\basereg)
.endm
.macro load_fp i, basereg=t6
	fld		f\i, ((NUM_GP_REGS+(\i))*REG_SIZE)(\basereg)
.endm

.option norvc

.section .text
.global m_trap_vector
.align 4
m_trap_vector:
	# store the old t6 before using it atomically
	csrrw	t6, mscratch, t6

	.set	i, 0
	.rept	31
		save_gp %i
		.set i, i + 1
	.endr

	# save the old t6
	mv		t5, t6
	csrr	t6, mscratch
	save_gp 31, t5

	csrw	mscratch, t5

	# in future might need to save_fp ?

	# uintptr_t m_trap (uintptr_t epc,
	# 			uintptr_t tval,
	# 			uintptr_t cause,
	# 			uintptr_t hart,
	# 			uintptr_t status,
	# 			TrapFrame *frame)
	csrr	a0, mepc
	csrr	a1, mtval
	csrr	a2, mcause
	csrr	a3, mhartid
	csrr	a4, mstatus
	csrr	a5, mscratch
	# la		t0,	_stack_end
	# ld		sp, 0(t0)
	la		sp, _stack_end
	call	m_trap

	# after mret set PC to the returned value
	csrw	mepc, a0

	# old mscratch
	csrr	t6, mscratch

	.set	i, 1
	.rept	31
		load_gp %i
		.set i, i + 1
	.endr

	mret

# void switch_to_user (uintptr_t frame_addr, uintptr_t mepc, uintptr_t satp);
.global switch_to_user
switch_to_user:
	csrw	mscratch, a0

	li		t0, (1 << 7) | (1 << 5)
	csrw	mstatus, t0
	csrw	mepc, a1
	csrw	satp, a2
	li		t1, 0xaaa
	csrw	mie, t1
	la		t2, m_trap_vector
	csrw	mtvec, t2

	sfence.vma

	# load register from context frame
	mv		t6, a0
	.set	i, 1
	.rept	31
		load_gp %i, t6
		.set	i, i + 1
	.endr

	mret

.global make_syscall
make_syscall:
	ecall
	ret

