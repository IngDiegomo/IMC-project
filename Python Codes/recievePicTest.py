from tracemalloc import start
import routines
import serial
import demoRoutines
import socket
import iPadComms
import datetime
import datasetInteraction
import fileInteractions
import iPadComms
import arduinoComms
import datetime
import time
import datasetInteraction

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

x = int(input("day"))

datasetInteraction.sendActualCurveCsv(conn, x)
datasetInteraction.sendNextCurveCsv(conn, x)

file = open("imagen.jpg", "wb")
imageChunk = conn.recv(1024)

print(len(imageChunk))
print(imageChunk[-1])

while imageChunk[-1] != 217:
    
    file.write(imageChunk)

    imageChunk = conn.recv(1024)
    print(len(imageChunk))
    print(imageChunk[-1])

print('done')
file.close()