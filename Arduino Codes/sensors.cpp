#include "sensors.h"

// Definition of scale and tds sensor objects
HX711 scaleN,scaleP,scaleK,scaleTank;
GravityTDS tdsN, tdsP, tdsK;

namespace sensors
{
    void initializeScales()
    {
        // Initialize de scales
        scaleN.begin(SCALE_DT_N,SCALE_SCK_N);           
        scaleP.begin(SCALE_DT_P,SCALE_SCK_P);
        scaleK.begin(SCALE_DT_K,SCALE_SCK_K);
        scaleTank.begin(SCALE_DT_TANK,SCALE_SCK_TANK);
        
        // Set the scale factors and tare them 
        scaleN.set_scale(SCALE_FACTOR_N);
        scaleN.tare();
        
        scaleP.set_scale(SCALE_FACTOR_P);
        scaleP.tare();

        scaleK.set_scale(SCALE_FACTOR_K);
        scaleK.tare();

        scaleTank.set_scale(SCALE_FACTOR_TANK);
        scaleTank.tare();
        
    }

    void initializeTds()
    {
        tdsN.setPin(TDS_PIN_N);                     // Set the tds sensor pin
        tdsN.setAref(5.0);                          // Set reference voltage of ADC
        tdsN.setAdcRange(1024);                     // Set the ADC range
        tdsN.setKvalueAddress(K_VALUE_ADDRESS_N);   // Set the memory addres where the calibration value is stored
        tdsN.begin();                               // Initialize the tds sensor
        
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
        if (power) scale.power_up();            // if power is True, power up the scale
        else scale.power_down();                // else, power it down
    }

   
    float readTds(GravityTDS &tds)
    {   
        float tdsValue;
        tds.setTemperature(TEMPERATURE);     // Set the assumed temperature of the water 
        tds.update();                       // update the sensor
        tdsValue = tds.getTdsValue();      // get the tds value
       
        return tdsValue;
    }

}