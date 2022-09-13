#include "routines.h"
#include "pinout.h"

int y = 0;
int asd;
char startNutrientFilling;
unsigned long tiempomedir1;
unsigned long tiempomedir2;


void setup()
{
    pinSetup();
    communications::serialSetup();

    
    while(!Serial.available());
    //asd = Serial.parseInt();
    while(Serial.available()>0) Serial.read();
    //Serial.println("Iniciando");
    

    sensors::initializeScales();
    sensors::initializeTds();
    //sensors::scaleOn(scaleN,false);
    //sensors::scaleOn(scaleP,false);
    //sensors::scaleOn(scaleTank,false);

    

    
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
    communications::sendSensorInfo('3');
    //Serial.print(scaleK.get_units());
    //Serial.print('\n'); 

    /*
    
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
    */
    

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