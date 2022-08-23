#ifndef SENSORS_H
#define SENSORS_H
#include "pinout.h"
#include "HX711.h"
#include <EEPROM.h>
#include "GravityTDS.h"


#define SCALE_FACTOR_N 103900.f/212
#define SCALE_FACTOR_P 103900.f/212
#define SCALE_FACTOR_K 98150.f/212
#define SCALE_FACTOR_TANK 1

#define K_VALUE_ADDRESS_N 1
#define K_VALUE_ADDRESS_P 8
#define K_VALUE_ADDRESS_K 15

#define TEMPERATURE 25


extern HX711 scaleN,scaleP,scaleK,scaleTank;
extern GravityTDS tdsN, tdsP, tdsK;

namespace sensors
{
    
    void initializeScales();
    void initializeTDS();
    void scaleOn(HX711 &scale, bool power);
    float readTds(GravityTDS &tds);
    
}

#endif