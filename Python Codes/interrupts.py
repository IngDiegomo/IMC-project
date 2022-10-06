import iPadComms
import RPi.GPIO as GPIO
import time

state = 0

def emergencyButtonInterrupt(socket, hmiDict, emerg):
    
    global state
    n = 500000
    readings = [0 for i in range(n)]
    for i in range (0,n):
        readings[i] = GPIO.input(emerg)
    
    if (most_frequent(readings) and (state == 0)):
        print("Interrupting")
        hmiDict["emergency"] = True
        iPadComms.sendJson(socket, hmiDict)
        state = 1
    elif (most_frequent(readings) == 0) and (state == 1):  
        print("Resuming")
        hmiDict["emergency"] = False
        iPadComms.sendJson(socket, hmiDict)
        state = 0
        
    return


def most_frequent(data):
    return max(set(data),key = data.count)