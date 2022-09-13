import json


def getInitialParams (socket):

    initialData = socket.recv(1024)
    while initialData == b'':
        initialData = socket.recv(1024)
    initialDataJson = json.loads(initialData.decode())
    dosageHour = initialDataJson["time"]
    nPlants = initialDataJson["plants"]

    return dosageHour , nPlants

def sendJson(socket,data):

    toSend = json.dumps(data)
    socket.sendall(toSend.encode())
