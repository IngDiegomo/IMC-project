import iPadComms
import arduinoComms
import datetime
import time
import datasetInteraction

def canBeFloat(num):                            # This function says if a string can be turned into a float
    try:
        float(num)
        return True
    except ValueError:
        return False

def nutrientFilling(serial,hmiDict,socket):

    # nutrient filling sequence
    serial.write(b'1')                  # Tell arduino to start the nutrient filling sequence

    while True:
        ack = serial.read()            # Acknowledgement from arduino
        if ack == b'1':
            print(ack)
            break

    # show in hmi grams per tank
    iPadComms.sendJson(socket,hmiDict)
    time.sleep(0.5)

    while (hmiDict["checks"] != [True, True, True]):                       # While all checks ar not done

        iPadComms.getCheckSignal(socket)                        # Wait for ipad check signal
        time.sleep(0.5)                     
        scales = arduinoComms.requestSensorInfo(serial,b'2')    # Request the weightscale readings from arduino
        hmiDict["grams"] = scales

        for i in range(0,len(hmiDict["checks"])):                                            # Loop all scales
            if ((scales[i] >= hmiDict["gramsToPour"][i] ) and (hmiDict["checks"][i] == 0)):  # If the scale value is greater or equal to than the grams to Pour
                hmiDict["checks"][i] = True 
                hmiDict["gramsDifference"][i] = hmiDict["gramsToPour"][i] - scales[i]
    
        iPadComms.sendJson(socket, hmiDict)                                                 # Send them to HMI
        time.sleep(0.5)

    hmiDict["pourDidFinish"] = True
    iPadComms.sendJson(socket, hmiDict)                                                 # Send them to HMI
    time.sleep(0.2)

    iPadComms.getContinueSignal(socket)                         # Wait for continue signal 
    serial.write(b'3') 
    return hmiDict


def waterFilling(serial, socket, maxed, hmiDict):           # Routine to fill the water of each nutrient tank

    # water filling sequence
    serial.write(b'1')                  # Tell arduino to start the water filling sequence
    
    while True:
        ack = serial.read()            # Acknowledgement from arduino
        if ack == b'1':
            print(ack)
            break

    filled = [0, 0, 0]  # Variable that indicates if a tank is filled
    level = 0           # Variable that indicates the current level of a tank
    lastLevel = 0       # Variable that indicates the last level of a tank
    changed = 0         # Variable that indicates if another tank is being filled 
    tank = 0            # Variable that indicates the current tank being filled

    while filled != [1, 1, 1]:  # While not all tanks are filled

        data = serial.read_until()  # Read the data sent from the arduino
        data = data.decode()

        if canBeFloat(data):                                       # If the data can be converte to float
            data = float(data)                                  # Convert it to float
            level = round((abs(data)/maxed)*100,2)              # Make it a percentage of the max filling value
            hmiDict["levels"][tank]= str(level) + "%"           # Update the hmiDict, the key "levels" and the corresponding tank
            changed = 0                                         
            if (level >= 80):                                   # If the level is greater than 80%
                hmiDict['criticals'][tank] = True                  # Turn on critical warning
                hmiDict['warnings'][tank] = False
            elif (level >= 70):                                 # If the level is greater than 70% 
                hmiDict['warnings'][tank] = True                   # Turn on warning

        elif data == 'n\n':                                             # If data is 'n\n'
            tank= 0                                                     # The tank being filled is 0 (N tank)
            hmiDict["fillingValves"] = [True, False, False, False]             # The valve of the first tank is open
            hmiDict["fillingPump"]= True                                         # The filling pump is on

        elif data == 'p\n':                                                 # If data is 'p\p'
            filled = [1, 0, 0]                                               # The first tank has been filled
            hmiDict["fillingValves"] = [False, True, False, False]             # The valve of the second tank is open
            tank = 1                                                            # The tank being filled now is 1 (P tank)
            changed = 1                                         # Indicate that theres been a change
            level = 0                                           # Level is now 0

        elif data == 'k\n':                                     # Same as above but with K tank
            filled = [1, 1, 0]
            hmiDict["fillingValves"] = [False, False, True, False]                           
            tank = 2
            changed = 1
            level = 0

        elif data == 'd\n':                                     # This signifies that the filling is done
            filled = [1, 1, 1]
            hmiDict["fillingValves"] = [False, False, False, False]             # All valves are closed
            hmiDict["fillingPump"]= False                                       # The filling pump is off
            tank = 3
            changed = 1
            level = 0
        
        if (level >= lastLevel + 5) or (changed == 1):          # If the level read is greater or equal to the last level + 5 or if there is been a change
            lastLevel = level                                   # Update the last level
            iPadComms.sendJson (socket, hmiDict)                # Send info to iPad
            time.sleep(1)
    
    levels = arduinoComms.recieveSensorInfo(serial)             # Recieve the verification values of the tank levels

    print("Lectura arduino, antes de agitar:")                  # This is for debugging
    print(levels)
    # Conver the levels into percentage and update the hmi dict
    hmiDict["levels"][0:3] = [str(round((abs(i)/maxed)*100,2))+"%" for i in levels]     
    
    iPadComms.sendJson(socket,hmiDict)                                  # Send the data to the iPad
    time.sleep(0.5)

    return hmiDict

