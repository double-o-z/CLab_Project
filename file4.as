; file ps.as
.entry LIST
.extern W

    ;
;
;
   ;
.define sz = 1
mcr m_mcr
cmp r3, L3
bne END
endmcr
MAIN: mov r3, LIST[sz]
LOOP: jmp W
prn #-5
mov STR[5], STR[2]
sub r1, r4
cmp K, #sz
m_mcr
m_mcr
bne W
L1: inc L3
.entry LOOP
    .entry LOOP
bne LOOP
END: hlt
.define len = 4
STR: .string "abcdef"
LIST: .data 6, -9, len
K: .data 22
.extern L3
