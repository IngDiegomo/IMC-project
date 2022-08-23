#ifndef ROUTINES_H
#define ROUTINES_H
#include "communication.h"

#define TANKS_FILL_VALUE 500
#define MIXING_TIME_MS 60000


namespace routines
{

    void weightVerification();
    void nutrientFilling();
    void fillTankWater(HX711 &scale, uint8_t valve);
    void waterFilling();
    int mixing();
    void dosing(float waterWeightN, 
                float waterWeightP, 
                float waterWeightK);

}



#endif