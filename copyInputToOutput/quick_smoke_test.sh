#!/bin/bash
#NAME: Jinbean Park
#EMAIL: keiserssose@gmail.com
#ID: 805330751

# 1. Check if the program seems to work or not.
`./lab0 --input="test1.txt" --output="outputTest1"`
if [ $? -eq 0 ]; then
    echo "No errors are encountered, program terminated successfully!"
else
    echo "Error occurred!"
fi

# 2. Check if the program supports the the required arguments.
`./lab0 --input= --output=`
if [ $? -eq 2 ]; then
    echo "Program successfully catched the case of not plugging file name."
else
    echo "Failed to catch the case of not plugging file name."
fi

#3. Check if the program successfully copies the standard input to standard output.
`diff test1.txt outputTest1`
if [ $? -eq 0 ]; then
    echo "Program successfully copied the standard input to standard output."
else
    echo "Failed to copy the standard input to standard output."
fi

#4. Check if the program catchs the case of not existing input file.
`./lab0 --input="notExistFile.txt" --output="outputTest2"`
if [ $? -eq 2 ]; then
    echo "Program successfully catched the case of not existing input file."
else
    echo "Failed to catch the case of not existing input file."
fi

#5. Check if the program catches the case of copying the standard input to output file not having write permisson.
`touch existFile`
`chmod 555 existFile`
`./lab0 --input="test1.txt" --output="existFile"`
if [ $? -eq 3 ]; then
    echo "Program successfully catched the case of copying the standard input to output file not having write permisson."
else
    echo "Failed to catch the case of copying the standard input to output file not having write permisson."
fi

#6. Check if the segfault and catch do works well or not.
`./lab0 --segfault --catch`
if [ $? -eq 4 ]; then
    echo "Sementaion fault was successfully caused and caught"
else
    echo "Failed to cause segmentaion fault and catch"
fi

# Deleted all files created.
`rm -f outputTest1 existFile`