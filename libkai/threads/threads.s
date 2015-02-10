// text section
.section __TEXT,__text

.align 4, 0x90

// string with str.len set
str:
	.ascii "Hello world!\n"
	.set str.len, .-str

// exit syscall, takes exit number
.globl _kai_test_asm
_kai_test_asm:
	// system call 4, write
	// stops printing at 0 byte.
	mov $0x2000004, %rax
	mov $1, %rdi // stdout
	movq str@GOTPCREL(%rip), %rsi // The value to print
	movq $str.len, %rdx // the size of the value to print
	syscall

	mov $0, %rax
	retq
