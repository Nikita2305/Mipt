import socket

HOST = 'localhost'
PORT = 7778         
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind((HOST, PORT))
    s.listen(1)
    conn, addr = s.accept()
    with conn:
        print('Connected by', addr)
        i = 0
        while True:
            i += 1
            data = conn.recv(1024)
            if not data: break
            if (i == 2):
                s.shutdown(socket.SHUT_RDWR)
                s.close()
                exit(0)
            print(data)
            number = int.from_bytes(data, "little", signed=True)
            print(number)
            conn.sendall((number + 1).to_bytes(4, "little", signed=True))
