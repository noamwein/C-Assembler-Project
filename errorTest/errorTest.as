;This doesn't include all of the errors that the assembler can detect,
;but it shows a wide range of the main ones that can be detected during the first pass.
	;Label related errors.
1MAIN: add $1, $2, $3
ADD: add $1, $2, $3
ADD: add $1, $2, $3
add: add $1, $2, $3
	;Parsing related errors.
add $1 $2, $3
add $1, ,$2, $3
CHECK add $1, $2, $3
CHECK:add $1, $2, $3
CHECK : add $1, $2, $3
	;Command related errors.
add $1, $2
add $1, $2, $3, $4
add1 $1, $2, $3
add $34, $2, $3
add $1, $2, 3
addi $9, 50000, $4
jmp $33
stop $1
	;Data related errors.
.asciz "12"34"
.asciz "1234", "1234"
.db 50000, 500000
.dh 999999, 9999999
.dw 2147483648, 99999999
