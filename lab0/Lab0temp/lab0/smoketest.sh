#!/bin/bash

PGM=lab0

EXIT_OK=0
EXIT_ARG=1
EXIT_BADIN=2
EXIT_BADOUT=3
EXIT_FAULT=4
SIG_SEGFAULT=139

let errors=0

#base test --input to --output
echo "hello world~\n" > in.txt
./lab0 --input=in.txt --output=out.txt &> /dev/null
if [ $? -ne $EXIT_OK ]
then
    let errors+=1
    echo "stdin to stdout fails"
fi

#compare the content of input and output files
cmp in.txt out.txt
if [ $? -ne 0 ]
then
    let errors+=1
    echo "error: copying from input file to output file: comparison failed"
fi
rm -f in.txt out.txt

#test invalid option
./$PGM --bogus &> /dev/null
if [ $? -ne $EXIT_ARG ]
then
    let errors+=1
    echo "error: bad argument should return 1"
fi
./$PGM --input &> /dev/null
if [ $? -ne $EXIT_ARG ]
then
    let errors+=1
    echo "error: bad argument should return 1"
fi

#test read permission denied for input file
echo "Some text in here\n" > in.txt
chmod u-r in.txt
./$PGM --input=in.txt &> /dev/null
if [ $? -ne $EXIT_BADIN ]
then
    let errors+=1
    echo "error: permission should deny for input file and return 2."
fi
rm -f in.txt

#test non-existed file for input file
./$PGM --input=fileNotExisted.txt &> /dev/null
if [ $? -ne $EXIT_BADIN ]
then
    let errors+=1
    echo "error: input file does not exist and should return 2."
fi

#test write permission denied for output file
touch out.txt
chmod u-w out.txt
echo "Hello!" | ./$PGM --output=out.txt &> /dev/null
if [ $? -ne $EXIT_BADOUT ]
then
    let errors+=1
    echo "error: permission should deny for output file and return 3."
fi
rm -f out.txt

#test non-existed output file
f="fileNotExisted.txt"
echo "Hello!" | ./$PGM --output=$f &> /dev/null
if [ $? -ne $EXIT_OK ]
then
    let errors+=1
    echo "error: non-existed file should be automatically created and return 0."
fi
if [ ! -s $f ]
then
    let errors+=1
    echo "error: non-existed file should have been written text by program."
fi
rm $f


#test if it causes and catches segfault correctly
./$PGM --segfault &> /dev/null
if [ $? -ne $SIG_SEGFAULT ]
then
    let errors+=1
    echo "error. segfault causation fails." 1>&2
fi
./$PGM --segfault --catch &> /dev/null
if [ $? -ne $EXIT_FAULT ]
then
    let errors+=1
    echo "error. segfault catch fails." 1>&2
fi

#test if segfault raised and exited before stdin to stdout
echo "some text.\n " | ./$PGM --output=out.txt --segfault &> /dev/null
if [ $? -ne $SIG_SEGFAULT ]
then
    let errors+=1
    echo "error. segfault causation fails." 1>&2
fi
if [ -s "out.txt" ]
then
    let errors+=1
    rm -f out.txt
    echo "error. output file should be empty when there's a segfault raised" 1>&2
fi
rm -f out.txt
#Check the result of all the tests
if [ $errors -gt 0 ]
then
    echo "FAIL"
else
    echo "SUCCESS. All tests pass."
fi
