#include "communication.h"
#include "sensors.h"


namespace communications
{
    void serialSetup()
    {
        Serial.begin(115200);           // Begin serial communication with specefied baud rate
        while (!Serial);                // Wait for serial to connect
        
    }

    void sendSensorInfo(int sensor)
    {
        switch (sensor)
        {

            case 1:                                         // Read all the tds sensors and send them through serial
                float tdsReadings[3];
                tdsReadings[0] = sensors::readTds(tdsN);
                tdsReadings[1] = sensors::readTds(tdsP);
                tdsReadings[2] = sensors::readTds(tdsK);
                for (int i = 0; i <3; i++)
                {
                    Serial.print(tdsReadings[i]);
                    if (i < 2) Serial.print(',');           // Send them as a comma separated series of data
                }
                Serial.print('\n');                         // Send a line break to signalize end of message
                break;

            case 2:                                            // Read all the limit switches and send them through serial
                int lsReadings[3];
                lsReadings[0] = digitalRead(LIMIT_SWITCH_N);
                lsReadings[1] = digitalRead(LIMIT_SWITCH_P);
                lsReadings[2] = digitalRead(LIMIT_SWITCH_K);
                for (int i = 0; i <3; i++)
                {
                    Serial.print(lsReadings[i]);
                    if (i < 2) Serial.print(',');           // Send them as a comma separated series of data
                }
                Serial.print('\n');                         // Send a line break to signalize end of message
                break;
            
            case 3:                                         // Read all the scales and send the readings through serial
                float scaleReadings[4];
                scaleReadings[0] = sensors::getScaleMedian(1);
                scaleReadings[1] = sensors::getScaleMedian(2);
                scaleReadings[2] = sensors::getScaleMedian(3);
                scaleReadings[3] = sensors::getScaleMedian(4);

                for (int i = 0; i <4; i++)
                {
                    Serial.print(scaleReadings[i]);
                    if (i < 3) Serial.print(',');          // Send them as a comma separated series of data
                }
                Serial.print('\n');                        // Send a line break to signalize end of message
                break;
            
            default:
                Serial.flush();

        }

    }

}