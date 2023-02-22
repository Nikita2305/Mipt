#!/bin/bash

gcc 0.c -o file -lfuse3
./file work --src dirs 
cat work/a.txt
fusermount3 -u work 
