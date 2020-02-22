#!/bin/bash

PGM=lab0

EXIT_OK=0
EXIT_ARG=1
EXIT_BADIN=2
EXIT_BADOUT=3
EXIT_FAULT=4
SIG_SEGFAULT=139

let errors=0

#test stdin to stdout
str="hello world~\n"
out=`echo $str | ./$PGM`
if [ out != str ]
then
    echo $out
    echo "stdin to stdout wrong output"
    errors+=1
fi
if [ $? -ne $EXIT_OK ]
then
    echo "stdin to stdout fails"
    errors+=1
fi

#test if it causes and catches segfault correctly
./$PGM --segfault
if [ $? -ne $SIG_SEGFAULT ]
then
    let errors+=1
    echo "segfault fails."
fi
./$PGM --segfault --catch 2>STDERR
if [ $? -ne $EXIT_FAULT ]
then
    let errors+=1
    echo "segfault catch fails."
fi
if [ $errors -gt 0 ]
then
    echo "FAIL"
else
    echo "SUCCESS"
fi
