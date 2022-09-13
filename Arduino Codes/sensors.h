#ifndef SENSORS_H
#define SENSORS_H
#include "pinout.h"
#include "HX711.h"
#include <EEPROM.h>
#include "GravityTDS.h"

// Calibration factors of each scale
#define SCALE_FACTOR_N 62750.f/136.11           //
#define SCALE_FACTOR_P 66180.f/136.11
#define SCALE_FACTOR_K 67330.f/136.11
#define SCALE_FACTOR_TANK 3100.f/136.11

// Memory adress where the calibration factor of each TDS is stored
#define K_VALUE_ADDRESS_N 1
#define K_VALUE_ADDRESS_P 8
#define K_VALUE_ADDRESS_K 15

// Assumed water temperature
#define TEMPERATURE 25

// Scales and tds sensors declarations
extern HX711 scaleN,scaleP,scaleK,scaleTank;
extern GravityTDS tdsN, tdsP, tdsK;

namespace sensors
{
    
    void initializeScales();                // Initializes the scales
    void initializeTds();                   // Initializes the tds sensors
    void scaleOn(HX711 &scale, bool power); // Turn on or turn off a scale
    float readTds(GravityTDS &tds);         // Read tds value of a tds sensor
    
}

#endif