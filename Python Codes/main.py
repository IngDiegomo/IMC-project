import routines
import serial
import socket
import iPadComms
import datetime
import datasetInteraction
import fileInteractions

host = ""  # The server's hostname or IP address
port = 65432  # The port used by the server

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)       # Create socket

gramsPerPlant = 5                                           # Define grams per plant

maxed = 3090                                                # Grams of a container full of water

dosingDay = 20                                              # Initial dosing day

photoStep = 5                                               # Days between each photo

photoDay = 5                                                # Days since last photo

deficiencyDay = 40                                          # Day from which to start taking photos

try:
    ser = serial.Serial("/dev/ttyACM0", 115200, timeout=60)  # Initialize serial connection
except:
    ser = serial.Serial("/dev/ttyACM1", 115200, timeout=60)
    
ser.reset_input_buffer()

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

dosageHour , nPlants = iPadComms.getInitialParams(conn)

dosageHour = datetime.time.fromisoformat(str(dosageHour))
today = datetime.date.today()
nextDosification = datetime.datetime.combine(today,dosageHour)

ser.write(b'0')

routines.nutrientFilling(ser, gramsPerPlant*nPlants, conn)
routines.waterFilling(ser,conn,maxed)
routines.mixing(ser,conn)

#Skipping the first day dosing
"""
tempDate = datetime.datetime.now()
currentTime = datetime.time(tempDate.hour, tempDate.minute, tempDate.second)
if currentTime > dosageHour:
    delta = datetime.timedelta(days=1, minutes= 0, seconds = 0)
    nextDosification = nextDosification + delta
    dosingDay += 1
"""


while(1):
    dateNow = datetime.datetime.now()
    if dateNow >= nextDosification:
        routines.dosing(dosingDay,ser,conn,maxed)
        dosingDay += 1
        delta = datetime.timedelta(days=1, seconds = 0)
        nextDosification = dateNow + delta
        #routines.irrigation(ser,socket,maxed)
        
        if dosingDay > deficiencyDay and photoDay != photoStep:
            photoDay += 1

        if dosingDay >= deficiencyDay and photoDay == photoStep:
            fileInteractions.takeAndClassifyPic()
            # Revisar foto
            deficiency = 0
            datasetInteraction.modifyCurve(deficiency,dosingDay)
            photoDay = 0

