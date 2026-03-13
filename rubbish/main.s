	.file	"main.c"
	.text
	.section .rdata,"dr"
.LC0:
	.ascii "test.dat\0"
.LC1:
	.ascii "commit failed\12\0"
	.text
	.globl	main
	.def	main;	.scl	2;	.type	32;	.endef
	.seh_proc	main
main:
	pushq	%rbp
	.seh_pushreg	%rbp
	pushq	%rdi
	.seh_pushreg	%rdi
	subq	$168, %rsp
	.seh_stackalloc	168
	leaq	160(%rsp), %rbp
	.seh_setframe	%rbp, 160
	.seh_endprologue
	call	__main
	leaq	-128(%rbp), %rdx
	movl	$0, %eax
	movl	$16, %ecx
	movq	%rdx, %rdi
	rep stosq
	leaq	.LC0(%rip), %rdx
	leaq	-128(%rbp), %rax
	movl	$0, %r9d
	movl	$16, %r8d
	movq	%rax, %rcx
	call	KVDAT_init
	leaq	-128(%rbp), %rax
	movq	%rax, %rcx
	call	KVDAT_commit
	xorl	$1, %eax
	testb	%al, %al
	je	.L2
	movl	$2, %ecx
	movq	__imp___acrt_iob_func(%rip), %rax
	call	*%rax
	movq	%rax, %rdx
	leaq	.LC1(%rip), %rax
	movq	%rdx, %r9
	movl	$14, %r8d
	movl	$1, %edx
	movq	%rax, %rcx
	call	fwrite
.L2:
	leaq	-128(%rbp), %rax
	movq	%rax, %rcx
	call	KVDAT_deinit
	movl	$0, %eax
	addq	$168, %rsp
	popq	%rdi
	popq	%rbp
	ret
	.seh_endproc
	.def	__main;	.scl	2;	.type	32;	.endef
	.ident	"GCC: (GNU) 15.2.0"
	.def	KVDAT_init;	.scl	2;	.type	32;	.endef
	.def	KVDAT_commit;	.scl	2;	.type	32;	.endef
	.def	fwrite;	.scl	2;	.type	32;	.endef
	.def	KVDAT_deinit;	.scl	2;	.type	32;	.endef
