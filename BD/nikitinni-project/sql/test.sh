#!/bin/bash
rm logs.txt
psql -a -f 3.sql >> logs.txt
psql -a -f 4.sql >> logs.txt
psql -a -f 5.sql >> logs.txt
psql -a -f 6.sql >> logs.txt
psql -a -f 8.sql >> logs.txt
psql -a -f 9.sql >> logs.txt
psql -a -f 10.sql >> logs.txt
psql -a -f 11.sql >> logs.txt
psql -a -f drop.sql >> logs.txt
