.text
.globl main
main:
	pushq %rbp
	movq %rsp, %rbp
	movl $0, %eax
	leave
	ret
/* end function main */

.data
.align 8
v0:
	.ascii "Hello world"
	.byte 0
/* end data */

