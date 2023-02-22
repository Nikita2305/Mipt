import socket
import sys
import time
import os

array = sys.argv[1:]
if (len(array) == 0):
    print("Expected config filenames as sys.args")
    quit()

for item in array:
    print(f"sending {item}...")
    try:
        with open(item, "rb") as f:
            data = f.read()
            print(data)
            name = os.path.basename(f.name)
        print(f"Name: {name}")
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
            sock.connect(('', 9999))
            sock.sendall(data)
            print(str(sock.recv(2048), encoding='utf-8'))
    except Exception as ex:
        print(f"Error with {item}: {str(ex)}")
