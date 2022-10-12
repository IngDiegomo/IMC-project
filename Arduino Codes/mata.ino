
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
char data = 20;

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


    
    routines::nutrientFilling();
    routines::waterFilling();
    routines::mixing();
 
    while (true)
    {
        while(!Serial.available());
        while ((data != '1') and
               (data != '2') and
               (data != '3') and
               (data != '4'))
                {
                    data = Serial.read();
                }
        switch (data) 
        {
            case '1':
                routines::dosing();
                
                break;

                
            case '2':
                routines::irrigation();
                break;


            case '3':
                routines::refillATank();

                break;

            case '4':
                routines::weightVerification();
                break;

            default:
                Serial.flush();

        }
    
    }

}
