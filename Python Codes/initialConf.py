import socket 
from datetime import time
import serial 
import iPadComms
import json

host = ""  # The server's hostname or IP address
port = 65432  # The port used by the server

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

try:
    ser = serial.Serial("/dev/ttyACM0", 230400, timeout=1)
except:
    ser = serial.Serial("/dev/ttyACM1", 230400, timeout=1)
    
ser.reset_input_buffer()

try:
    s.bind((host,port))
except socket.error:
    print("Bind failed")
else:
    s.listen()
    print("Socket awaiting messages")
    conn, addr = s.accept()
    print(f"Connected by {addr}")

# wait for iPad start signal

#start = conn.recv(1024)
#while start != b'1':
#    start = conn.recv(1024)

#print(start)

# get initial json from iPad

dosageHour , nPlants = iPadComms.getInitialParams(conn)

dosageHour = time.fromisoformat(str(dosageHour))

# show in hmi 100 grams per tank

# nutrient filling sequence
ser.write('1')                  # Tell arduino to start the nutrient filling sequence

while True:
    ack = ser.read()            # Acknowledgement from arduino
    if ack == b'1':
        print(ack)
        break




# recieve limit switch info from arduino, send to hmi 
"""
lsN = 0
lsP = 0
lsK = 0
lsAll = [lsN, lsP, lsK]
gramsN = 0
gramsP = 0
gramsK = 0
gramsAll = [gramsN, gramsP, gramsK]
while (gramsN <= 100) and (gramsP <= 100) and (gramsN <= 100):
    
    if (gramsN<= 100):
        
        if not ((lsN == 0) and (lsP == 1) and (lsK == 1)):
            conn.sendall(b"Incorrect lid open")
            conn.sendall(lsAll.encode)
            conn.sendall(gramsAll.encode)
        else:
            conn.sendall(lsAll.encode)
            conn.sendall(gramsAll.encode)

    elif (gramsP<= 100):
        if not ((lsN == 1) and (lsP == 0) and (lsK == 1)):
            conn.sendall(b"Incorrect lid open")
            conn.sendall(lsAll.encode)
            conn.sendall(gramsAll.encode)
        else:
            conn.sendall(lsAll.encode)
            conn.sendall(gramsAll.encode)

    elif (gramsK<= 100):
        if not ((lsN == 1) and (lsP == 1) and (lsK == 0)):
            conn.sendall(b"Incorrect lid open")
            conn.sendall(lsAll.encode)
            conn.sendall(gramsAll.encode)
        else:
            conn.sendall(lsAll.encode)
            conn.sendall(gramsAll.encode)

    #recieve lsN,lsP,lsK and gramsN, gramsP, gramsK from arduino and update them
conn.sendall(b"Filling completed")
"""
