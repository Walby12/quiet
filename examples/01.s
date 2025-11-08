.text
.globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	leaq var_0(%rip), %rdx
	leaq STR_LIT_0_1(%rip), %rsi
	leaq fmt_2(%rip), %rdi
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
var_0:
	.ascii "world"
	.byte 0
/* end data */

.data
.balign 8
STR_LIT_0_1:
	.ascii "Hello"
	.byte 0
/* end data */

.data
.balign 8
fmt_2:
	.ascii "%s %s\n"
	.byte 0
/* end data */

.section .note.GNU-stack,"",@progbits
