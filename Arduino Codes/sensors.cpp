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
        float temp;
        switch (scale)                          // Switch the scale parameter (1, 2, 3, 4 is N, P, K, Tank)
        {
            case (1):
               
                for (int i = 0; i < n; i++)                         // For n values
                {
                    median = filterN.AddValue(scaleN.get_units());  // Add the reading to the median filter array
                    temp = getScaleFiltered(1);
                }
                return filterN.GetFiltered();                       // Return the resulting median
                break;
            
            case (2):
               
                for (int i = 0; i < n; i++)                         // Same as above
                {
                    median = filterP.AddValue(scaleP.get_units());
                    temp = getScaleFiltered(2);

                }
                return filterP.GetFiltered();
                break;
            
            case (3):
               
                for (int i = 0; i < n; i++)                         // Same as above
                {
                    median = filterK.AddValue(scaleK.get_units());
                    temp = getScaleFiltered(3);

                }
                return filterK.GetFiltered();
                break;
            
            case (4):
               
                for (int i = 0; i < n; i++)                                 // Same as above
                {
                    median = filterTank.AddValue(scaleTank.get_units());
                    temp = getScaleFiltered(4);

                }
                return filterTank.GetFiltered();
                break;
            
            default:
                Serial.flush();
               
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
            Serial.flush();
        }
    }
    float getScaleFiltered(int scale)
    {
        static float filteredN, filteredP, filteredK, filteredTank; // Satic values for each weight scale
        float valueRead;
        switch (scale)      // (1, 2, 3, 4 is N, P, K, Tank)
        {
            case (1):       
                valueRead = updateDataN(scaleN.get_units());     // Read the weightscale
                return valueRead;

            case (2):   // Same as above
                valueRead = updateDataP(scaleP.get_units()); 
                return valueRead;

            case (3):   // Same as above
                valueRead = updateDataK(scaleK.get_units());
                return valueRead; 
                break;

            case (4):   // Same as above but with different threshold 
                valueRead = updateDataTank(scaleTank.get_units()); 
                return valueRead;

            default:
                Serial.flush();      
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
                Serial.flush();
           
        }

    }

    float updateDataN(float newX)
    {
        static float lastReadedVal=0;
        static float x[5] = {0};
        static float y[5] = {0};
        float b[5] = {0.12866723, 0.12866723, 0, 0, 0};  //M
        float a[5] = {1, -0.74266554, 0, 0, 0}; //N
        float nextY;

        if(newX >0)
        {
            nextY = (b[0]*newX + b[1]*x[4] + b[2]*x[3] + b[3]*x[2]+ b[4]*x[1] - a[1]*y[4] - a[2]*y[3] - a[3]*y[2] - a[4]*y[1]);
            // left shift
            for (int i = 0; i < 4; i++)
            {
                x[i] = x[i + 1];
                y[i] = y[i + 1];
            }
            // agregar valores nuevos
            x[4] = newX;
            y[4] = nextY;
            lastReadedVal = nextY;
            return nextY;
        }
        else
        {
            return lastReadedVal;
        }
  }

      float updateDataP(float newX)
    {
        static float lastReadedVal=0;
        static float x[5] = {0};
        static float y[5] = {0};
        float b[5] = {0.12866723, 0.12866723, 0, 0, 0};  //M
        float a[5] = {1, -0.74266554, 0, 0, 0}; //N
        float nextY;

        if(newX >0)
        {
            nextY = (b[0]*newX + b[1]*x[4] + b[2]*x[3] + b[3]*x[2]+ b[4]*x[1] - a[1]*y[4] - a[2]*y[3] - a[3]*y[2] - a[4]*y[1]);
            // left shift
            for (int i = 0; i < 4; i++)
            {
                x[i] = x[i + 1];
                y[i] = y[i + 1];
            }
            // agregar valores nuevos
            x[4] = newX;
            y[4] = nextY;
            lastReadedVal = nextY;
            return nextY;
        }
        else
        {
            return lastReadedVal;
        }
    }

      float updateDataK(float newX)
    {
        static float lastReadedVal=0;
        static float x[5] = {0};
        static float y[5] = {0};
        float b[5] = {0.12866723, 0.12866723, 0, 0, 0};  //M
        float a[5] = {1, -0.74266554, 0, 0, 0}; //N
        float nextY;

        if(newX >0)
        {
            nextY = (b[0]*newX + b[1]*x[4] + b[2]*x[3] + b[3]*x[2]+ b[4]*x[1] - a[1]*y[4] - a[2]*y[3] - a[3]*y[2] - a[4]*y[1]);
            // left shift
            for (int i = 0; i < 4; i++)
            {
                x[i] = x[i + 1];
                y[i] = y[i + 1];
            }
            // agregar valores nuevos
            x[4] = newX;
            y[4] = nextY;
            lastReadedVal = nextY;
            return nextY;
        }
        else
        {
            return lastReadedVal;
        }
    }

      float updateDataTank(float newX)
    {
        static float lastReadedVal=0;
        static float x[5] = {0};
        static float y[5] = {0};
        float b[5] = {0.12866723, 0.12866723, 0, 0, 0};  //M
        float a[5] = {1, -0.74266554, 0, 0, 0}; //N
        float nextY;

        if(newX >0)
        {
            nextY = (b[0]*newX + b[1]*x[4] + b[2]*x[3] + b[3]*x[2]+ b[4]*x[1] - a[1]*y[4] - a[2]*y[3] - a[3]*y[2] - a[4]*y[1]);
            // left shift
            for (int i = 0; i < 4; i++)
            {
                x[i] = x[i + 1];
                y[i] = y[i + 1];
            }
            // agregar valores nuevos
            x[4] = newX;
            y[4] = nextY;
            lastReadedVal = nextY;
            return nextY;
        }
        else
        {
            return lastReadedVal;
        }
    }

}