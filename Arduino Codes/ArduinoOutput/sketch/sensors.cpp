#include "sensors.h"

HX711 scaleN,scaleP,scaleK,scaleTank;
GravityTDS tdsN, tdsP, tdsK;

namespace sensors
{
    void initializeScales()
    {
        scaleN.begin(SCALE_DT_N,SCALE_SCK_N);
        scaleP.begin(SCALE_DT_P,SCALE_SCK_P);
        scaleK.begin(SCALE_DT_K,SCALE_SCK_K);

        scaleN.set_scale(SCALE_FACTOR_N);
        scaleN.tare();
        scaleN.power_down();

        scaleP.set_scale(SCALE_FACTOR_P);
        scaleP.tare();
        scaleP.power_down();
        
        scaleK.set_scale(SCALE_FACTOR_K);
        scaleK.tare();
        scaleK.power_down();
    }

    void initializeTDS()
    {
        tdsN.setPin(TDS_PIN_N);
        tdsN.setAref(5.0);  
        tdsN.setAdcRange(1024);  
        tdsN.setKvalueAddress(K_VALUE_ADDRESS_N);  
        tdsN.begin();          
        
        tdsP.setPin(TDS_PIN_P);
        tdsP.setAref(5.0);  
        tdsP.setAdcRange(1024);  
        tdsP.setKvalueAddress(K_VALUE_ADDRESS_P);  
        tdsP.begin();  
        
        
        tdsK.setPin(TDS_PIN_K);
        tdsK.setAref(5.0);  
        tdsK.setAdcRange(1024);  
        tdsK.setKvalueAddress(K_VALUE_ADDRESS_K);  
        tdsK.begin();  
        
    }

    void scaleOn(HX711 &scale, bool power)
    {
        if (power) scale.power_up();
        else scale.power_down();
    }

   
    float readTds(GravityTDS &tds)
    {   
        float tdsValue;
        tds.setTemperature(TEMPERATURE);  
        tds.update();  
        tdsValue = tds.getTdsValue();  
       
        return tdsValue;
    }

}