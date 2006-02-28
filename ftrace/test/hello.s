	.file	"hello.c"
	.section	".text"
	.align 4
	.type	writestr, #function
	.proc	020
writestr:
	!#PROLOGUE# 0
	save	%sp, -120, %sp
	!#PROLOGUE# 1
	st	%i0, [%fp+68]
	st	%g0, [%fp-20]
.LL2:
	ld	[%fp+68], %o5
	ld	[%fp-20], %g1
	add	%o5, %g1, %g1
	ldub	[%g1], %g1
	sll	%g1, 24, %g1
	sra	%g1, 24, %g1
	cmp	%g1, 0
	be	.LL3
	nop
	ld	[%fp-20], %g1
	add	%g1, 1, %g1
	st	%g1, [%fp-20]
	b	.LL2
	 nop
.LL3:
	mov	1, %o0
	ld	[%fp+68], %o1
	ld	[%fp-20], %o2
	call	write, 0
	 nop
	nop
	ret
	restore
	.size	writestr, .-writestr
	.section	".rodata"
	.align 8
.LLC0:
	.asciz	"hello\n"
	.section	".text"
	.align 4
	.global main
	.type	main, #function
	.proc	04
main:
	!#PROLOGUE# 0
	save	%sp, -112, %sp
	!#PROLOGUE# 1
	st	%i0, [%fp+68]
	st	%i1, [%fp+72]
	sethi	%hi(.LLC0), %g1
	or	%g1, %lo(.LLC0), %o0
	call	writestr, 0
	 nop
	mov	0, %g1
	mov	%g1, %i0
	ret
	restore
	.size	main, .-main
	.ident	"GCC: (GNU) 3.4.1"
