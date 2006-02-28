	.file	"pc.c"
	.section	".rodata"
	.align 8
.LLC0:
	.asciz	"@pc == 0x%08lx\n"
	.align 8
.LLC1:
	.asciz	"second printf"
	.section	".text"
	.align 4
	.global main
	.type	main, #function
	.proc	04
main:
	!#PROLOGUE# 0
	save	%sp, -112, %sp
	!#PROLOGUE# 1
	call	get_retpc, 0
	mov	0, %i0
	mov	%o0, %o1
	sethi	%hi(.LLC0), %o0
	call	printf, 0
	or	%o0, %lo(.LLC0), %o0
	call	get_retpc, 0
	 nop
	mov	%o0, %l0
	sethi	%hi(.LLC1), %o0
	call	puts, 0
	or	%o0, %lo(.LLC1), %o0
	call	%l0, 0
	 nop
	return	%i7+8
	nop
	.size	main, .-main
	.ident	"GCC: (GNU) 3.4.1"
