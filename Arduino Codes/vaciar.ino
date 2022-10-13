/*
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
    asd = Serial.parseInt();
    //volumeToDose = asd;
    //volumeToDoseDT = 508 - asd*3;
    while(Serial.available()>0) Serial.read();
    
    

    //sensors::initializeScales();
    //sensors::initializeTds();
    //sensors::scaleOn(scaleN,false);
    //sensors::scaleOn(scaleP,false);
    //sensors::scaleOn(scaleTank,false);
    Serial.println("Prendiendo");

    
}

void loop()
{

    if (asd == 1){
        
        if (digitalRead(DOSE_PUMP_N))
        {
            digitalWrite(DOSE_PUMP_N, LOW);
        }
        else{
            digitalWrite(DOSE_PUMP_N, HIGH);
        }

    }

    
    if (asd == 2){
        
        if (digitalRead(DOSE_PUMP_P))
        {
            digitalWrite(DOSE_PUMP_P, LOW);
        }
        else{
            digitalWrite(DOSE_PUMP_P, HIGH);
        }

    }
    
    
    if (asd == 3){
        
        if (digitalRead(DOSE_PUMP_K))
        {
            digitalWrite(DOSE_PUMP_K, LOW);
        }
        else{
            digitalWrite(DOSE_PUMP_K, HIGH);
        }

    }
    Serial.println("Elija valvula");
    while(!Serial.available());
    asd = Serial.parseInt();
    while(Serial.available()>0) Serial.read();

}
*/