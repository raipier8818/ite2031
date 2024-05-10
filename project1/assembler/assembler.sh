# compile the assembler
make clean && make

# check compile error
if [ $? -ne 0 ]; then
    echo "Compile error"
    exit 1
fi


# How to use
# 1. Create test directory.
# 2. Put all the test files in the test directory(*.as, *.mc.correct). If the test case raises compile error, then *.mc file is not needed.
# 3. Run the script.

if [ ! -d $test_dir ]; then
    echo "Test directory does not exist"
    exit 1
fi

# program name
program="assembler"

# test directory
test_dir="./test"

# test file format
input_format="as"

# result file format
expected_file_format="mc.correct"
result_format="mc"

# log file format when the program raises compile error
log_format="log"

# diff file format to check difference between the result and the expected output
diff_format="diff"


num_of_cases=$(find $test_dir -name "*.$input_format" | wc -l)
num_of_success=0
num_of_fail=0

# remove all the result files
find $test_dir -name "*.$result_format" -exec rm {} \;
find $test_dir -name "*.$log_format" -exec rm {} \;
find $test_dir -name "*.$diff_format" -exec rm {} \;

echo "Running tests..."
# find all the .as files in the test directory
for file in $(find $test_dir -name "*.$input_format" | sort -V); do
    # get the file name without the extension
    filename=$(basename -- "$file")
    filename="${filename%.*}"

    # run the program on the file and save the result in a log file
    ./$program $file $test_dir/$filename.$result_format >> $test_dir/$filename.$log_format

    # check if the program ran successfully
    if [ $? -eq 1 ]; then
        # check the result file exist
        if [ ! -f $test_dir/$filename.$expected_file_format ]; then
            echo "\033[32m$filename PASSED\033[0m \t ($(cat $test_dir/$filename.$log_format))"
            num_of_success=$((num_of_success+1))
        else
            echo "\033[31m$filename FAILED \t ($test_dir/$filename.$expected_file_format exists)\033[0m"
            num_of_fail=$((num_of_fail+1))
        fi
    else
        # check if the expected output file exists
        if [ ! -f $test_dir/$filename.$expected_file_format ]; then
            echo "$filename.$expected_file_format NOT EXIST"
            continue
        fi

        # compare the output with the expected output
        diff $test_dir/$filename.$expected_file_format $test_dir/$filename.$result_format >> $test_dir/$filename.$diff_format

        # if the files are the same, print "PASSED", otherwise print "FAILED"
        if [ $? -eq 0 ]; then
            echo "\033[32m$filename PASSED\033[0m"
            num_of_success=$((num_of_success+1))
        else
            echo "\033[31m$filename FAILED\033[0m"
            num_of_fail=$((num_of_fail+1))
        fi
    fi
done

# remove the result file, if you want to keep it, comment this line
find $test_dir -name "*.$result_format" -exec rm {} \;
find $test_dir -name "*.$log_format" -exec rm {} \;
find $test_dir -name "*.$diff_format" -exec rm {} \;

echo "Number of test cases: \t$num_of_cases"
echo "Number of success: \t$num_of_success"
echo "Number of fail: \t$num_of_fail"
echo "Not tested: \t\t$(($num_of_cases-$num_of_success-$num_of_fail))"
