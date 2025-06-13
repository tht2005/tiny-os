.section .text
.global m_trap_vector
m_trap_vector:
	wfi
	j		m_trap_vector

.global s_trap_vector
s_trap_vector:
	wfi
	j		s_trap_vector
