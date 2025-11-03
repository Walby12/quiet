.text
.globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	movl $0, %eax
	leave
	ret
.type main, @function
.size main, .-main
/* end function main */

.data
.balign 8
x_0:
	.ascii "%s"
	.byte 0
/* end data */

.section .note.GNU-stack,"",@progbits
