import iPadComms
import arduinoComms
import datetime
import time
import datasetInteraction

def demoNutrientFilling(serial, tank, socket, dosingDay):

    demoDict = {
        "grams_to_pour": "Calculando gramos",
        "check": False,
        "success": True
    }

    if dosingDay>= 20 and dosingDay<=50:
        demoDict["grams_to_pour"] =  'Eche 5 gramos'
        gramsToPour = 5
    elif dosingDay>50 and dosingDay<= 80:
        demoDict["grams_to_pour"] = 'Eche 10 gramos'
        gramsToPour = 10
    elif dosingDay>80 and dosingDay<= 120:
        demoDict["grams_to_pour"] = 'Eche 20 gramos'
        gramsToPour = 20
    elif dosingDay>80 and dosingDay<= 120:
        demoDict["grams_to_pour"] = 'Eche 40 gramos'
        gramsToPour = 40
    elif dosingDay>120 and dosingDay<= 175:
        demoDict["grams_to_pour"] = 'Eche 70 gramos'  
        gramsToPour = 70  
    else:
        demoDict["grams_to_pour"] = 'Eche 5 gramos'
        gramsToPour = 5  

    
    # nutrient filling sequence
    serial.write(b'1')                  # Tell arduino to start the nutrient filling sequence

    while True:
        ack = serial.read()            # Acknowledgement from arduino
        if ack == b'1':
            print(ack)
            break

    initialValues = arduinoComms.requestSensorInfo(serial,b'2')
    limitValue = initialValues[tank] + gramsToPour

    
    # show in hmi grams per tank
    iPadComms.sendJson(socket,demoDict)
    time.sleep(0.5)


    while (demoDict["check"] != True):                       # While all checks ar not done

        iPadComms.getCheckSignal(socket)                        # Wait for ipad check signal
        time.sleep(0.5)                     
        scales = arduinoComms.requestSensorInfo(serial,b'2')    # Request the weightscale readings from arduino
        if (scales[tank] >= limitValue):                         # If the scale value is greater or equal to than the grams to Pour
                demoDict["check"] = True 
                demoDict["gramsToPour"] = 'Listo, llenando...' 
                    
                
        iPadComms.sendJson(socket, demoDict)                                                 # Send them to HMI
        time.sleep(0.5)

  
    serial.write(b'3') 

    return 


def dosingDemo(day, serial, socket, demoDict):            # Demo dosing routine

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
    demoDict["n"] = str(round(volumes[0]),2) + " gramos"
    demoDict["p"] = str(round(volumes[1]),2) + " gramos"
    demoDict["k"] = str(round(volumes[2]),2) + " gramos"
    iPadComms.sendJson(socket,demoDict)
    time.sleep(0.5)

    dosed = 0                       # Indicates if the dosing is finished


    while dosed == 0:                       # While dosing is not finished

        data = serial.read_until()          # Recieve data from arduino
        data = data.decode()
        
        if data == "1\n" or data == "0\n":      # If data is 1 or 0
            pass

        elif data == "d\n":     # If d recieved, dosing is done
            dosed = 1
            pass
        
    
    levels = arduinoComms.recieveSensorInfo(serial)                     # Recieve the levels from the arduino

    tds = arduinoComms.recieveSensorInfo(serial)                         # Get the tds level

    return demoDict

def irrigationDemo(serial, socket, maxedTank,  hmiDict, statusDict, lastState):                      # This function starts the irrigation routine
    

    dateNow = datetime.datetime.now()
    secondsNow = (dateNow.minute + dateNow.hour * 60)*60
    delaySecs = secondsNow + 45

    serial.write(b'2')
    hmiDict["irrigationPump"] = True
    hmiDict["aerationPump"] = True
    statusDict["status"] = "Irrigando"
    lastState = "Irrigando"
    iPadComms.sendJson(socket,statusDict)
    time.sleep(0.2)

    while (secondsNow < delaySecs):

        levels = arduinoComms.recieveSensorInfo(serial)                                 # Recieve the levels from the arduino                         
        hmiDict["levels"][3] = round((abs(levels[3])/maxedTank),2)         # Get the percentage of the main tank
        statusDict["levels"][3] = round((abs(levels[3])/maxedTank),2) 
        iPadComms.sendJson(socket,statusDict)                                              # Send the data
        time.sleep(0.5)
        if (((levels[3]/maxedTank)*100) <= 1):                                          # If main tank level is below x%, finish irrigation
            serial.write(b'1')
            hmiDict["irrigationPump"] = False
            hmiDict["aerationPump"] = False
            statusDict["status"] = "Irrigacion terminada"
            lastState = "Irrigacion terminada"
            iPadComms.sendJson(socket,statusDict)
            time.sleep(0.2)
            return hmiDict, statusDict

        delayNow = datetime.datetime.now()
        secondsNow = (delayNow.hour *60 + delayNow.minute)*60

    serial.write(b'1')
    hmiDict["irrigationPump"] = False
    hmiDict["aerationPump"] = False
    statusDict["status"] = "Irrigacion terminada"
    lastState = "Irrigacion terminada"
    iPadComms.sendJson(socket,statusDict)
    time.sleep(0.2)
    return hmiDict, statusDict, lastState

def refillDemo(serial, tank, socket, dosingDay):

    checkDemoDict = {
        "grams_to_pour": "Gramos a vertir",
        "check": False,
        "success": False
        }

    serial.write(b'4')
    time.sleep(0.5)
    actualWeights = arduinoComms.recieveSensorInfo(serial)

    if actualWeights[tank] >= 1200:
        checkDemoDict["grams_to_pour"] = "Tanque lleno"
        iPadComms.sendJson(socket,checkDemoDict)
        time.sleep(0.5)
        return

    else:
        checkDemoDict["grams_to_pour"] = "Calculando gramos"
        iPadComms.sendJson(socket,checkDemoDict)
        time.sleep(0.5)

        serial.write(b'3')
        time.sleep(0.5)
        msg = str(tank + 1) + 'x'
        serial.write(msg.encode())
        time.sleep(0.5)

        while True:
            ack = serial.read()            # Acknowledgement from arduino
            if ack == b'1':
                print(ack)
                break

        demoNutrientFilling(serial, tank, socket, dosingDay)

        while True:
            data = serial.read_until()                          # Read the arduino data
            data = data.decode()
            if data == "e\n":
                print(ack)
                break
        
        checkDemoDict["grams_to_pour"] = "Refill listo"
        iPadComms.sendJson(socket,checkDemoDict)
        time.sleep(0.5)
        
