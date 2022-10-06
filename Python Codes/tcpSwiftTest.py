from tracemalloc import start
import routines
import serial
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

maxed = 3090                                                # Grams of a container full of water

dosingDay = 20                                              # Initial dosing day

photoStep = 5                                               # Days between each photo

photoDay = 5                                                # Days since last photo

deficiencyDay = 40                                          # Day from which to start taking photos

consToday = datasetInteraction.getTodayValues(dosingDay)    # Concentration values of starting day

gramsPerPlant = [(1000/dosingDay)*i for i in consToday]     # Grams per plant 

gramsPerPlant = [i if i > 1 else 1 for i in gramsPerPlant]  # If the grams are below 


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

dosageHour , nPlants, nIrrigation, doseWhenReady = iPadComms.getInitialParams(conn)

dosageHour = datetime.time.fromisoformat(str(dosageHour))

today = datetime.date.today()

nextDosification = datetime.datetime.combine(today,dosageHour)
#successDict = {
#    "success" : True
#}
#iPadComms.sendJson(conn,successDict)

gramsToPour = [round(i*nPlants) for i in gramsPerPlant]

checksScreenDict = {
    "gramsToPour": gramsToPour,
    "checks": [False, False, False],
    "grams": [0.0, 0.0, 0.0],
    "success" : True,
    "pourDidFinish": False,
    "gramsDifference": [0.0, 0.0, 0.0]
}

hmiScreenDict = {   
    "levels": ["0%", "0%", "0%", "0%"],
    "fillingValves": [0, 0, 0, 0],
    "fillingPump": 0,
    "warnings": [0, 0, 0, 0],
    "criticals": [0, 0, 0, 0],
    "lids": [0, 0, 0],
    "motors": [0, 0, 0],
    "tds": [0, 0, 0],
    "dosePumps": [0, 0 ,0],
    "interrupted": 0
}
print("aca")
time.sleep(1)
iPadComms.sendJson(conn,checksScreenDict)
print("aqui")
print(gramsToPour)
# show in hmi grams per tank
#while (True):
#    iPadComms.sendJson(conn,checksScreenDict)
#    a = input()
#    if (a == 25):
#        break

# nutrient filling sequence
while (checksScreenDict["checks"] != [True, True, True]):

    iPadComms.getCheckSignal(conn)

    #scales = arduinoComms.requestSensorInfo(serial,b'2')  
    scales = input("Gramos: ")
    scales = scales.split(',')
    scales = [float(i) for i in scales]                        
    checksScreenDict["grams"] = scales
    
    for i in range(0,len(checksScreenDict["checks"])):  

        if ((scales[i] >= checksScreenDict["gramsToPour"][i] ) and (checksScreenDict["checks"][i] == False)):  
                checksScreenDict["checks"][i] = True 
                checksScreenDict["gramsDifference"][i] = checksScreenDict["gramsToPour"][i] - scales[i]                                                                     
                                                
    
    iPadComms.sendJson(conn,checksScreenDict)                                           
    time.sleep(0.5)

checksScreenDict["pourDidFinish"] = True                         
iPadComms.sendJson(conn, checksScreenDict)                                        
time.sleep(0.2)

iPadComms.getContinueSignal(conn) 
print("Fuca hasta aqui")

filled = [0, 0, 0]  # Variable that indicates if a tank is filled
level = 0           # Variable that indicates the current level of a tank
lastLevel = 0       # Variable that indicates the last level of a tank
changed = 0         # Variable that indicates if another tank is being filled 
tank = 0            # Variable that indicates the current tank being filled

while filled != [1, 1, 1]:  # While not all tanks are filled

    data = input("Grams 1 valor: ")

    if routines.canBeFloat(data):                              # If the data can be converte to float
        data = float(data)                                  # Convert it to float
        level = round((abs(data)/maxed)*100,2)              # Make it a percentage of the max filling value
        hmiScreenDict["levels"][tank]= str(level) + "%"           # Update the hmiDict, the key "levels" and the corresponding tank
        changed = 0                                         
        if (level >= 80):                                   # If the level is greater than 80%
            hmiScreenDict['criticals'][tank] = 1                  # Turn on critical warning
            hmiScreenDict['warnings'][tank] = 0
        elif (level >= 70):                                 # If the level is greater than 70% 
            hmiScreenDict['warnings'][tank] = 1                   # Turn on warning

    elif data == 'n':                                     # If data is 'n\n'
        tank= 0                                             # The tank being filled is 0 (N tank)
        hmiScreenDict["fillingValves"] = [1, 0, 0, 0]             # The valve of the first tank is open
        hmiScreenDict["fillingPump"]= 1                           # The filling pump is on

    elif data == 'p':                                     # If data is 'p\p'
        filled = [1, 0, 0]                                  # The first tank has been filled
        hmiScreenDict["fillingValves"] = [0, 1, 0, 0]             # The valve of the second tank is open
        tank = 1                                            # The tank being filled now is 1 (P tank)
        changed = 1                                         # Indicate that theres been a change
        level = 0                                           # Level is now 0

    elif data == 'k':                                     # Same as above but with K tank
        filled = [1, 1, 0]
        hmiScreenDict["fillingValves"] = [0, 0, 1, 0]                           
        tank = 2
        changed = 1
        level = 0

    elif data == 'd':                                     # This signifies that the filling is done
        filled = [1, 1, 1]
        hmiScreenDict["fillingValves"] = [0, 0, 0, 0]             # All valves are closed
        hmiScreenDict["fillingPump"]= 0                           # The filling pump is off
        tank = 3
        changed = 1
        level = 0
        
    if (level >= lastLevel + 5) or (changed == 1):          # If the level read is greater or equal to the last level + 5 or if there is been a change
        lastLevel = level                                   # Update the last level
        iPadComms.sendJson (conn, hmiScreenDict)                # Send info to iPad
        time.sleep(1)

print(maxed*0.65)
levels = input("Grams 3 valores: ")

print("Lectura arduino, antes de agitar:")                  # This is for debugging
print(levels)
levels = [str(round((abs(i)/maxed)*100,2))+"%" for i in levels]     # Conver the levels into percentage
levels.append("0%")                                                 # Append the level of the main tank

hmiScreenDict["levels"] = levels                                          # Update the hmi dict

iPadComms.sendJson(conn,hmiScreenDict)                                  # Send the data to the iPad
time.sleep(0.5)


"""
hmiScreenDict = routines.waterFilling(ser,conn,maxed,hmiScreenDict)
hmiScreenDict = routines.mixing(ser,conn, hmiScreenDict)

#Skipping the first day dosing

tempDate = datetime.datetime.now()
currentTime = datetime.time(tempDate.hour, tempDate.minute, tempDate.second)
if currentTime > dosageHour:
    delta = datetime.timedelta(days=1, minutes= 0, seconds = 0)
    nextDosification = nextDosification + delta
    dosingDay += 1


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
"""
