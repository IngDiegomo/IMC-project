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

demoDict = {
    "result": "Planta sana",
    "n": "x gramos",
    "p": "x gramos",
    "k": "x gramos"
}


x = int(input("day"))
iPadComms.sendJson(conn,demoDict)
time.sleep(0.5)



while True:

    datasetInteraction.sendActualCurveCsv(conn, x)
    datasetInteraction.sendNextCurveCsv(conn, x)


    demoDosingDay , getPicture, doseDemo, refillDemo, refillTank = iPadComms.getDemoParams(conn)
    print(demoDosingDay)

    if doseDemo:
    
        volumes = input("gramos: ")
        volumes = volumes.split(',')
        volumes = [float(i) for i in volumes]
        print("Volumenes resultantes a dosificar, de las ecuaciones calculado por el arduino:") # This is for debugging
        print(volumes)
        demoDict["n"] = str(round(volumes[0],2)) + " gramos"
        demoDict["p"] = str(round(volumes[1],2)) + " gramos"
        demoDict["k"] = str(round(volumes[2],2)) + " gramos"
        iPadComms.sendJson(conn,demoDict)
        time.sleep(0.5)

        dosed = 0                       # Indicates if the dosing is finished


        while dosed == 0:                       # While dosing is not finished

            data = input("Finish dosing: ")
        
            if data == "d":     # If d recieved, dosing is done
                dosed = 1
                
    elif getPicture:

        iPadComms.getPic(conn)
        fileInteractions.classifyStaticPic()
        deficiency, certainty = fileInteractions.checkClassificationResults()
        
        if deficiency == 0:
            demoDict["result"] = "Planta sana"
        elif deficiency == 1:
            demoDict["result"] = "Deficiencia de Nitrogeno"
        elif deficiency == 2:
            demoDict["result"] = "Deficiencia de Fósforo"
        elif deficiency == 3:
            demoDict["result"] = "Deficiencia de Potasio"
        
        iPadComms.sendJson(conn, demoDict)
        time.sleep(0.5)
        
        if deficiency >= 1 and deficiency <= 3:
            datasetInteraction.modifyCurve(deficiency, demoDosingDay, certainty)
            datasetInteraction.sendActualCurveCsv(conn, demoDosingDay)
            datasetInteraction.sendNextCurveCsv(conn, demoDosingDay)
            
    elif refillDemo:

        checkDemoDict = {
            "grams_to_pour": 'Gramos a vertir',
            "check": False,
            "success": False
            }
        
        iPadComms.sendJson(conn,checkDemoDict)
        time.sleep(0.5)
        actualWeights = input("4 pesos: ")
        actualWeights = actualWeights.split(',')
        actualWeights = [float(i) for i in actualWeights]

        if actualWeights[refillTank] >= 1200:
            checkDemoDict["grams_to_pour"] = 'Tanque lleno'
            iPadComms.sendJson(conn,checkDemoDict)
            time.sleep(0.5)
        

        else:
            checkDemoDict["success"] = True
            iPadComms.sendJson(conn,checkDemoDict)
            time.sleep(0.5)

            if demoDosingDay>= 20 and demoDosingDay<=50:
                checkDemoDict["grams_to_pour"] = 'Eche 5 gramos'
                gramsToPour = 5
            elif demoDosingDay>50 and demoDosingDay<= 80:
                checkDemoDict["grams_to_pour"] = 'Eche 10 gramos'
                gramsToPour = 10
            elif demoDosingDay>80 and demoDosingDay<= 120:
                checkDemoDict["grams_to_pour"] = 'Eche 20 gramos'
                gramsToPour = 20
            elif demoDosingDay>80 and demoDosingDay<= 120:
                checkDemoDict["grams_to_pour"] = 'Eche 40 gramos'
                gramsToPour = 40
            elif demoDosingDay>120 and demoDosingDay<= 175:
                checkDemoDict["grams_to_pour"] = 'Eche 70 gramos'
                gramsToPour = 70
            else:
                checkDemoDict["gramsToPour"] = 'Eche 5 gramos'
                gramsToPour = 5

            iPadComms.sendJson(conn,demoDict)
            time.sleep(0.5)

            limitValue = actualWeights[refillTank] + gramsToPour


            while (checkDemoDict["check"] != True):                       # While all checks ar not done

                iPadComms.getCheckSignal(conn)                        # Wait for ipad check signal
                time.sleep(0.5)                     
                scales = input("4 scales")    # Request the weightscale readings from arduino
                scales = scales.split(',')
                scales = [float(i) for i in scales]
                if (scales[refillTank] >= limitValue):                         # If the scale value is greater or equal to than the grams to Pour
                    checkDemoDict["check"] = True 
                    checkDemoDict["gramsToPour"] = 'Listo, llenando...' 
                    iPadComms.sendJson(conn, checkDemoDict)                                                 # Send them to HMI
                    time.sleep(0.5)


        






#print("aca")
#time.sleep(1)
#iPadComms.sendJson(conn,checksScreenDict)
#print("aqui")
#print(gramsToPour)
# show in hmi grams per tank
#while (True):
#    iPadComms.sendJson(conn,checksScreenDict)
#    a = input()
#    if (a == 25):
#        break

# nutrient filling sequence
"""
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


