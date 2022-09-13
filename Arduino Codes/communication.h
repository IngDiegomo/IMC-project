#ifndef COMMUNICATION_H
#define COMMUNICATION_H
#include "pinout.h"
#include "sensors.h"



namespace communications
{
    void serialSetup();                 // Setup serial communication
    void sendSensorInfo(int sensor);    // Send readings of a sensor through serial
    float readCsvData();                // Recieve Csv data from serial
}


#endif