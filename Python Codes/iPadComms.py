import json


def getInitialParams (socket):

    initialData = b''
    while initialData == b'':
        initialData = socket.recv(1024)
        print(initialData)
    initialDataJson = json.loads(initialData.decode())
    dosageHour = initialDataJson["time"]
    nPlants = initialDataJson["count"]
    nIrrigation = initialDataJson["irrigation"]
    #doseWhenReady = initialDataJson["doseWhenReady"]
    #nIrrigation = 1
    doseWhenReady = True

    return dosageHour , nPlants, nIrrigation, doseWhenReady

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

def sendJson(socket,data):

    toSend = json.dumps(data)
    socket.sendall(toSend.encode())
