import socket 
import json
from time import sleep
import serial

HOST = "192.168.0.110"  # The server's hostname or IP address
PORT = 65432  # The port used by the server

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

s.connect((HOST, PORT))

hour = input("Enter hour in HH:MM:SS format: ")
nPlants = int(input("Enter quantity of plants: "))
nIrrigation = int(input("Irrigations per day: "))
doseWhenReady = bool(int(input("Dose when ready?: ")))

dataDict = {
    "time": hour,
    "count": nPlants,
    "irrigation": nIrrigation,
    "doseWhenReady": doseWhenReady
}

toSend = json.dumps(dataDict)
s.sendall(toSend.encode())
firstRecieved = False

while True:

    data = s.recv(1024)
    if data != b'':

        dictR = json.loads(data.decode())
        if "gramsToPour" in dictR and not firstRecieved:
            print ("Eche " + str(dictR["gramsToPour"])+ " gramos en cada cubo")
            print("Checks:")
            print(dictR["checks"])
            print(dictR)
            firstRecieved = 1
            checkDict = {
                "check":bool(int(input("Check: ")))
            }
            toSend = json.dumps(checkDict)
            s.sendall(toSend.encode())
 
        elif "gramsToPour" in dictR:
            if (dictR["pourDidFinish"]):
                print(dictR)
                continueDict = {
                    "next":bool(int(input("Continue: ")))
                }
                toSend = json.dumps(continueDict)
                s.sendall(toSend.encode())
            else:
                print(dictR)
                checkDict = {
                    "check":bool(int(input("Check: ")))
                }
                toSend = json.dumps(checkDict)
                s.sendall(toSend.encode())
        elif "levels" in dictR:
            print(dictR)
            
        
 

"""
start = input("Enter 1 for start: ")
while int(start) != 1:
    start = input("Enter 1 for start: ")
    print(start)

s.sendall(b'1')


s.sendall(hour.encode())


s.sendall(nPlants.encode())
"""
"""
dataDict = {
    "asd": [1, 2, 3],
    "asd2": [1.1, 2.2, 3.3]
}
toSend = json.dumps(dataDict)
s.sendall(toSend.encode())
print('asd')
while True:
    data = s.recv(1024)
    if data != b'':
        decoded = data.decode()
        dictR = json.loads(decoded)
        print(decoded)
        print(dictR)
        print(dictR["asd"])
"""

