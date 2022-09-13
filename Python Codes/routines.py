import iPadComms
import arduinoComms
import datetime
import time
import datasetInteraction

def isFloat(num):
    try:
        float(num)
        return True
    except ValueError:
        return False

def nutrientFilling(serial,grams,socket):
    # show in hmi grams per tank
    socket.sendall(str(grams).encode())
    
    checks = [0, 0, 0]
    checksDict = {}
    act = False

    # nutrient filling sequence
    serial.write(b'1')                  # Tell arduino to start the nutrient filling sequence
    
    while True:
        ack = serial.read()            # Acknowledgement from arduino
        if ack == b'1':
            print(ack)
            break
    time.sleep(0.2)
    while (checks != [1,1,1]):

        scales = arduinoComms.requestSensorInfo(serial,b'2')
        scales.pop()
        for i in range(0,len(checks)):
            if ((scales[i] >= grams ) and (checks[i] == 0)):
                checks[i] = 1
                checksDict ["checks"] = checks
                act = True
        
        if act:
            iPadComms.sendJson(socket,checksDict)
            act = False
    
    time.sleep(1)
    gramsDict = {
        "grams": arduinoComms.requestSensorInfo(serial,b'3')
    }

    iPadComms.sendJson(socket, gramsDict)  

def waterFilling(serial,socket,maxed):

    filled = [0, 0, 0]
    level = 0
    waterDict = {
        "tank": 0,
        "level": level,
        "warning": 0,
        "critical": 0
    }
    lastLevel = 0
    changed = 0

    while filled != [1, 1, 1]:
        data = serial.read_until()
        data = data.decode()
        if isFloat(data):
            data = float(data)
            level = round((abs(data)/maxed)*100,2)
            waterDict['level'] = str(level) + "%"
            changed = 0
            if (level >= 80):
                waterDict['critical'] = 1
                waterDict['warning'] = 0
            elif (level >= 70):
                waterDict['warning'] = 1

        elif data == 'n\n':
            waterDict['tank'] = 'n'

        elif data == 'p\n':
            filled = [1, 0, 0]
            waterDict['tank'] = 'p'
            waterDict['level'] = 0
            waterDict['warning'] = 0
            waterDict['critical'] = 0
            changed = 1
            level = 0

        elif data == 'k\n':
            filled = [1, 1, 0]
            waterDict['tank'] = 'k'
            waterDict['level'] = 0
            waterDict['warning'] = 0
            waterDict['critical'] = 0
            changed = 1
            level = 0

        elif data == 'd\n':
            filled = [1, 1, 1]
            waterDict['tank'] = 0
            waterDict['level'] = 0
            waterDict['warning'] = 0
            waterDict['critical'] = 0
            changed = 1
            level = 0
        
        if (level >= lastLevel + 5) or (changed == 1):  
            lastLevel = level
            iPadComms.sendJson (socket, waterDict)
            time.sleep(0.2)
    
    levels = arduinoComms.recieveSensorInfo(serial)
    levels = [str(round((abs(i)/maxed)*100,2))+"%" for i in levels]

    finalLevelsDict = {
        "levels":levels
    }

    iPadComms.sendJson(socket,finalLevelsDict)

def mixing(serial,socket):

    mixingDict = {
        "lids": [0, 0, 0],
        "motors": [0, 0, 0],
        "mixed": 0,
        "interrupted": 0
    }

    lastMixingDict = mixingDict.copy()

    while (mixingDict['mixed'] != 1):
        data = serial.read_until()
        data = data.decode()

        if ("1" in data) or ("0" in data):
            data = data.split(',')
            print (data)
            mixingDict["lids"] = [float(i) for i in data]
    
        elif data == "m\n":
            mixingDict["motors"] = [1, 1, 1] 
            mixingDict["interrupted"] = 0

        elif data == "d\n":
            mixingDict["motors"] = [0, 0, 0]
             
            
        elif data == "e\n":
            mixingDict["motors"] = [0, 0, 0] 
            mixingDict["mixed"] = 1
            iPadComms.sendJson(socket,mixingDict)
            break
        
        elif data == "i\n":
            mixingDict["interrupted"] = 1
        
        if lastMixingDict != mixingDict:
            iPadComms.sendJson(socket,mixingDict)
            time.sleep(0.2)
            lastMixingDict = mixingDict.copy()

    data = serial.read_until()
    data = data.decode()
    data = data.split(',')

    tdsDict ={
        "tds": [float(i) for i in data]
    } 
    iPadComms.sendJson(socket,tdsDict)

def dosing(day, serial, socket, maxed):

    serial.write(b'1')

    conValues = datasetInteraction.getTodayValues(day)
    arduinoComms.sendArrayData(serial,conValues)

    routineStep = 0

    dosingDict = {
        "dosePumps": [0, 0 ,0],
        "fillingPump": 0,
        "fillingTank": 0,
        "dosed": 0
    }

    while dosingDict['dosed'] == 0:
        data = serial.read_until()
        data = data.decode()
        
        if int(data) == 1 or int(data) == 0:

            dosingDict['dosePumps'] = [routineStep==0, routineStep==2, routineStep==4]
            dosingDict['fillingTank'] = routineStep == 6
            dosingDict['fillingPump'] = routineStep == 6
            routineStep += 1
        
        elif data == "d\n":
            dosingDict['dosed'] = 1

        iPadComms.sendJson(socket,dosingDict)
        time.sleep(0.2)
    
    levels = arduinoComms.recieveSensorInfo(serial)
    levels = [str(round((abs(i)/maxed)*100,2))+"%" for i in levels]

    levelDict = {
        "levels":levels
    }

    iPadComms.sendJson(socket,levelDict)

    tds = arduinoComms.recieveSensorInfo(serial)
   
    tdsDict = {
        "tds":tds
    }

    iPadComms.sendJson(socket,tdsDict)

def irrigation(serial, socket, maxed):

        irrigationDict = {
            "levels": [0, 0, 0, 0],
            "pump": 1
        }
        iPadComms.sendJson(irrigationDict,socket)
        serial.write(b'1')
        endtime = datetime.timedelta(hours= 1) + datetime.datetime.now()    
        while (datetime.datetime.now() < endtime):

            levels = arduinoComms.recieveSensorInfo(serial)
            levels = [str(round((abs(i)/maxed)*100,2))+"%" for i in levels]
            irrigationDict = {
                "levels":levels
            }
            iPadComms.sendJson(socket,irrigationDict)
        
        serial.write(b'1')
        irrigationDict["pump"] = 0
        iPadComms.sendJson(socket,irrigationDict)

