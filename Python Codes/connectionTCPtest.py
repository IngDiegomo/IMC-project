import socket 

HOST = "192.168.137.90"  # The server's hostname or IP address
PORT = 65432  # The port used by the server

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

s.connect((HOST, PORT))
true = 1

start = input("Enter 1 for start: ")
while int(start) != 1:
    start = input("Enter 1 for start: ")
    print(start)

s.sendall(b'1')

hour = input("Enter hour in HH:MM:SS format: ")
s.sendall(hour.encode())

nPlants = input("Enter quantity of plants: ")
s.sendall(nPlants.encode())

