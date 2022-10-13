from textwrap import indent
import numpy as np
import json
import time
import iPadComms

def getTodayValues(day):
    """
    Get the concentration values of a specified day from the
    dataset.
    
    Parameters
    ----------
    day : int
        Specified day from which to get the values.
    
    Returns
    -------
    list of int
        [N, P, K] values of concentration.          
    """    
    dataset = np.genfromtxt('actual_dosing.csv', delimiter=',')
    return [round(dataset[day][1],4), round(dataset[day][2],4), round(dataset[day][3],4)]

def modifyCurve(deficiency, day, certainty):
    """
    Modify the concentration curve of a specified nutrient
    with a deficiency detected on a specified day.

    Modifies the csv files.
    
    Parameters
    ----------
    day : int
        Specified day from which to get the values.
    deficiency : int
        Specified nutrient deficiency 1, 2 or 3 
        for N, P and K.   
    """   
    actualDataset = np.genfromtxt('actual_dosing.csv', delimiter=',' , encoding="utf8")
    nextDataset = np.genfromtxt('next_dosing.csv', delimiter=',' , encoding="utf8")
    nextDataset[1:day, deficiency] = actualDataset[1:day, deficiency] * 1.15 * certainty
    actualDataset[day:, deficiency] = actualDataset[day:, deficiency] *1.15 * certainty

    np.savetxt("actual_dosing.csv", actualDataset, delimiter=",")
    np.savetxt("next_dosing.csv", nextDataset, delimiter=",")       

def sendActualCurveCsv(socket, day):

    valuesToSend = 10
    dataset = np.genfromtxt('actual_dosing.csv', delimiter=',' , encoding="utf8")

    for j in range (1,4):
        dataDict = {
            'points_'+str(j):[]
        }

        for i in range (capped(day-valuesToSend),capped(day+valuesToSend)):
            point = {
                'x': int(dataset[i,0]),
                'y': round(dataset[i,j],3)
            }
            dataDict['points_'+str(j)].append(point)
            
       
        iPadComms.sendJson(socket,dataDict)
        time.sleep(1)

def sendNextCurveCsv(socket, day):

    valuesToSend = 10
    dataset = np.genfromtxt('next_dosing.csv', delimiter=',' , encoding="utf8")

    for j in range (1,4):
        dataDict = {
            'points_'+str(j+3):[]
        }

        for i in range (capped(day-valuesToSend),capped(day+valuesToSend)):
            point = {
                'x': int(dataset[i,0]),
                'y': round(dataset[i,j],3)
            }
            dataDict['points_'+str(j+3)].append(point)

    
        iPadComms.sendJson(socket,dataDict)
        time.sleep(1)

def capped(n):
    if n <= 1:
        return 1
    elif n>175:
        return 175
    else:
        return n

"""
dosingDay = 20

consToday = getTodayValues(dosingDay)    # Concentration values of starting day
print(consToday)
maxCons = getTodayValues(175)

gramsPerPlant = [0, 0, 0]

for i in range(0,3):
    gramsPerPlant[i] = round(consToday[i] * 390 / maxCons[i])

print (gramsPerPlant)



consT = [i/(3090*0.65) for i in gramsPerPlant]

volumeToDoseN = ((consToday[0]*(1.27*(10^2)/2.5*(10^2)))/consT[0])
volumeToDoseP = ((consToday[1]*(1.27*(10^2)/2.5*(10^2)))/consT[1])
volumeToDoseK = ((consToday[2]*(1.27*(10^2)/2.5*(10^2)))/consT[2])
volumeToDoseTank = (((-consT[2]*consT[0]*consT[1]+consT[2]*consT[0]*consToday[1]
                       +consT[2]*consT[0]*consToday[1]+consT[0]*consT[1]
                       *consToday[2])*(-1.27*(10^2)/2.5*(10^2)))
                       /(consT[2]*consT[0]*consT[1]))


print(volumeToDoseN)
print(volumeToDoseP)
print(volumeToDoseK)
print(volumeToDoseTank)
"""
        