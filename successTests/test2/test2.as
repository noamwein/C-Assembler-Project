.entry Next
.extern wNumber
		;this is a check
STR: .asciz     "aBcd"



LOOP: ori $9,-5,$2
	la val1
	jmp Next
Next: move $20,$4
END: stop

LIST: .dh 60, 53
	bgt $4,$2,END
	sw $0,4,$10
	bne $31,$9, LOOP
	call val1
	jmp $4
	la wNumber
.extern val1
	.dh 27056
