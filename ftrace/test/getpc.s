	.file	"pc.c"
	.section	".text"
	.align 4
	.global get_retpc
	.type	get_retpc, #function
	.proc	04
get_retpc:
	!#PROLOGUE# 0
	save	%sp, -120, %sp
	!#PROLOGUE# 1

	mov %i7, %i0
	return	%i7+8
	nop

	ta 1

	.size	get_retpc, .-get_retpc
	.ident	"GCC: (GNU) 3.4.1"
