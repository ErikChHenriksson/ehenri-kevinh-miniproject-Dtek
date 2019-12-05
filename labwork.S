  # labwork.S
  # Written 2015-2017 by F Lundevall
  # Skeleton file for IS1200/IS1500 lab 1.
  # The macros PUSH and POP are in the public domain.
  # Please add your own code at the end of the file.

.global hexasc
.global delay
.global time2string

  #
  # Please keep the two macros PUSH and POP unchanged
  #
.macro	PUSH reg
	addi	$sp,$sp,-4
	sw	\reg,0($sp)
.endm

.macro	POP reg
	lw	\reg,0($sp)
	addi	$sp,$sp,4
.endm
  #
  # Please add your own code below this line
  #

.text

hexasc:
	and 	$v0, $a0, 0xf
	slti 	$t0, $v0, 0xa
	addi 	$v0, $v0, 48
	beq 	$t0, 1, skip_extra_add
	nop
	addi	$v0, $v0, 7
skip_extra_add:
	jr 	$ra
	nop


delay:
	sgt	$t0, $a0, $0
	beqz	$t0, end
	nop
	addi	$a0, $a0, -1
	
	li	$t1, 0
for_loop:
	beq	$t1, 3900, delay
	nop
	addi 	$t1, $t1, 1
	j for_loop
	nop

end:
	jr 	$ra
	nop
	

# $a0 output address
# $a1 16bits of NBCD encoded time
# No return value
time2string:
	PUSH 	$s0
	PUSH 	$s1
	PUSH	$s2
	PUSH	$ra			
	
	move 	$s0, $a0	# save memory address
	# Set up $s1
	addi	$s1, $0, 0x3a		
	sll	$s1, $s1, 16		
	# Set up $s2
	move	$s2, $0
	
	# Add first digit to $s1
	srl	$a0, $a1, 12
	jal 	hexasc
	nop
	or	$s1, $s1, $v0
	
	# Add second digit to $s1
	srl	$a0, $a1, 8
	jal 	hexasc
	nop
	sll	$v0, $v0, 8
	or	$s1, $s1, $v0	
	
	# Add third digit to $s1
	srl	$a0, $a1, 4
	jal 	hexasc
	nop
	sll	$v0, $v0, 24
	or	$s1, $s1, $v0	

	# Add fourth digit to $s2
	move	$a0, $a1	
	jal 	hexasc
	nop
	or	$s2, $s2, $v0	
	
	# Store string in memory
	sw	$s1, 0($s0)
	sw	$s2, 4($s0)
	
	POP	$ra
	POP	$s2
	POP	$s1
	POP $s0
	jr	$ra
	nop

