import socket 
import json
from time import sleep
import serial

HOST = "192.168.137.103"  # The server's hostname or IP address
PORT = 65432  # The port used by the server

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

s.connect((HOST, PORT))

hour = input("Enter hour in HH:MM:SS format: ")
nPlants = int(input("Enter quantity of plants: "))

dataDict = {
    "time": hour,
    "plants": nPlants
}

toSend = json.dumps(dataDict)
s.sendall(toSend.encode())

while True:
    grams = s.recv(1024)
    if grams != b'':
        print (grams.decode())
        break

print ("Eche " + str(grams.decode())+ " gramos en cada cubo")

while True:

    data = s.recv(1024)
    if data != b'':

        dictR = json.loads(data.decode())
        if "checks" in dictR:
            print("Checks:")
            print(dictR["checks"])
        elif "grams" in dictR:
            print("Grams of nutrient:")
            print(dictR["grams"])
        elif "tank" in dictR:
            print("State of tanks:")
            print(dictR)
        elif "levels" in dictR:
            print("Levels:")
            print(dictR)
        elif "lids" in dictR:
            print("Mixing State:")
            print(dictR)
        elif "tds" in dictR:
            print("TDS readings after mixing:")
            print(dictR)
        elif "dosePumps" in dictR:
            print("Dosing state:")
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

