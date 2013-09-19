#!/bin/bash
NUM_TEST=10000

# g++ -Wall -O3 mine_sweeperer.cc

for ((i=0; i<NUM_TEST; i++)) 
do	
	./a.out
	EXIT_STATUS=$?
	if [ $EXIT_STATUS -ne 0 ]; then 
		exit 1      # will terminate if assertion failure occurs
	fi
done
echo -e "\n\nhey it might be working!!!"   # will only reach this line if no assertion failure occured
