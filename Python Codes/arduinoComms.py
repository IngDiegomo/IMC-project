def requestSensorInfo(serial, sensor):
    
    
    serial.write(sensor)
    data = serial.read_until()
    data = data.decode()
    data = data.split(',')
    data = [float(i) for i in data]
    
    return data

def recieveSensorInfo(serial):
    
    data = serial.read_until()
    data = data.decode()
    data = data.split(',')
    data = [float(i) for i in data]
    
    return data

def sendArrayData(serial, data):

    for i in data:
        msg = str(i) + 'x'
        serial.write(msg.encode())

    