def mixing(serial,socket,hmiDict):                          # Mixing routine

    mixed = 0                                               # Variable to signify the end of mixing
    lastMixingDict = hmiDict.copy()                         # Copy of the hmi dict

    while (mixed != 1):                                     # While the mixing is not finished

        data = serial.read_until()                          # Read the arduino data
        data = data.decode()

        if ("1" in data) or ("0" in data):                  # If there is a 1 or a 0 in the data
            data = data.split(',')
            hmiDict["lids"] = [float(i) for i in data]      # Update the lids values
    
        elif data == "m\n":                                 # If there is an m in the data
            hmiDict["motors"] = [True, True, True]          # Update the motors values

        elif data == "d\n":                                 # If there is a d in the data
            hmiDict["motors"] = [False, False, False]       # The motors are turned off
             
        elif data == "e\n":                                 # If there is an e in the data
            hmiDict["motors"] = [False, False, False]       # The motors are turned off
            mixed = 1                                       # The mixing is done
            iPadComms.sendJson(socket,hmiDict)              # Send 
            time.sleep(0.5)
            break
        
        elif data == "i\n":                                 # If data is an i
            hmiDict["interrupted"] = True                   # The mixing was interrupted
        
        if lastMixingDict != hmiDict:                       # If there has been any changes to the dict
            iPadComms.sendJson(socket,hmiDict)              # Send the data to the iPad
            time.sleep(0.5)
            lastMixingDict = hmiDict.copy()                 # Update the last dict

    data = serial.read_until()                              # Read the data for the tds values
    data = data.decode()
    data = data.split(',')                      

    hmiDict["tds"] = [float(i) for i in data]               # Update the tds values
    
    iPadComms.sendJson(socket,hmiDict)                      # Send the data to the iPad
    time.sleep(0.2)

    levels = arduinoComms.recieveSensorInfo(serial)         # Recieve the levels info

    print("Lectura arduino, despues de agitar:")            
    print(levels)

    return hmiDict

def dosing(day, serial, socket, maxed, maxedTank, hmiDict):            # Dosing routine

    serial.write(b'1')

    levels = arduinoComms.recieveSensorInfo(serial)                                 # This is for debugging
    print("Litros leidos por el arduino (menos los 5 gramos de los nutrientes):")
    print(levels)
    
    conValues = datasetInteraction.getTodayValues(day)          
    print("Concentraciones del dataset que la raspi mando:")                        # This is for debugging
    print(conValues)
    arduinoComms.sendArrayData(serial,conValues)

    cons = arduinoComms.recieveSensorInfo(serial)
    print("Concentraciones del dataset que el arduino recibio:")                    # This is for debugging
    print(cons)

    conPotes = arduinoComms.recieveSensorInfo(serial)
    print("Concentraciones que el arduino dice que hay en los potes (5/litros):")   # This is for debugging
    print(conPotes)

    volumes = arduinoComms.recieveSensorInfo(serial)
    print("Volumenes resultantes a dosificar, de las ecuaciones calculado por el arduino:") # This is for debugging
    print(volumes)

    routineStep = 0                 # Step of the routine
    dosed = 0                       # Indicates if the dosing is finished
    lastLevel = 0
    changed = 0

    while dosed == 0:                       # While dosing is not finished

        data = serial.read_until()          # Recieve data from arduino
        data = data.decode()
        
        if data == "1\n" or data == "0\n":      # If data is 1 or 0
    
            hmiDict['dosePumps'] =  [routineStep==0, routineStep==2, routineStep==4]     # Dose pumps values depend on the routine step
            hmiDict['fillingValves'][3] = routineStep == 6                                   # Filling valves and pump values depend on the routine step
            hmiDict['fillingPump'] = routineStep == 6
            routineStep += 1
            changed = 1

        elif data == "d\n":     # If d recieved, dosing is done
            dosed = 1
            changed = 1
        
        elif canBeFloat(data):                                       # If the data can be converte to float
            data = float(data)                                  # Convert it to float
            level = round((abs(data)/maxedTank)*100,2)              # Make it a percentage of the max filling value
            hmiDict["levels"][3]= str(level) + "%"           # Update the hmiDict, the key "levels" and the corresponding tank                                         
            if level>lastLevel + 3:
                changed = 1
                lastLevel = level
            if (level >= 80):                                   # If the level is greater than 80%
                hmiDict['criticals'][3] = True                  # Turn on critical warning
                hmiDict['warnings'][3] = False
            elif (level >= 70):                                 # If the level is greater than 70% 
                hmiDict['warnings'][3] = True                   # Turn on warning
        
        if changed == 1:
            iPadComms.sendJson(socket,hmiDict)       # Send the data to the iPad
            time.sleep(0.5)
            changed = 0
    
    levels = arduinoComms.recieveSensorInfo(serial)                     # Recieve the levels from the arduino

    levelsNutrientTanks = [str(round((abs(i)/maxed)*100,2))+"%" for i in levels]    # Get the percentages for the nutrient tanks
    levelsNutrientTanks.pop()
    hmiDict["levels"][0:3] = levelsNutrientTanks
    
    hmiDict["levels"][3] = str(round((abs(levels[3])/maxedTank)*100,2))+"%"         # Get the percentage of the main tank

    hmiDict['tds'] = arduinoComms.recieveSensorInfo(serial)                         # Get the tds level

    iPadComms.sendJson(socket,hmiDict)

    return hmiDict

