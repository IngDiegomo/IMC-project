import datetime
import os.path 

#deltaTime = datetime.datetime()
#plantDatetime = datetime.datetime()
dateNow = datetime.datetime.now()

while(1):
    dateNow = datetime.datetime.now()
    if dateNow.hour == 19:
        # Llamar dosificacion
        delta = datetime.timedelta(days=0, seconds = 10)
        nextDosification = dateNow + delta
        while(1):
            dateNow = datetime.datetime.now()
            if dateNow >= nextDosification :
                # Llamar dosificacion
                print("Fuca")
                nextDosification = dateNow + delta


                


