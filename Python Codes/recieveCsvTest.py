import socket 
import json

HOST = "192.168.0.110"  # The server's hostname or IP address
PORT = 65432  # The port used by the server

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

s.connect((HOST, PORT))


data = s.recv(1048576)
if data != b'':
    print(data)
    dictR = json.loads(data.decode())
    
 