def irrigation(serial, socket, maxed, maxedTank, nIrrigation, doseTime, doseWhenReady, hmiDict):                      # This function starts the irrigation routine

    iPadComms.sendJson(socket,hmiDict)
    volumeToIrrigate = maxedTank/nIrrigation
        
    dateNow = datetime.datetime.now()
    minutesNow = dateNow.minute + dateNow.hour * 60
    dosageMinutes =doseTime.minute + doseTime.hour * 60
    if (minutesNow > dosageMinutes):
            minutesLeft = 24*60 - minutesNow + dosageMinutes
    elif (minutesNow < dosageMinutes):
        if (doseWhenReady):
            minutesLeft = 24*60 - minutesNow + dosageMinutes
        else:
            minutesLeft = dosageMinutes - minutesNow

    minuteStep = round(minutesLeft/nIrrigation)
    i = 0
    while (i < nIrrigation):
        volumeIrrigated = 0
        serial.write(b'2')
        timeStart = datetime.datetime.now()
        minutesStart = timeStart.hour*60 + timeStart.minute
        hmiDict["irrigationPump"] = True
        hmiDict["aerationPump"] = True
        iPadComms.sendJson(socket,hmiDict)
        time.sleep(0.5)
        while(volumeIrrigated < volumeToIrrigate):
            levels = arduinoComms.recieveSensorInfo(serial)                                 # Recieve the levels from the arduino
            levelsNutrientTanks = [str(round((abs(i)/maxed)*100,2))+"%" for i in levels]    # Get the percentages for the nutrient tanks
            levelsNutrientTanks.pop()
            hmiDict["levels"][0:3] = levelsNutrientTanks                            
            hmiDict["levels"][3] = str(round((abs(levels[3])/maxedTank)*100,2))+"%"         # Get the percentage of the main tank
            iPadComms.sendJson(socket,hmiDict)                                              # Send the data
            time.sleep(0.5)
            volumeIrrigated = maxedTank - levels[3]
            if (((levels[3]/maxedTank)*100) <= 1):                                          # If main tank level is below x%, finish irrigation
                i = nIrrigation
                serial.write(b'1')
                hmiDict["irrigationPump"] = False
                hmiDict["aerationPump"] = False
                iPadComms.sendJson(socket,hmiDict)
                return hmiDict

        i += 1
        serial.write(b'1')
        hmiDict["irrigationPump"] = False
        hmiDict["aerationPump"] = False
        iPadComms.sendJson(socket,hmiDict)
        time.sleep(0.2)

        delayNow = datetime.datetime.now()
        delayNowMinutes = delayNow.hour *60 + delayNow.minute
        delayMinutes = minutesStart + minuteStep
        while (delayNowMinutes < delayMinutes):
            delayNow = datetime.datetime.now()
            delayNowMinutes = delayNow.hour *60 + delayNow.minute

    return hmiDict

def irrigationDemo(serial, socket, maxedTank,  hmiDict):                      # This function starts the irrigation routine
    
    dateNow = datetime.datetime.now()
    minutesNow = dateNow.minute + dateNow.hour * 60
    delayMins = minutesNow + 5

    serial.write(b'2')
    hmiDict["irrigationPump"] = True
    hmiDict["aerationPump"] = True
    iPadComms.sendJson(socket,hmiDict)
    time.sleep(0.2)

    while (delayNowMinutes < delayMins):

        levels = arduinoComms.recieveSensorInfo(serial)                                 # Recieve the levels from the arduino                         
        hmiDict["levels"][3] = str(round((abs(levels[3])/maxedTank)*100,2))+"%"         # Get the percentage of the main tank
        iPadComms.sendJson(socket,hmiDict)                                              # Send the data
        time.sleep(0.5)
        if (((levels[3]/maxedTank)*100) <= 1):                                          # If main tank level is below x%, finish irrigation
            serial.write(b'1')
            hmiDict["irrigationPump"] = False
            hmiDict["aerationPump"] = False
            iPadComms.sendJson(socket,hmiDict)
            time.sleep(0.2)
            return hmiDict

        delayNow = datetime.datetime.now()
        delayNowMinutes = delayNow.hour *60 + delayNow.minute

    serial.write(b'1')
    hmiDict["irrigationPump"] = False
    hmiDict["aerationPump"] = False
    iPadComms.sendJson(socket,hmiDict)
    time.sleep(0.2)
    return hmiDict


