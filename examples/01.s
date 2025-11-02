.text
.globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	leave
	ret
.type main, @function
.size main, .-main
/* end function main */

.data
.balign 8
x_0:
	.ascii "hello world"
	.byte 0
/* end data */

.section .note.GNU-stack,"",@progbits
