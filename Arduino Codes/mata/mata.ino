#include "communication.h"
#include "pinout.h"

int y = 0;

void setup()
{
    pinSetup();
    sensors::initializeScales();
    sensors::initializeTDS();
    communications::serialSetup();
    sensors::scaleOn(scaleN,true);
    sensors::scaleOn(scaleP,true);
    sensors::scaleOn(scaleK,true);

}

void loop()
{
    while (!Serial.available())
    y = Serial.parseInt();
    communications::sendSensorInfo(y);
    y = 0;  
    while(Serial.available()>0) Serial.read();  

}