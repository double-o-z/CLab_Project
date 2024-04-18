; file ps.as

	.define sz = 2
MAIN:   mov r3, LIST[sz]
LOOP:   jmp L1
	.entry STR
mcr m_mcr
cmp r3, #sz
bne END
endmcr
prn #-5
mov STR[5]   , STR[2]
sub r1, r4
	m_mcr

  L1: 	inc L3
bne LOOP

END: hlt

	.define len = 4

STR: 	.string "abcdef"
LIST: 	.data L2, -9, len
K: 		.data 155

	.extern L3


