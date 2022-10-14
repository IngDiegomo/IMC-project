from time import time
from tracemalloc import start
import routines
import demoRoutines
import serial
import socket
import iPadComms
import datetime
import datasetInteraction
import fileInteractions
import RPi.GPIO as GPIO
import interrupts
import time

host = ""  # The server's hostname or IP address
port = 65432  # The port used by the server

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)       # Create socket

GPIO.setmode(GPIO.BCM)

emergButton = 26                                            # Emergency button pin

maxed = 3090                                                # Grams of a container full of water

photoStep = 5                                               # Days between each photo

photoDay = 5                                                # Days since last photo

deficiencyDay = 40                                          # Day from which to start taking photos

#gramsPerPlant = 5

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
    "checks": [False, False, False],
    "grams": [0.0, 0.0, 0.0],
    "success" : True,
    "pourDidFinish": False,
    "gramsDifference": [0.0, 0.0, 0.0]
}

hmiScreenDict = {
    
    "levels": [0.0, 0.0, 0.0, 0.0],
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

statusScreenDict = {
    "levels":[0.0, 0.0, 0.0, 0.0],
    "status": "normal",
    "nextDose": "calculando..."
}

GPIO.setup(emergButton, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)

GPIO.add_event_detect(emergButton, GPIO.BOTH,
                      callback = lambda emergButton: interrupts.emergencyButtonInterrupt(conn, statusScreenDict,emergButton),  
                      bouncetime = 600)

dosageHour , nPlants, nIrrigation, doseWhenReady, dosingDay = iPadComms.getInitialParams(conn)


#consToday = datasetInteraction.getTodayValues(dosingDay)    # Concentration values of starting day

#maxCons = datasetInteraction.getTodayValues(175)

#gramsPerPlant = [0, 0, 0]

#for i in range(0,3):
#    gramsPerPlant[i] = [consToday[i] * 390 / maxCons[i]]

#gramsPerPlant = [i if i > 1 else 1 for i in gramsPerPlant]  # If the grams are below 

if dosingDay>= 20 and dosingDay<=50:
    checksScreenDict["gramsToPour"] = [5, 5, 5]  
elif dosingDay>50 and dosingDay<= 80:
    checksScreenDict["gramsToPour"] = [10, 10, 10]
elif dosingDay>80 and dosingDay<= 120:
    checksScreenDict["gramsToPour"] = [20, 20, 20]
elif dosingDay>80 and dosingDay<= 120:
    checksScreenDict["gramsToPour"] = [30, 30, 30] 
elif dosingDay>120 and dosingDay<= 175:
    checksScreenDict["gramsToPour"] = [100, 100, 100]
else:
    checksScreenDict["gramsToPour"] = [5, 5, 5]    


#checksScreenDict["gramsToPour"] = [round(i*nPlants) for i in gramsPerPlant]
#checksScreenDict["gramsToPour"] = [gramsPerPlant*nPlants, gramsPerPlant*nPlants, gramsPerPlant*nPlants]     # Grams to pour fixed value

maxedTank = 5000                                                               # Max grams to fill the main tank
tankLevel = 508*nPlants


ser.write(b'0')

checksScreenDict = routines.nutrientFilling(ser, checksScreenDict, conn)
hmiScreenDict, statusScreenDict = routines.waterFilling(ser, conn, maxed, hmiScreenDict, statusScreenDict)
hmiScreenDict, statusScreenDict = routines.mixing(ser, conn, hmiScreenDict, statusScreenDict)
hmiScreenDict, statusScreenDict = routines.dosing(dosingDay, ser, conn, maxed, maxedTank, hmiScreenDict, statusScreenDict, nPlants)
hmiScreenDict, statusScreenDict = demoRoutines.irrigationDemo(ser,conn,maxedTank,hmiScreenDict, statusScreenDict)
statusScreenDict, nextDosification = routines.calculateNextDosingTime(conn, statusScreenDict, dosageHour)

demoDict = {
    "result": "Planta sana",
    "n": "x gramos",
    "p": "x gramos",
    "k": "x gramos"
}

iPadComms.getCheckSignal(conn) 
datasetInteraction.sendActualCurveCsv(conn, dosingDay)
datasetInteraction.sendNextCurveCsv(conn, dosingDay)
iPadComms.sendJson(conn, demoDict)
print('mande la curva')
time.sleep(0.5)

while True:

    demoDosingDay , getPicture, doseDemo, refillDemo, refillTank = iPadComms.getDemoParams(conn)

    if doseDemo:
        print('dosing')
        demoDict = demoRoutines.dosingDemo(demoDosingDay, ser, conn, demoDict, nPlants)
        print('dosed')
    elif getPicture:

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
        
        if deficiency >= 1 and deficiency <= 3:
            datasetInteraction.modifyCurve(deficiency, demoDosingDay, certainty)
            datasetInteraction.sendActualCurveCsv(conn, demoDosingDay)
            datasetInteraction.sendNextCurveCsv(conn, demoDosingDay)
            

                











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

