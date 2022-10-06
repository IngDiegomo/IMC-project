import datetime

maxedTank = 508
nIrrigation = 1
doseTime = datetime.time(20,30,0)
doseWhenReady = True
dateNow = datetime.time(13,45,0)


minutesNow = dateNow.minute + dateNow.hour * 60
dosageMinutes =doseTime.minute + doseTime.hour * 60
if (minutesNow > dosageMinutes):
        minutesLeft = 24*60 - minutesNow + dosageMinutes
elif (minutesNow < dosageMinutes):
    if (doseWhenReady):
        minutesLeft = 24*60 - minutesNow + dosageMinutes
    else:
        minutesLeft = dosageMinutes - minutesNow

minuteStep = round(minutesLeft/nIrrigation)
print(minutesLeft)
print(minuteStep)
    