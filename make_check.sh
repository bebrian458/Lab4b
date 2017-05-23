#!/bin/bash
#
# NAME:    Brian Be
# EMAIL:   bebrian458@gmail.com
# ID:  	204612203
#

num_errs=0

# Check for bad options - exit code
echo "checking for bad opts exit code..."
./lab4b --badopt &> errors.txt
if [[ $? -ne 1 ]]
then 
	num_errs+=1
	echo "Error: exit code was $? instead of 1"
else 
	echo "... OK"
fi

# Check for bad options - error message
echo "checking for bad opts error message..."
egrep "Invalid Arguments." errors.txt > /dev/null
if [[ $? -ne 0 ]]
then 
	num_errs+=1
	echo "Error: could not find error message"
else 
	echo "... OK"
fi
rm -f errors.txt

# Check for bad commands - exit code
echo "checking for bad cmd exit code..."
./lab4b --log=LOGFILE <<-EOF
badcomand
EOF
if [[ $? -ne 1 ]]
then
	num_errs+=1
	echo "Error: exit code was $? instead of 1"
else
	echo "... OK"
fi

# Check for bad commands - error message
echo "checking for bad cmd error message..."
egrep ": not a valid command" LOGFILE > /dev/null
if [[ $? -ne 0 ]]
then
	num_errs+=1
	echo "Error: could not find error message"
else
	echo "... OK"
fi
rm -f LOGFILE

# Check for all commands - exit code
echo "checking all commands..."
./lab4b --log=LOGFILE <<-EOF
STOP
START
PERIOD=1
PERIOD=15
PERIOD=2
SCALE=C
SCALE=F
OFF
EOF
if [[ $? -ne 0 ]]
then
	num_errs+=1
	echo "Error: exit code was $? instead of 0"
else
	echo "... exit code OK"
fi

# Check for all comands - logging
if [[ ! -s LOGFILE ]]
then
	num_errs+=1
	echo "Error: could not file LOGFILE"
else
	for cmd in STOP START PERIOD=1 PERIOD=15 PERIOD=2 SCALE=C SCALE=F OFF
	do
		egrep $cmd LOGFILE > /dev/null
		if [[ $? -ne 0 ]]
		then
			num_errs+=1
			echo "Error: could not find $cmd in LOGFILE"
		else
			echo "... $cmd OK"
		fi
	done
fi
rm -f LOGFILE

if [[ $num_errs -ne 0 ]]
then
	echo "FAILED TESTS WITH $num_errs!"
else
	echo "PASSED ALL TESTS!"
fi



