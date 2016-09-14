#!/bin/bash

export LD_LIBRARY_PATH=.:std

for FILE in tests/*; do
	SHOULD=`egrep '^->' "$FILE" | sed 's/^-> //'`
	RESULT=`egrep -v '^->' "$FILE" | ./main 2>&1`
	echo -ne "Testing   $FILE\t"
	if [ "$SHOULD" = "$RESULT" ]; then
		echo "(ok)"
	else
		echo "(FAILED)"
		echo "EXPECTED:"
		echo "$SHOULD"
		echo "EVALUATED"
		echo "$RESULT"
	fi
done
