#include "communication.h"
#include "sensors.h"


namespace communications
{
    void serialSetup()
    {
        Serial.begin(115200);
        while (!Serial);
    }

    void sendSensorInfo(int sensor)
    {
        switch (sensor)
        {

            case 1:
                float tdsReadings[3];
                tdsReadings[0] = sensors::readTds(tdsN);
                tdsReadings[1] = sensors::readTds(tdsP);
                tdsReadings[2] = sensors::readTds(tdsK);
                for (int i = 0; i <3; i++)
                {
                    Serial.print(tdsReadings[i]);
                    if (i < 2) Serial.print(',');
                }
                Serial.print('\n');
                break;

            case 2:
                int lsReadings[3];
                lsReadings[0] = digitalRead(LIMIT_SWITCH_N);
                lsReadings[1] = digitalRead(LIMIT_SWITCH_P);
                lsReadings[2] = digitalRead(LIMIT_SWITCH_K);
                for (int i = 0; i <3; i++)
                {
                    Serial.print(lsReadings[i]);
                    if (i < 2) Serial.print(',');
                }
                Serial.print('\n');
                break;
            
            case 3:
                float scaleReadings[3];
                scaleReadings[0] = scaleN.get_units(10);
                scaleReadings[1] = scaleP.get_units(10);
                scaleReadings[2] = scaleK.get_units(10);

                for (int i = 0; i <3; i++)
                {
                    Serial.print(scaleReadings[i]);
                    if (i < 2) Serial.print(',');
                }
                Serial.print('\n');
                break;
            
            default:
                break;

        }

    }

}