import iPadComms
import RPi.GPIO as GPIO
import time

state = 0

def emergencyButtonInterrupt(socket, hmiDict, emerg, lastState):
    
    global state
    n = 800000
    readings = [0 for i in range(n)]
    for i in range (0,n):
        readings[i] = GPIO.input(emerg)
    
    if (most_frequent(readings) and (state == 0)):
        print("Interrupting")
        hmiDict["status"] = "Parada de emergencia"
        iPadComms.sendJson(socket, hmiDict)
        time.sleep(0.5)
        state = 1
    elif (most_frequent(readings) == 0) and (state == 1):  
        print("Resuming")
        hmiDict["status"] = lastState
        iPadComms.sendJson(socket, hmiDict)
        time.sleep(0.5)
        state = 0
        
    return


def most_frequent(data):
    return max(set(data),key = data.count)