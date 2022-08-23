#include "routines.h"

char rcv;
int next;

float nutrientWeightN, nutrientWeightP, nutrientWeightK;
float waterLitersN, waterLitersP, waterLitersK;
float tdsMixedN, tdsMixedP, tdsMixedK;

namespace routines
{

    void weightVerification()
    {
        nutrientWeightN = scaleN.get_units(5);
        nutrientWeightP = scaleP.get_units(5);
        nutrientWeightK = scaleK.get_units(5);
    }

    void nutrientFilling()
    {
        while(true)
        {
            while(!Serial.available());
            char rcv = Serial.read();
            while(Serial.available()>0) Serial.read();
            switch (rcv)
            {
            case '1':
                communications::sendSensorInfo(2);
                break;
            
            case '2':
                communications::sendSensorInfo(3);
                break;
            
            case '3':
                next = 1;
                break;
    
            default:
                break;
            }
            if (next == 1){
                next = 0;
                break;
            }
        }
        nutrientFilling();
    }

    void fillTankWater(HX711 &scale, uint8_t valve)
    {
        float weight = 0;
        weight = scale.get_units(5);
        if (weight < TANKS_FILL_VALUE)
        {
            digitalWrite(valve,HIGH);
            digitalWrite(FILLING_PUMP,HIGH);

            while (weight < TANKS_FILL_VALUE)
            {
                weight = scale.get_units(5);
            }

            digitalWrite(FILLING_PUMP,LOW);
            digitalWrite(valve,LOW);

        }

    }

    void waterFilling()
    {
        fillTankWater(scaleN,FILLING_VALVE_N);
        Serial.print('n\n');
        waterLitersN = (scaleN.get_units(5) - nutrientWeightN)/1000;

        fillTankWater(scaleP,FILLING_VALVE_P);
        Serial.print('p\n');
        waterLitersP = (scaleP.get_units(5) - nutrientWeightP)/1000;
        
        fillTankWater(scaleK,FILLING_VALVE_K);
        Serial.print('k\n');
        waterLitersK = (scaleK.get_units(5) - nutrientWeightK)/1000;

    }

    int mixing()
    {   
        unsigned long mixTime;
        int interrupted, mixed;
        Serial.print('mOn\n');
        mixTime = millis() + MIXING_TIME_MS;
        digitalWrite(MOTOR_N,HIGH);
        digitalWrite(MOTOR_P,HIGH);
        digitalWrite(MOTOR_K,HIGH);
        while(millis()<mixTime)
        {
            if (!digitalRead(LIMIT_SWITCH_N) or
                !digitalRead(LIMIT_SWITCH_P) or
                !digitalRead(LIMIT_SWITCH_K))
            {
                digitalWrite(MOTOR_N,LOW);
                digitalWrite(MOTOR_P,HIGH);
                digitalWrite(MOTOR_K,HIGH);
                return 0;
            }
        }
        tdsMixedN = sensors::readTds(tdsN);
        tdsMixedP = sensors::readTds(tdsP);
        tdsMixedK = sensors::readTds(tdsK);
        return 1;
    }
}

void dosing()
{
    float conN, conP, conK;
    conN = Serial.parseFloat();
    conP = Serial.parseFloat();
    conK = Serial.parseFloat(); 




    
}