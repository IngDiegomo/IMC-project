#include "routines.h"

int next;

float nutrientWeightN, nutrientWeightP, nutrientWeightK;
float waterLitersN, waterLitersP, waterLitersK;
float tankLevelN, tankLevelP, tankLevelK;
float tdsMixedN, tdsMixedP, tdsMixedK;

namespace routines
{

    void weightVerification()
    {
        float scaleReadings[3];
        scaleReadings[0] = scaleN.get_units(20);
        scaleReadings[1] = scaleP.get_units(20);
        scaleReadings[2] = scaleK.get_units(20);

        nutrientWeightN = scaleReadings[0];
        nutrientWeightP = scaleReadings[1];
        nutrientWeightK = scaleReadings[2];
                
        for (int i = 0; i <3; i++)
        {
            Serial.print(scaleReadings[i]);
            if (i < 2) Serial.print(',');          // Send them as a comma separated series of data
        }
        Serial.print('\n');                        // Send a line break to signalize end of message
    
    }
    

    void nutrientFilling()
    {
        char startNutrientFilling;
        
        while(!Serial.available());
        while (startNutrientFilling != '1') startNutrientFilling = Serial.read();    
        
        while(Serial.available() > 0) Serial.read();

        Serial.print('1');
        Serial.flush();

        while(next == 0)
        {
            while(!Serial.available());
            char data = Serial.read();
            while(Serial.available()>0) Serial.read();
            switch (data)
            {
            case '1':
                communications::sendSensorInfo('2');
                Serial.flush();
                break;
            
            case '2':
                communications::sendSensorInfo('3');
                Serial.flush();
                break;
            
            case '3':
                next = 1;
                break;
    
            default:
                break;
            }
        }
        weightVerification();
        Serial.flush();
    }

    void fillTankWater(HX711 &scale, uint8_t valve)
    {
        float weight = 0;
        weight = scale.get_units();
        if (weight < TANKS_FILL_VALUE)
        {
            digitalWrite(valve,HIGH);
            digitalWrite(FILLING_PUMP,HIGH);

            while (weight < TANKS_FILL_VALUE)
            {
                weight = scale.get_units();
                Serial.print(weight);
                Serial.print('\n');
                Serial.flush();
            }

            digitalWrite(FILLING_PUMP,LOW);
            digitalWrite(valve,LOW);
            delay(1000);

        }

    }

    void waterFilling()
    {
        Serial.print('n');
        Serial.print('\n');
        Serial.flush();
        fillTankWater(scaleN,FILLING_VALVE_N);
        
        Serial.print('p');
        Serial.print('\n');
        Serial.flush();
        fillTankWater(scaleP,FILLING_VALVE_P);
        
        Serial.print('k');
        Serial.print('\n');
        Serial.flush();
        fillTankWater(scaleK,FILLING_VALVE_K);
        
        Serial.print('d');
        Serial.print('\n');
        Serial.flush();

        tankLevelN = scaleN.get_units(20);
        tankLevelP = scaleP.get_units(20);
        tankLevelK = scaleK.get_units(20);

        waterLitersN = (tankLevelN - nutrientWeightN)/1000;
        waterLitersP = (tankLevelP - nutrientWeightP)/1000;
        waterLitersK = (tankLevelK - nutrientWeightK)/1000;

        float tanksLevel[3] = {tankLevelN, tankLevelP, tankLevelK};
        for (int i = 0; i <3; i++)
        {
            Serial.print(tanksLevel[i],2);
            if (i < 2) Serial.print(',');           // Send them as a comma separated series of data
        }
        Serial.print('\n');                         // Send a line break to signalize end of message
        Serial.flush();

    }

    int tryToMix()
    {   
        unsigned long mixTime;
        int interrupted, mixed;
        Serial.print('m');
        Serial.print('\n');
        Serial.flush();
        mixTime = millis() + MIXING_TIME_MS;
        delay(1000);
        digitalWrite(MOTOR_N,HIGH);
        delay(1000);
        digitalWrite(MOTOR_P,HIGH);
        delay(1000);
        digitalWrite(MOTOR_K,HIGH);
        while(millis()<mixTime)
        {
            if (digitalRead(LIMIT_SWITCH_N) or
                digitalRead(LIMIT_SWITCH_P) or
                digitalRead(LIMIT_SWITCH_K))
            {
                digitalWrite(MOTOR_N,LOW);
                digitalWrite(MOTOR_P,LOW);
                digitalWrite(MOTOR_K,LOW);
                Serial.print('d');
                Serial.print('\n');
                Serial.flush();
                return false;
            }
        } 
        
        digitalWrite(MOTOR_N,LOW);
        digitalWrite(MOTOR_P,LOW);
        digitalWrite(MOTOR_K,LOW);
        Serial.print('d');
        Serial.print('\n');
        Serial.flush();
        tdsMixedN = sensors::readTds(tdsN);
        tdsMixedP = sensors::readTds(tdsP);
        tdsMixedK = sensors::readTds(tdsK);
        return true;
    }
    

