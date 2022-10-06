#include "routines.h"
#include "pinout.h"

int y = 0;
int asd;
char startNutrientFilling;
unsigned long tiempomedir1;
unsigned long tiempomedir2;

float actualVolumeN; 
float actualVolumeP; 
float actualVolumeK; 
float actualVolumeTank; 
float volumeToDose,volumeToDoseDT;
float volumeN; 
float volumeP; 
float volumeK; 
int weigthValues = 20;


void setup()
{
    pinSetup();
    communications::serialSetup();

    
    while(!Serial.available());
    //asd = Serial.parseInt();
    //volumeToDose = asd;
    //volumeToDoseDT = 508 - asd*3;
    while(Serial.available()>0) Serial.read();
    
    

    sensors::initializeScales();
    sensors::initializeTds();
    //sensors::scaleOn(scaleN,false);
    //sensors::scaleOn(scaleP,false);
    //sensors::scaleOn(scaleTank,false);
    //Serial.println("Iniciando");

    
}

void loop()
{
    // Con esto pruebo el tiempo que duran las medidas promedio de la rutina weightVerification
    //Serial.println("Iniciando");
    //tiempomedir1 = millis();
    //routines::weightVerification();
    //tiempomedir2 = millis();
    //Serial.println(tiempomedir2-tiempomedir1);

    // Con esto reviso el comportamiento de las 4 básculas simultaneamente
    //communications::sendSensorInfo('3');
    //Serial.print(scaleK.get_units());
    //Serial.print('\n'); 

    //digitalWrite(FILLING_VALVE_TANK,HIGH);
    //Serial.println("Asd");

    //digitalWrite(DOSE_PUMP_N, HIGH);
    //digitalWrite(DOSE_PUMP_P, HIGH);
    //digitalWrite(DOSE_PUMP_K, HIGH);



    
    routines::nutrientFilling();
    routines::waterFilling();
    routines::mixing();
 
    while (true)
    {
        while(!Serial.available());
        char data = Serial.read();
        while(Serial.available()>0) Serial.read();

        switch (data) 
        {
            case '1':
                routines::dosing();
                
                break;

                
            case '2':
                routines::irrigation();
                break;


            case '3':
                break;

            case '4':
                break;

            case '5':

                break;

            default:

                break; 
        }
    
    }
    

    /* Con esto probe el sistema de seguridad y el agitado
    if ((asd == 1)                   and
        !digitalRead(LIMIT_SWITCH_N) and
        !digitalRead(LIMIT_SWITCH_P) and
        !digitalRead(LIMIT_SWITCH_K))

    {  
        Serial.println("Batiendo");
        delay(500);
        y = routines::tryToMix();
        asd = 0;
        Serial.print(y);
        while(!Serial.available());
        asd = Serial.parseInt();
        while(Serial.available()>0) Serial.read();
    
    }

    */

    /*
     // Con esto probamos la dosificacion
    if (asd != 0){
    Serial.println("Dosificando");
    actualVolumeN = sensors::getNValuesFiltered(1, weigthValues);
    actualVolumeP = sensors::getNValuesFiltered(2, weigthValues);
    actualVolumeK = sensors::getNValuesFiltered(3, weigthValues);
    actualVolumeTank = sensors::getNValuesFiltered(4, weigthValues);
    
    volumeN = actualVolumeN - volumeToDose + 5;
    volumeP = actualVolumeP - volumeToDose + 5;
    volumeK = actualVolumeK - volumeToDose + 5;

    routines::doseToTankDebug(DOSE_PUMP_N,volumeN,scaleN);
    routines::doseToTankDebug(DOSE_PUMP_P,volumeP,scaleP);
    routines::doseToTankDebug(DOSE_PUMP_K,volumeK,scaleK);
    

    
    float debugVolumeTank = sensors::getNValuesMedian(4, weigthValues);


        while (true)
        {
            debugVolumeTank = sensors::getNValuesMedian(4, weigthValues);
            if (debugVolumeTank > (actualVolumeTank + volumeToDose))
            {
                break;
            }
        }
        digitalWrite(FILLING_VALVE_TANK,HIGH);
        digitalWrite(FILLING_PUMP,HIGH);
        Serial.print(digitalRead(FILLING_VALVE_TANK));
        Serial.print('\n'); 
        Serial.print(digitalRead(FILLING_PUMP));
        Serial.print('\n'); 
        while ((sensors::getScaleFiltered(4))<volumeToDoseDT)
        {
            Serial.println(sensors::getScaleFiltered(4));
        }
        digitalWrite(FILLING_PUMP,LOW);
        digitalWrite(FILLING_VALVE_TANK,LOW);
        Serial.print(digitalRead(FILLING_VALVE_TANK));
        Serial.print('\n'); 
        Serial.println(digitalRead(FILLING_PUMP));
        
        Serial.print('d');
        Serial.print('\n');
    
    
        
    }
    Serial.println("Dime");
    while(!Serial.available());
    asd = Serial.parseInt();
    volumeToDose = asd;
    while(Serial.available()>0) Serial.read();
    */
    

    /* Con esto probe la rutina de llenado
    if (asd == 1){
        
        
        digitalWrite(FILLING_VALVE_K,HIGH);
        digitalWrite(FILLING_VALVE_N,HIGH);
        digitalWrite(FILLING_VALVE_P,HIGH);
        digitalWrite(FILLING_VALVE_TANK,HIGH);
        digitalWrite(FILLING_PUMP,HIGH);
      

        Serial.print("Filling");
        routines::waterFilling();
        
    }
    
    while(!Serial.available());
    asd = Serial.parseInt();
    while(Serial.available()>0) Serial.read();
    
    if (asd == 2)
    {
        Serial.print("Apagando");
        digitalWrite(FILLING_PUMP,LOW);
        
        digitalWrite(FILLING_VALVE_N,LOW);
        digitalWrite(FILLING_VALVE_P,LOW);
        digitalWrite(FILLING_VALVE_K,LOW);
        digitalWrite(FILLING_VALVE_TANK,LOW);

    }
    
 */   
   
    
    

}