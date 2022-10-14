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
demoDict = {
    "result":"Ninguna foto recibida"
    }

while True:
    
    iPadComms.getPic(conn)
    fileInteractions.classifyStaticPic()
    deficiency, certainty = fileInteractions.checkClassificationResults()
    if deficiency == 0:
        demoDict["result"] = "Planta sana " + str(round(certainty,3))
    elif deficiency == 1:
        demoDict["result"] = "Deficiencia de Nitrogeno " + str(round(certainty,3))
    elif deficiency == 2:
        demoDict["result"] = "Deficiencia de Fósforo " + str(round(certainty,3))
    elif deficiency == 3:
        demoDict["result"] = "Deficiencia de Potasio " + str(round(certainty,3))
        
    iPadComms.sendJson(conn, demoDict)
    time.sleep(0.5)
