; file ps.as

mcr m_mcr_bisly
		jmp L3
endmcr
.define sz = 2
MAIN:   mov r3, LIST[sz]
LOOP:   jmp L1
mcr m_mcr
		cmp r3, #sz
		bne END
endmcr
		prn #-5
		mov STR[5]   , STR[2]
		m_mcr_bisly
mcr m_mcr_bamba
		jmp L2
endmcr
		sub r1, r4
m_mcr
m_mcr_bisly

L1: 	inc L3
		bne LOOP
m_mcr_bamba

.define len = 4
m_mcr_bamba

END: hlt

STR: 	.string "abcdef"
LIST: 	.data 6, -9, len
K: 		.data 22

.extern L3


 	
