#!/bin/bash

if [ "$1" == "" ]
then
	echo "lost argument 1"
	exit
fi

if [ "$2" == "" ]
then
	echo "lost argument 1"
	exit
fi

rm file
gcc $1 -o file -lfuse3
export LD_LIBRARY_PATH=/usr/local/lib/x86_64-linux-gnu
./file -d $2 --src dir:dir2:dir3
