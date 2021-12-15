#!/bin/bash
#Name: Jinbean Park
#EMAIL: keiserssose@gmail.com
#ID: 805330751

# 1. Chekc if the program supports commands 'SCALE=C' or not.
{ echo "SCALE=C"; sleep 3; echo "OFF"; } | ./lab4b --log=lab4bTest.txt

grep "SCALE=C" lab4bTest.txt > /dev/null
if [ $? -eq 0 ]; then
	echo "The program succeeds to log command SCALE=C into lab4bTest.txt"
else
	echo "Failed to log commands SCALE=C into lab4bTest.txt"
fi


# 2. Chekc if the program supports commands 'SCALE=F' or not.
{ echo "SCALE=F"; sleep 3; echo "OFF"; } | ./lab4b --log=lab4bTest.txt

grep "SCALE=F" lab4bTest.txt > /dev/null
if [ $? -eq 0 ]; then
	echo "The program succeeds to log command SCALE=F into lab4bTest.txt"
else
	echo "Failed to log commands SCALE=F into lab4bTest.txt"
fi


# 3. Chekc if the program supports commands 'OFF' properly or not.
{ sleep 3; echo "OFF"; } | ./lab4b --log=lab4bTest.txt

grep "OFF" lab4bTest.txt > /dev/null
if [ $? -eq 0 ]; then
	echo "The program succeeds to log command OFF into lab4bTest.txt"
else
	echo "Failed to log commands OFF into lab4bTest.txt"
fi

grep "SHUTDOWN" lab4bTest.txt > /dev/null
if [ $? -eq 0 ]; then
	echo "The program succeeds to log SHUTDOWN into lab4bTest.txt"
else
	echo "Failed to log SHUTDOWN into lab4bTest.txt"
fi


# 4. Chekc if the program supports commands STOP or not.
{ echo "STOP"; sleep 3; echo "OFF"; } | ./lab4b --log=lab4bTest.txt

grep "STOP" lab4bTest.txt > /dev/null
if [ $? -eq 0 ]; then
	echo "The program succeeds to log command STOP into lab4bTest.txt"
else
	echo "Failed to log commands STOP into lab4bTest.txt"
fi


# 5. Chekc if the program supports commands START or not.
{ echo "STOP"; sleep 3; echo "START"; sleep 3; echo "OFF"; } | ./lab4b --log=lab4bTest.txt

grep "START" lab4bTest.txt > /dev/null
if [ $? -eq 0 ]; then
	echo "The program succeeds to log command START into lab4bTest.txt"
else
	echo "Failed to log commands START into lab4bTest.txt"
fi

rm -f lab4bTest.txt
