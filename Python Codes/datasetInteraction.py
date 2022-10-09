from textwrap import indent
import numpy as np
import json
import time

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
    return [dataset[day][1], dataset[day][2], dataset[day][3]]

def modifyCurve(deficiency, day):
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
    nextDataset[1:day, deficiency] = actualDataset[1:day, deficiency] * 1.15 
    actualDataset[day:, deficiency] = actualDataset[day:, deficiency] *1.15

    np.savetxt("actual_dosing.csv", actualDataset, delimiter=",")
    np.savetxt("next_dosing.csv", nextDataset, delimiter=",")       

def sendActualCurveCsv():

    dataset = np.genfromtxt('actual_dosing.csv', delimiter=',' , encoding="utf8")

    for j in range (1,4):
        dataDict = {
            'points':[]
        }

        for i in range (0,len(dataset[1:,0])):
            point = {
                'x': dataset[i+1,0],
                'y': dataset[i+1,j]
            }
            dataDict['points'].append(point)
            dataDict = json.dumps(dataDict)
            time.sleep(0.5)

def sendNextCurveCsv():

    dataset = np.genfromtxt('next_dosing.csv', delimiter=',' , encoding="utf8")

    for j in range (1,4):
        dataDict = {
            'points':[]
        }

        for i in range (0,len(dataset[1:,0])):
            point = {
                'x': dataset[i+1,0],
                'y': dataset[i+1,j]
            }
            dataDict['points'].append(point)
            dataDict = json.dumps(dataDict)
            time.sleep(0.5)
