	lw	0	1	ptr
	jalr	1	2
	sw	0	3	result
	halt
	nor	0	0	3
	jalr	2	1
ptr	.fill	4
result	.fill	0
