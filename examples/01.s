.text
.globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	leaq fmt_0(%rip), %rdi
	movl $0, %eax
	callq printf
	movl $0, %eax
	leave
	ret
.type main, @function
.size main, .-main
/* end function main */

.data
.balign 8
fmt_0:
	.ascii "Hello world\n"
	.byte 0
/* end data */

.section .note.GNU-stack,"",@progbits
