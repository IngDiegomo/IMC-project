#ifndef ROUTINES_H
#define ROUTINES_H
#include "communication.h"

#define TANKS_FILL_VALUE 3090*0.65
#define MIXING_TIME_MS 60000
#define VOLUME_PER_PLANT 0.508
#define GRAMS_PER_PLANT 5 

extern float nutrientWeightN, nutrientWeightP, nutrientWeightK;
extern float waterLitersN, waterLitersP, waterLitersK;
extern float tdsMixedN, tdsMixedP, tdsMixedK;

namespace routines
{

    void weightVerification();
    void nutrientFilling();
    void fillTankWater(HX711 &scale, uint8_t valve);
    void waterFilling();
    int tryToMix();
    void mixing();
    void doseToTank();
    void dosing();
    void irrigation();
    
}



#endif