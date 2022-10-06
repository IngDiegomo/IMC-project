#include "sensors.h"

// Definition of scale and tds sensor objects
HX711 scaleN,scaleP,scaleK,scaleTank;
GravityTDS tdsN, tdsP, tdsK;

// Objects for median filter values
MedianFilter<float> filterN(13);
MedianFilter<float> filterP(13);
MedianFilter<float> filterK(13);
MedianFilter<float> filterTank(13);

// Value to fill the median filter on initialization
int nValuesToFillMedian = 15;

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
        
        // Fill the median filter with n values
        getNValuesMedian(1, nValuesToFillMedian);
        getNValuesMedian(2, nValuesToFillMedian);
        getNValuesMedian(3, nValuesToFillMedian);
        getNValuesMedian(4, nValuesToFillMedian);


    }

    void initializeTds()
    {
        tdsN.setPin(TDS_PIN_N);                     // Set the tds sensor pin
        tdsN.setAref(5.0);                          // Set reference voltage of ADC
        tdsN.setAdcRange(1024);                     // Set the ADC range
        tdsN.setKvalueAddress(K_VALUE_ADDRESS_N);   // Set the memory addres where the calibration value is stored
        tdsN.begin();                               // Initialize the tds sensor
        
        tdsP.setPin(TDS_PIN_P);                     // Same as above
        tdsP.setAref(5.0);  
        tdsP.setAdcRange(1024);  
        tdsP.setKvalueAddress(K_VALUE_ADDRESS_P);  
        tdsP.begin();  
        
        
        tdsK.setPin(TDS_PIN_K);                     // Same as above
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

    float getNValuesMedian(int scale, int n)
    {
        float median;                           // Value to store the median
        switch (scale)                          // Switch the scale parameter (1, 2, 3, 4 is N, P, K, Tank)
        {
            case (1):
               
                for (int i = 0; i < n; i++)                         // For n values
                {
                    median = filterN.AddValue(scaleN.get_units());  // Add the reading to the median filter array

                }
                return filterN.GetFiltered();                       // Return the resulting median
                break;
            
            case (2):
               
                for (int i = 0; i < n; i++)                         // Same as above
                {
                    median = filterP.AddValue(scaleP.get_units());

                }
                return filterP.GetFiltered();
                break;
            
            case (3):
               
                for (int i = 0; i < n; i++)                         // Same as above
                {
                    median = filterK.AddValue(scaleK.get_units());

                }
                return filterK.GetFiltered();
                break;
            
            case (4):
               
                for (int i = 0; i < n; i++)                                 // Same as above
                {
                    median = filterTank.AddValue(scaleTank.get_units());

                }
                return filterTank.GetFiltered();
                break;
            
            default:
                break;
        }
    }
    

    float getScaleMedian(int scale)
    {
        switch (scale)                                          //(1, 2, 3, 4 is N, P, K, Tank)
        {
            case (1):
                return filterN.AddValue(scaleN.get_units());    // Add the reading of a scale to the median filter array
                break;
            
            case (2):
                return filterP.AddValue(scaleP.get_units());    // Same as above
                break;

            case (3):
                return filterK.AddValue(scaleK.get_units());    // Same as above
                break;
            
            case (4):
                return filterTank.AddValue(scaleTank.get_units());  // Same as above
                break;
    
            default:
                break;
        }
    }
    float getScaleFiltered(int scale)
    {
        static float filteredN, filteredP, filteredK, filteredTank; // Satic values for each weight scale
        float valueRead;
        switch (scale)      // (1, 2, 3, 4 is N, P, K, Tank)
        {
            case (1):       
                valueRead = scaleN.get_units();     // Read the weightscale
                if ((valueRead > (filteredN + FILTER_VALUE)) or valueRead < (filteredN - FILTER_VALUE) )   // If the value read is above or below the last value read +- certain threshold
                {
                    return filteredN;   // Return the last value read
                }
                else
                {
                    filteredN = valueRead;  // Else, update the last value read with this reading and return it
                    return filteredN;
                } 
                
                break;

            case (2):   // Same as above
                valueRead = scaleP.get_units(); 
                if ((valueRead > (filteredP + FILTER_VALUE)) or valueRead < (filteredP - FILTER_VALUE) )
                {
                    return filteredP;
                }
                else
                {
                    filteredP = valueRead;
                    return filteredP;
                } 
                
                break;

            case (3):   // Same as above
                valueRead = scaleK.get_units(); 
                if ((valueRead > (filteredK + FILTER_VALUE)) or valueRead < (filteredK - FILTER_VALUE) )
                {
                    return filteredK;
                }
                else
                {
                    filteredK = valueRead;
                    return filteredK;
                } 
                
                break;

            case (4):   // Same as above but with different threshold 
                valueRead = scaleTank.get_units(); 
                if ((valueRead > (filteredTank + FILTER_VALUE_TANK)) or valueRead < (filteredTank - FILTER_VALUE_TANK) )
                {
                    return filteredTank;
                }
                else
                {
                    filteredTank = valueRead;
                    return filteredTank;
                } 
                
                break;

            default:
                break;
        }


    }
    float getNValuesFiltered(int scale, int n)
    {
        float total;
        switch (scale)  // (1, 2, 3, 4 is N, P, K, Tank)
        {
            case (1):
               
                for (int i = 0; i < n; i++)         // Loop with n iterations
                {
                    total += getScaleFiltered(1);   // Add the filtered value to the total
                }
                return total/n;                     // Return the mean of n values
                break;
            
            case (2):                               // Same as above
               
                for (int i = 0; i < n; i++)
                {
                    total += getScaleFiltered(2);
                }
                return total/n;
                break;
            
            case (3):                           // Same as above
               
                for (int i = 0; i < n; i++)
                {
                    total += getScaleFiltered(3);
                }
                return total/n;
                break;
            
            case (4):                           // Same as above
               
                for (int i = 0; i < n; i++)
                {
                    total += getScaleFiltered(4);
                }
                return total/n;
                break;
            
            default:
                break;
        }

    }
}