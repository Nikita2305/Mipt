#!/bin/bash
rm -f q.txt
gcc 3.c -o file
./file q.txt $1 $2
echo "output:"
cat q.txt
