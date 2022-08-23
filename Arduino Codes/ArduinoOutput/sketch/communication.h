#ifndef COMMUNICATION_H
#define COMMUNICATION_H
#include "pinout.h"
#include "sensors.h"



namespace communications
{
    
    void serialSetup();
    void sendSensorInfo(int sensor);
    float readCsvData();

}


#endif