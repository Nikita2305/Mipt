from socket import socket, AF_INET, SOCK_DGRAM

IP = "127.0.0.1"
PORT = 3000
MAX_SIZE = 1

sock = socket(AF_INET, SOCK_DGRAM)       # создание UDP-сокета
sock.bind((IP, PORT))                    # нужно анонсировать порт  

while True:
    data, addr = sock.recvfrom(MAX_SIZE) # получить сообщение
    print(data)
    sock.sendto(str(5).encode(), addr)
