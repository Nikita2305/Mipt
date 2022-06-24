#!/usr/bin/bash
apt-get update
echo -e "Fetching python3, pip & modules"
apt-get install -y python3.8
apt install -y python3-pip
pip install psycopg2-binary
echo -e "DONE"
