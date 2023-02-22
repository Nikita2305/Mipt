#!/bin/bash

if [ "$1" == "" ]
then
	echo "lost argument 1"
	exit
fi

ls $1
cd $1
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
