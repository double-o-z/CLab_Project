; file ps.as

	.define sz = 2
MAIN:   mov r3, LIST[sz]
LOOP:   jmp L1
	.entry STR
mcr m_mcr
cmp r3, #sz
bne END
endmcr
prn L4
prn #-5
mov STR[5]   , STR[2]
sub r1, r4
	m_mcr

  L1: 	inc L3
bne LOOP
prn L3
END: hlt

	.define len = 4
prn L4
STR: 	.string "abcdef"
LIST: 	.data 6, -9, len
K: 		.data 155

	.extern L3
	.extern L4


