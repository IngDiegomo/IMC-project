


import datetime
import os.path 


dosageHour = datetime.time.fromisoformat("13:22:00")
today = datetime.date.today()
nextDosification = datetime.datetime.combine(today,dosageHour)

#Skipping the first day dosing
"""
tempDate = datetime.datetime.now()
currentTime = datetime.time(tempDate.hour, tempDate.minute, tempDate.second)
if currentTime > dosageHour:
    delta = datetime.timedelta(days=0, minutes= 2, seconds = 0)
    nextDosification = nextDosification + delta
    #dosingDay += 1
"""

while(1):
    dateNow = datetime.datetime.now()
    if dateNow >= nextDosification:
        # Llamar dosificacion
        # Llamar riego 
        #dosingDay += 1
        delta = datetime.timedelta(days=0, seconds = 5)
        nextDosification = dateNow + delta
        print("Fuca")

