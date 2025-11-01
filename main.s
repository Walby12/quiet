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
t0:
	.ascii "Wow"
	.byte 0
/* end data */

.data
.align 8
t1:
	.ascii "Wow"
	.byte 0
/* end data */

