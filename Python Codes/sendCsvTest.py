import socket
import json
import numpy as np

host = ""  # The server's hostname or IP address
port = 65432  # The port used by the server

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)       # Create socket

try:     
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1) # Connect to server
    s.bind((host,port))
except socket.error:
    print("Bind failed")
else:
    s.listen()
    print("Socket awaiting messages")
    conn, addr = s.accept()
    print(f"Connected by {addr}")

dataset = np.genfromtxt('actual_dosing.csv', delimiter=',' , encoding="utf8")

dataDict = {
    'points':[]
}

for i in range (0,len(dataset[1:,0])):
    point = {
        'x': dataset[i+1,0],
        'y': dataset[i+1,1]
    }
    dataDict['points'].append(point)

toSend = json.dumps(dataDict, indent= 2)
conn.sendall(toSend.encode())