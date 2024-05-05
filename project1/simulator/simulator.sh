rm -rf output.txt diff.txt

make clean && make && ./simulator spec.mc >> output.txt

# compare output.txt and spec.out.correct
diff output.txt spec.out.correct >> diff.txt