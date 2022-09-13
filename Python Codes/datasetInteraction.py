import numpy as np

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



