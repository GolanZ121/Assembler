.entry cmp
.extern 100
.extern T
MAIN: mov LENGTH ,MAIN
LOOP: jmp L1 , @r1
prn 
LABEL: prn 4 
mcro LABEL
        prn 4
endmcro
LABEL
.data 5 4 , 6
.data +654,12 123,321
bne 1
50: stop
T: .string "a"
longerthan31charactersisntalabel: stop
sub @r1 , 4
bne 100
L1: inc 100
.entry LOOP   22
jmp W  @r1
END: stop , @r2
STR: .string "abcdef
LENGTH: .data 50789,6
mov: .data 22
.extern
just jibrish
.data 4.5

