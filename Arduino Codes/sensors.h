#ifndef SENSORS_H
#define SENSORS_H
#include "pinout.h"
#include "HX711.h"
#include "MedianFilterLib.h"
#include <EEPROM.h>
#include "GravityTDS.h"

// Calibration factors of each scale
#define SCALE_FACTOR_N 1516500.f/3194          
#define SCALE_FACTOR_P 1560800.f/3194
#define SCALE_FACTOR_K 1464520.f/3194
#define SCALE_FACTOR_TANK 275000.f/12156

// Memory adress where the calibration factor of each TDS is stored
#define K_VALUE_ADDRESS_N 1
#define K_VALUE_ADDRESS_P 8
#define K_VALUE_ADDRESS_K 15

// Assumed water temperature
#define TEMPERATURE 25

// Filter values
#define FILTER_VALUE 100
#define FILTER_VALUE_TANK 300


// Scales and tds sensors declarations
extern HX711 scaleN,scaleP,scaleK,scaleTank;
extern GravityTDS tdsN, tdsP, tdsK;


namespace sensors
{
    
    void initializeScales();                // Initializes the scales
    void initializeTds();                   // Initializes the tds sensors
    void scaleOn(HX711 &scale, bool power); // Turn on or turn off a scale
    float readTds(GravityTDS &tds);         // Read tds value of a tds sensor
    float getNValuesMedian(int scale, int n);  // Get the median filter of N values (1, 2, 3, 4 is N, P, K, Tank)
    float getScaleMedian(int n);               // Gets the median filtered value of a scale (1, 2, 3, 4 is N, P, K, Tank)
    float getScaleFiltered(int scale);         // Gets the filtered last value of a scale (1, 2, 3, 4 is N, P, K, Tank)
    float getNValuesFiltered(int scale, int n); // Gets the mean of N values of last filtered value of a scale (1, 2, 3, 4 is N, P, K, Tank)

}

#endif