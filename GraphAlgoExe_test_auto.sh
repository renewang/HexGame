#!/bin/bash 

echo "Running unit tests:"

valgrind --tool=memcheck --dsymutil=yes --log-file=/tmp/valgrind-%p.log $1/GraphAlgoExe --gtest_output=xml:$1/test_details.xml | tee -a test.log

if [ "$?" -eq 0 ]
then
	echo $1 PASS;
else
	echo "ERROR in test $1; Here's failure message from $1/tests_details.xml";
	grep failure $1/test_details.xml;
	exit 1;
fi 
