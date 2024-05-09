	noop				program of calculating sum of 0 to n
	lw	0	1	n	load n
	lw	0	2	result	load result
	lw	0	3	neg	load -1
loop	add	1	2	2	add n to result
	add	1	3	1	add n to -1
	beq	0	1	1	if n == 0, done
	beq	0	0	loop	go to loop
	sw	0	2	result	store result
	halt
n	.fill	10
result	.fill	0
neg	.fill	-1