    void mixing()
    {

        while (true)
        {
            do 
            {
                delay(200);
                communications::sendSensorInfo('2');
                Serial.flush();
            }
            while (digitalRead(LIMIT_SWITCH_N) or
                   digitalRead(LIMIT_SWITCH_P) or
                   digitalRead(LIMIT_SWITCH_K));

            delay(200);
            communications::sendSensorInfo('2');
            Serial.flush();
    
            if (tryToMix())
            {
                Serial.print('e');
                Serial.print('\n');
                Serial.flush();
                communications::sendSensorInfo('1');
                Serial.flush();
                break; 
            }
            else
            {
                Serial.print('i');
                Serial.print('\n');
                Serial.flush();
        
            }
        }
    }

    void doseToTank(uint8_t pump, float volume, HX711 &scale)
    {
        digitalWrite(pump,HIGH);
        Serial.println(digitalRead(pump));
        while ((scale.get_units(2)/1000)<volume)
        digitalWrite(pump,LOW);
        Serial.println(digitalRead(pump));

    }

    void dosing()
    {
        float conTankN, conTankP, conTankK;
        float conN, conP, conK;
        float volumeN, volumeP, volumeK,volumeTank;

        conTankN = Serial.parseFloat();
        conTankP = Serial.parseFloat();
        conTankK = Serial.parseFloat(); 

        conN = nutrientWeightN/waterLitersN;
        conP = nutrientWeightP/waterLitersP;
        conK = nutrientWeightK/waterLitersK;

        volumeN = (conTankN*(1.27E+2/2.5E+2))/conN;
        volumeP = (conTankP*(1.27E+2/2.5E+2))/conP;
        volumeK = (conTankK*(1.27E+2/2.5E+2))/conK;
        volumeTank = ((-conK*conN*conP+conK*conN*conTankP
                       +conK*conP*conTankN+conN*conP
                       *conTankK)*(-1.27E+2/2.5E+2))
                       /(conK*conN*conP);

        doseToTank(DOSE_PUMP_N,volumeN,scaleN);
        doseToTank(DOSE_PUMP_P,volumeP,scaleP);
        doseToTank(DOSE_PUMP_K,volumeK,scaleK);

        digitalWrite(FILLING_VALVE_TANK,HIGH);
        digitalWrite(FILLING_PUMP,HIGH);
        Serial.println(digitalRead(FILLING_VALVE_TANK));
        Serial.println(digitalRead(FILLING_PUMP));
        while ((scaleTank.get_units(2)/1000)<volumeTank)
        digitalWrite(FILLING_PUMP,LOW);
        digitalWrite(FILLING_VALVE_TANK,LOW);
        Serial.println(digitalRead(FILLING_VALVE_TANK));
        Serial.println(digitalRead(FILLING_PUMP));

        Serial.print('d');
        Serial.print('\n');

        tankLevelN = scaleN.get_units(20);
        tankLevelP = scaleP.get_units(20);
        tankLevelK = scaleK.get_units(20);

        float tanksLevel[3] = {tankLevelN, tankLevelP, tankLevelK};
        for (int i = 0; i <3; i++)
        {
            Serial.print(tanksLevel[i],2);
            if (i < 2) Serial.print(',');           // Send them as a comma separated series of data
        }

        communications::sendSensorInfo('1');
    }

    void irrigation()
    {
        char data;
        digitalWrite(IRRIGATION_PUMP,HIGH);
        int dataSendTime = millis() + 1000;
        while (data != '1')
        {
            if (millis() >= dataSendTime)
            {
                dataSendTime += 1000;
                communications::sendSensorInfo('3');
            }
            if (Serial.available()) data = Serial.read();
        }
        digitalWrite(IRRIGATION_PUMP,LOW);

    }

}