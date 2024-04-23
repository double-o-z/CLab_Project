; file ps.as
    .entry LIST
    .extern W

;
;
;
;
.define sz = 1
MAIN: mov r3, LIST[sz]
LO:OP: jmp W
prn #-5
mov STR[5], STR[2]
sub r1, r4
cmp K, #sz
bne W
L1: inc L3
    .entry LO:OP
bne LO:OP
END: hlt
    .define len = 4
STR: .string "abcdef"
LIST: .data 6, -9, len
K: .data 22
    .extern L3
