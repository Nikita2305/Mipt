#!/bin/bash

if [ "$1" == "" ]
then
	echo "lost argument 1"
	exit
fi

if [ "$2" == "" ]
then
	echo "lost argument 2"
	exit
fi

rm file
gcc $1 -o file -lfuse3
export LD_LIBRARY_PATH=/usr/local/lib/x86_64-linux-gnu
./file $2 --src dir:dir2:dir3
ls $2
cd $2
ls -la 
echo -e "\n"
tree
echo -e "\n"
ls * -la 
echo -e "\n"
wc */*
echo -e "\n"
cat */*
echo -e "\n"
cat *
echo -e "\n"
cd ..
sleep 1
fusermount3 -u $2
sleep 1
