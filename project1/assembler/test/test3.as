	lw	0	2	n
	lw	0	3	one
	lw	0	4	neg
loop	add	2	4	2	$2 -= 1
	beq	0	2	done
	add	0	2	5	$5 = $2
	add	0	3	1	$1 = $3
mult	add	1	3	3	$3 = $1 + #3
	add	4	5	5	$5 -= 1
	beq	0	5	loop	if $5 == 0, jump loop
	beq	0	0	mult	jump mult
done	sw	0	3	result
	halt
n	.fill	6
one	.fill	1
neg	.fill	-1
result	.fill	0
