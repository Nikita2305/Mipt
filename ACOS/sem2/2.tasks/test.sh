#!/bin/bash

if [ "$1" == "" ]
then
	echo "missed the only argument - process id"
    exit
fi

if [ "$2" == "" ]
then
	echo "Note: you can add second argument(any string) to delay" 
fi

kill -s SIGTERM $1 # should be missed 

kill -s 34 $1 # should be ending 
if [ "$2" != "" ]
then
	sleep 0.1
fi

kill -s 34 $1 # should be ending 
if [ "$2" != "" ]
then
	sleep 0.1
fi

kill -s 38 $1 
if [ "$2" != "" ]
then
	sleep 0.1
fi

wc temp.txt # should be 11 strings // be careful about empty line
cat temp.txt
