#ifndef ROUTINES_H
#define ROUTINES_H
#include "communication.h"

#define TANKS_FILL_VALUE 3090*0.60
#define MIXING_TIME_MS 20000
#define MOTOR_DELAY_MS 1000
#define PREFILL_DELAY_MS 2000
#define VOLUME_PER_PLANT 0.508
#define GRAMS_PER_PLANT 5 

extern float nutrientWeightN, nutrientWeightP, nutrientWeightK;
extern float waterLitersN, waterLitersP, waterLitersK;
extern float tdsMixedN, tdsMixedP, tdsMixedK;

namespace routines
{

    void weightVerification();
    void nutrientFilling();
    void fillTankWater(int scale, uint8_t valve);
    void waterFilling();
    int tryToMix();
    void mixing();
    void doseToTank(uint8_t pump, float volume, int scale);
    void dosing();
    void irrigation();
    void doseToTankDebug(uint8_t pump, float volume, HX711 &scale);
    bool mixingInterrupted();
    void refillATank();
    void dosingDemo();

}



#endif