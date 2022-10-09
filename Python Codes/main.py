from tracemalloc import start
import routines
import serial
import socket
import iPadComms
import datetime
import datasetInteraction
import fileInteractions
import RPi.GPIO as GPIO
import interrupts

host = ""  # The server's hostname or IP address
port = 65432  # The port used by the server

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)       # Create socket

GPIO.setmode(GPIO.BCM)

emergButton = 26                                            # Emergency button pin


maxed = 3090                                                # Grams of a container full of water

dosingDay = 20                                              # Initial dosing day

photoStep = 5                                               # Days between each photo

photoDay = 5                                                # Days since last photo

deficiencyDay = 40                                          # Day from which to start taking photos

consToday = datasetInteraction.getTodayValues(dosingDay)    # Concentration values of starting day

#gramsPerPlant = [(1000/dosingDay)*i for i in consToday]     # Grams per plant 

#gramsPerPlant = [i if i > 1 else 1 for i in gramsPerPlant]  # If the grams are below 

gramsPerPlant = 5



try:
    ser = serial.Serial("/dev/ttyACM0", 115200, timeout=3000)  # Initialize serial connection
except:
    ser = serial.Serial("/dev/ttyACM1", 115200, timeout=3000)
    
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

checksScreenDict = {
    "gramsToPour": 0,
    "checks": [0, 0, 0],
    "grams": [0.0, 0.0, 0.0],
    "success" : True,
    "pourDidFinish": False,
    "gramsDifference": [0.0, 0.0, 0.0]
}

hmiScreenDict = {
    
    "levels": ["0%", "0%", "0%", "0%"],
    "fillingValves": [False, False, False, False],
    "fillingPump": False,
    "warnings": [False, False, False, False],
    "criticals": [False, False, False, False],
    "lids": [False, False, False],
    "motors": [False, False, False],
    "tds": [0, 0, 0],
    "dosePumps": [False, False ,False],
    "interrupted": False,
    "irrigationPump": False,
    "emergency": False,
}


GPIO.setup(emergButton, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)

GPIO.add_event_detect(emergButton, GPIO.BOTH,
                      callback = lambda emergButton: interrupts.emergencyButtonInterrupt(conn, hmiScreenDict,emergButton),  
                      bouncetime = 500)

dosageHour , nPlants, nIrrigation, doseWhenReady = iPadComms.getInitialParams(conn)

dosageHour = datetime.time.fromisoformat(str(dosageHour))
today = datetime.date.today()
nextDosification = datetime.datetime.combine(today,dosageHour)


#checksScreenDict["gramsToPour"] = [round(i*nPlants) for i in gramsPerPlant]
checksScreenDict["gramsToPour"] = [gramsPerPlant*nPlants, gramsPerPlant*nPlants, gramsPerPlant*nPlants]     # Grams to pour fixed value

maxedTank = 5000                                                               # Max grams to fill the main tank
tankLevel = 508*nPlants


ser.write(b'0')

checksScreenDict = routines.nutrientFilling(ser, checksScreenDict, conn)
hmiScreenDict = routines.waterFilling(ser, conn, maxed, hmiScreenDict)
hmiScreenDict = routines.mixing(ser, conn, hmiScreenDict)
hmiScreenDict = routines.dosing(dosingDay, ser, conn, maxed, maxedTank, hmiScreenDict)
hmiScreenDict = routines.irrigationDemo(ser,conn,maxedTank,hmiScreenDict)



"""
if (doseWhenReady):
    hmiScreenDict = routines.dosing(dosingDay, ser, conn, maxed, maxedTank, hmiScreenDict)
    delta = datetime.timedelta(days=1, minutes= 0, seconds = 0)
    nextDosification = nextDosification + delta
    dosingDay += 1
    hmiScreenDict = routines.irrigation(ser, conn, maxed, maxedTank, nIrrigation, dosageHour, doseWhenReady, hmiScreenDict)
    doseWhenReady = False
    
else:
    tempDate = datetime.datetime.now()
    currentTime = datetime.time(tempDate.hour, tempDate.minute, tempDate.second)
    if currentTime > dosageHour:
        delta = datetime.timedelta(days=1, minutes= 0, seconds = 0)
        nextDosification = nextDosification + delta
    

while(1):

    dateNow = datetime.datetime.now()
    
    if (dateNow >= nextDosification):

        hmiScreenDict = routines.dosing(dosingDay, ser, conn, maxed, maxedTank, hmiScreenDict)
        dosingDay += 1
        delta = datetime.timedelta(days=1, seconds = 0)
        nextDosification = dateNow + delta

        if dosingDay > deficiencyDay and photoDay != photoStep:
            photoDay += 1

        if dosingDay >= deficiencyDay and photoDay == photoStep:
            #fileInteractions.takeAndClassifyPic()
            # Revisar foto
            deficiency = 0
            datasetInteraction.modifyCurve(deficiency, dosingDay)
            photoDay = 0
        
        hmiScreenDict = routines.irrigation(ser, conn, maxed, maxedTank, nIrrigation, dosageHour, doseWhenReady, hmiScreenDict)
"""        

