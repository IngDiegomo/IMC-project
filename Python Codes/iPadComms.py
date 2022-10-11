from email.mime import image
import json
import shutil
import time
import os

def getInitialParams (socket):

    initialData = b''
    while initialData == b'':
        initialData = socket.recv(1024)
        print(initialData)
    initialDataJson = json.loads(initialData.decode())
    dosageHour = initialDataJson["time"]
    nPlants = initialDataJson["count"]
    nIrrigation = initialDataJson["irrigation"]
    dosingDay = initialDataJson["plant_age"]
    #doseWhenReady = initialDataJson["doseWhenReady"]
    #nIrrigation = 1
    doseWhenReady = True

    return dosageHour , nPlants, nIrrigation, doseWhenReady, dosingDay

def getCheckSignal (socket):

    checkDict = {
        "check" : False
    }
    while checkDict["check"] == False:
        checkData = socket.recv(1024)
        print(checkData)
        checkDataJson = json.loads(checkData.decode())
        if "check" in checkDataJson:
            checkDict["check"] = checkDataJson ["check"] 
    
    return 

def getContinueSignal (socket):

    continueDict = {
        "next" : False
    }
    while continueDict["next"] == False:
        continueData = socket.recv(1024)
        print(continueData)
        continueDataJson = json.loads(continueData.decode())
        if "next" in continueDataJson:
            continueDict["next"] = continueDataJson ["next"] 
    
    return 

def getDemoParams (socket):

    demoData = 0
    while demoData == 0:
        demoData = socket.recv(1024)
        print(demoData)
    demoDataJson = json.loads(demoData.decode())
    if "get_picture" in demoDataJson:
        demoDosingDay = 20
        doseDemo = False
        refillDemo = False
        getPicture = True
        refillTank = 0
        return demoDosingDay, getPicture, doseDemo, refillDemo, refillTank
    elif "dose" in demoDataJson:
        demoDosingDay = demoDataJson["day"]
        getPicture = False
        doseDemo = demoDataJson["dose"]
        refillDemo = demoDataJson["refill"]
        refillTank = demoDataJson["tank"]

    return demoDosingDay , getPicture, doseDemo, refillDemo, refillTank

def sendJson(socket,data):

    toSend = json.dumps(data)
    socket.sendall(toSend.encode())

def getPic(socket):

    imageDict = {
        'did_finish': False
        }

    file = open("imagen.jpg", "wb")

    while imageDict['did_finish'] != True:
    
        imageChunk = socket.recv(1024)
    
        try:
            imageDataJson = json.loads(imageChunk.decode())
            imageDict['did_finish'] = imageDataJson['did_finish']
        except:
            file.write(imageChunk)

    file.close()
    source = "/home/pi/P2/Python Codes/imagen.jpg"
    dest= "/home/pi/MATLAB_ws/R2021a/C/Users/diego/Desktop/Trimestre_17_FINAL_YA_FJLHI_GJIOA_UFKHAG/P2/Software/Codigos_Matlab/imagen.jpg"
    textPath = "/home/pi/MATLAB_ws/R2021a/C/Users/diego/Desktop/Trimestre_17_FINAL_YA_FJLHI_GJIOA_UFKHAG/P2/Software/Codigos_Matlab/results.txt"
    shutil.move(source,dest)
    try:
        os.remove(textPath)
        print("Done")
    except:
        print("Done")
    

