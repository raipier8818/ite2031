	noop				nor test
	lw	0	1	n	load reg1 with 7 (symbolic address)
	nor	0	1	1	reg1 = -8
	sw	0	1	n	store -8 at n
	halt
n	.fill	7
