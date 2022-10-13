#include "routines.h"



float nutrientWeightN, nutrientWeightP, nutrientWeightK;
float waterLitersN, waterLitersP, waterLitersK;
float tankLevelN, tankLevelP, tankLevelK, tankLevelTank;
float tdsMixedN, tdsMixedP, tdsMixedK;
float weigthVerificationNValues = 20;
float conN, conP, conK;                                                 // Actual concentrations on the nutrient tanks

float nOvershoot = 5;
float pOvershoot = 10;
float kOvershoot = 15;

int gramsPerPlant = 5;

namespace routines
{

    void weightVerification()   // Get the a multiple value median to verify the weight the user poured into the nutrient tanks
    {
        float scaleReadings[3];
        scaleReadings[0] = sensors::getNValuesMedian(1, weigthVerificationNValues); // Get n value median of scale 1 (N scale)
        scaleReadings[1] = sensors::getNValuesMedian(2, weigthVerificationNValues); // Get n value median of scale 2 (P scale)
        scaleReadings[2] = sensors::getNValuesMedian(3, weigthVerificationNValues); // Get n value median of scale 1 (K scale)

        nutrientWeightN = scaleReadings[0];     // Store the weights in a global variable for later calculations
        nutrientWeightP = scaleReadings[1];
        nutrientWeightK = scaleReadings[2];
                
        for (int i = 0; i <3; i++)                  // Loop for sending the weights to the raspberry pi
        {
            Serial.print(scaleReadings[i]);     
            if (i < 2) Serial.print(',');          // Send them as a comma separated series of data
        }
        Serial.print('\n');                        // Send a line break to signalize end of message
    
    }
    

    void nutrientFilling()
    {
        int next;
        char startNutrientFilling;              // Variable to signal the start of nutrient filling routine
        
        next = 0;
        while(!Serial.available());
        while (startNutrientFilling != '1') startNutrientFilling = Serial.read();   // Loop until the variable is 1
        while(Serial.available() > 0) Serial.read();

        Serial.print('1');          // Send acknowledgement
        Serial.flush();

        while(next == 0)            // Loop broken by a value recieved from the raspberry pi
        {
            while(!Serial.available());
            char data = Serial.read();                  // Recieved value
            while(Serial.available()>0) Serial.read();
            switch (data)                               // Send a different sensor reading depending on the value recieved
            {
            case '1':                                   
                communications::sendSensorInfo(2);
                Serial.flush();
                data = 0;
                next = 0;
                break;
            
            case '2':
                weightVerification();
                Serial.flush();
                data = 0;
                next = 0;
                break;
            
            case '3':                   // End of routine value
                next = 1;
                data = 0;
                break;
    
            default:
                Serial.flush(); 
                data = 0;
                next = 0;

            }
        }
        //weightVerification();           // Run the weight verification
        Serial.flush();
    }

    void fillTankWater(int scale, uint8_t valve)        // Routine to fill a tank with water
    {
       float weight = 0;                               // Variable to keep track of the weight of the tank
       float wN, wP, wK, wT;
       
        weight = sensors::getScaleFiltered(scale);
        if (weight < TANKS_FILL_VALUE)                  // If the weight is lower than the filling value
        {
            digitalWrite(valve,HIGH);                   // Open the valve
            digitalWrite(FILLING_PUMP,HIGH);            // Turn on the filling pump

            while (weight < TANKS_FILL_VALUE)           // While the weight is lower than the filling value
            {
                weight = sensors::getScaleFiltered(scale);  
                wN = sensors::getScaleFiltered(1);
                wP = sensors::getScaleFiltered(2);
                wK = sensors::getScaleFiltered(3);
                wT = sensors::getScaleFiltered(4);
                wT = sensors::getScaleMedian(4);
                Serial.print(weight);                       // Send the weight scale reading through serial
                Serial.print('\n');
            }

            digitalWrite(FILLING_PUMP,LOW);                 // Turn off the filling pump
            digitalWrite(valve,LOW);                        // Close the filling valve
            delay(1000);                                    // Delay to ensure all serial values have been sent

        }

    }

    void waterFilling()
    {
        char startWaterFilling;              // Variable to signal the start of water filling routine
        
        while(!Serial.available());
        while (startWaterFilling != '1') startWaterFilling = Serial.read();   // Loop until the variable is 1
        while(Serial.available() > 0) Serial.read();

        Serial.print('1');          // Send acknowledgement
        Serial.flush();

        digitalWrite(FILLING_VALVE_TANK,LOW);

        Serial.print('n');                      // Send an 'n' meaning that the N tank is being filled
        Serial.print('\n');
        Serial.flush();
        fillTankWater(1,FILLING_VALVE_N);       // Fill the N tank
        
        Serial.print('p');                          
        Serial.print('\n');
        Serial.flush();
        fillTankWater(2,FILLING_VALVE_P);       // Fill the P tank
        
        Serial.print('k');
        Serial.print('\n');
        Serial.flush();
        fillTankWater(3,FILLING_VALVE_K);       // Fill the K tank
        
        Serial.print('d');                      // Send a 'd' meaning the filling sequence is done
        Serial.print('\n');
        Serial.flush();


        tankLevelN = sensors::getNValuesMedian(1, weigthVerificationNValues);   // Verification of the tank levels after filling
        tankLevelP = sensors::getNValuesMedian(2, weigthVerificationNValues);
        tankLevelK = sensors::getNValuesMedian(3, weigthVerificationNValues);
        
        waterLitersN = (tankLevelN - nutrientWeightN)/1000;         // Calculate the liters of water in the tank
        waterLitersP = (tankLevelP - nutrientWeightP)/1000;
        waterLitersK = (tankLevelK - nutrientWeightK)/1000;


        
        /*
        waterLitersN = (tankLevelN - gramsPerPlant)/1000;                     // Calculate the liters of water in the tank
        waterLitersP = (tankLevelP - gramsPerPlant)/1000;
        waterLitersK = (tankLevelK - gramsPerPlant)/1000;
        */

        
         // Calculate the nutrient concentration value in each tank, dependant on the nutrients poured
        conN = nutrientWeightN/waterLitersN;
        conP = nutrientWeightP/waterLitersP;
        conK = nutrientWeightK/waterLitersK;
        

        // Calculate the nutrient concentration value with a fixed value for grams per plant
        /*
        conN = gramsPerPlant/waterLitersN;
        conP = gramsPerPlant/waterLitersP;
        conK = gramsPerPlant/waterLitersK;
        */

        float tanksLevel[3] = {tankLevelN, tankLevelP, tankLevelK};
        for (int i = 0; i <3; i++)
        {
            Serial.print(tanksLevel[i],2);
            if (i < 2) Serial.print(',');           // Send them as a comma separated series of data
        }
        Serial.print('\n');                         // Send a line break to signalize end of message
        Serial.flush();


        unsigned long preFillTime;
        float wT;
        preFillTime = millis() + PREFILL_DELAY_MS;
        digitalWrite(DOSE_PUMP_N,HIGH);
        digitalWrite(DOSE_PUMP_P,HIGH);
        digitalWrite(DOSE_PUMP_K,HIGH);
        while(millis()<=preFillTime){
            wT = sensors::getScaleFiltered(1);
            wT = sensors::getScaleFiltered(2);
            wT = sensors::getScaleFiltered(3);
            wT = sensors::getScaleFiltered(4);
            wT = sensors::getScaleMedian(1);
            wT = sensors::getScaleMedian(2);
            wT = sensors::getScaleMedian(3);
            wT = sensors::getScaleMedian(4);
        }
        digitalWrite(DOSE_PUMP_N,LOW);
        digitalWrite(DOSE_PUMP_P,LOW);
        digitalWrite(DOSE_PUMP_K,LOW);
         wT = sensors::getScaleFiltered(4);



    }

    bool mixingInterrupted()                    // Indicates if the mixing has been interrupted
    {

        if (digitalRead(LIMIT_SWITCH_N) or      // If any lid is open or the emergency button is pressed
            digitalRead(LIMIT_SWITCH_P) or
            digitalRead(LIMIT_SWITCH_K) or 
            digitalRead(EMERGENCY))
            {
                return true;                    // Return true
            }
        else
        {
            return false;                       // Else return false
        }

    }

    int tryToMix()                                  // Function to try to mix
    {   
        unsigned long mixTime, motorsTime;          // Variables for time
        int interrupted, mixed;                     // Variables to indicate if the mixing is done or interrupted
        float wT;
        wT = sensors::getScaleFiltered(4);
        Serial.print('m');                          // Indicate that the mixing is going to start
        Serial.print('\n');
        Serial.flush();

        motorsTime = millis() + MOTOR_DELAY_MS;     // Delay the start of the motors
        while (millis() < motorsTime)               
        {
            if (mixingInterrupted())                // If the mixing is interrupted
            {
                digitalWrite(MOTOR_N,LOW);          // Turn off the motors
                digitalWrite(MOTOR_P,LOW);
                digitalWrite(MOTOR_K,LOW);
                Serial.print('d');                  // Send a d
                Serial.print('\n');
                Serial.flush();
                return false;                       // Return false
            }
        }
        digitalWrite(MOTOR_N,HIGH);                 // Turn on motor N

        motorsTime = millis() + MOTOR_DELAY_MS;
        while (millis() < motorsTime)               // Delay the start of the next motor
        {
            if (mixingInterrupted())                // Same as above
            {
                digitalWrite(MOTOR_N,LOW);
                digitalWrite(MOTOR_P,LOW);
                digitalWrite(MOTOR_K,LOW);
                Serial.print('d');
                Serial.print('\n');
                Serial.flush();
                return false;
            } 
        }

        digitalWrite(MOTOR_P,HIGH);                 // Turn on motor P
        motorsTime = millis() + MOTOR_DELAY_MS;
        while (millis() < motorsTime)               // Same as above
        {
            if (mixingInterrupted())
            {
                digitalWrite(MOTOR_N,LOW);
                digitalWrite(MOTOR_P,LOW);
                digitalWrite(MOTOR_K,LOW);
                Serial.print('d');
                Serial.print('\n');
                Serial.flush();
                return false;
            } 
        }

        digitalWrite(MOTOR_K,HIGH);             // Turn on motor K
        mixTime = millis() + MIXING_TIME_MS;
        while(millis()<mixTime)                 
        {
            if (mixingInterrupted())
            {
                digitalWrite(MOTOR_N,LOW);
                digitalWrite(MOTOR_P,LOW);
                digitalWrite(MOTOR_K,LOW);
                Serial.print('d');
                Serial.print('\n');
                Serial.flush();
                return false;
            }
        } 
        
        digitalWrite(MOTOR_N,LOW);          // Turn off the motors
        digitalWrite(MOTOR_P,LOW);
        digitalWrite(MOTOR_K,LOW);
        Serial.print('d');                  // Send done
        Serial.print('\n');
        Serial.flush();
        tdsMixedN = sensors::readTds(tdsN); // Read and store the values of the tds sensors
        tdsMixedP = sensors::readTds(tdsP);
        tdsMixedK = sensors::readTds(tdsK);
        return true;
    }
    

    void mixing()                                       // Mixing routine
    {
        while (true)
        {
            do 
            {
                delay(200);
                communications::sendSensorInfo(2);    // Send the lids states
                Serial.flush();
            }
            while (mixingInterrupted());                // While mixing is interrupted

            delay(200);
            communications::sendSensorInfo(2);        // Send the lids states
            Serial.flush();
    
            if (tryToMix())                             // Try to mix
            {
                Serial.print('e');                      // If mixing successful, send e
                Serial.print('\n');
                Serial.flush();
                communications::sendSensorInfo(1);    // Send the tds values
                Serial.flush();
                break; 
            }
            else
            {
                Serial.print('i');                      // If mixing interrupted, send i
                Serial.print('\n');
                Serial.flush();
        
            }
        }
        float debugLN, debugLP, debugLK;                                    // This is for debugging
        
        debugLN = sensors::getNValuesMedian(1, weigthVerificationNValues);
        debugLP = sensors::getNValuesMedian(2, weigthVerificationNValues);
        debugLK = sensors::getNValuesMedian(3, weigthVerificationNValues);

        //debugLN = (tankLevelN - gramsPerPlant)/1000;
        //debugLP = (tankLevelP - gramsPerPlant)/1000;
        //debugLK = (tankLevelK - gramsPerPlant)/1000;

        float debugLevels[3] = {debugLN, debugLP, debugLK};
        for (int i = 0; i <3; i++)
        {
            Serial.print(debugLevels[i],2);
            if (i < 2) Serial.print(',');           // Send them as a comma separated series of data
        }
        Serial.print('\n');                         // Send a line break to signalize end of message
        Serial.flush();

    }

    void doseToTank(uint8_t pump, float volume, int scale)      // This function doses untill a nutrient tank reaches certain volum
    {
        float wT;
        Serial.print('1');                                          // Signal the start of the dosing
        Serial.print('\n');
        Serial.println(volume);
        digitalWrite(pump,HIGH);                                    // Turn on the pump
        while ((sensors::getScaleFiltered(scale))>volume) {        // Wait until the tank volume reaches the specified value
            
            Serial.println(sensors::getScaleFiltered(scale));
            wT = sensors::getScaleFiltered(4);
            wT = sensors::getScaleMedian(4);
        }                                       
        digitalWrite(pump,LOW);                                     // Turn off the pump
        Serial.print('0');                            // Send the state of the pump
        Serial.print('\n');  

    }

    void doseToTankDebug(uint8_t pump, float volume, HX711 &scale)      // This is for debugging
    {
        digitalWrite(pump,HIGH);
        Serial.print(digitalRead(pump));
        Serial.print('\n');  
        while (scale.get_units()>volume) {}
        digitalWrite(pump,LOW);
        Serial.print(digitalRead(pump));
        Serial.print('\n');  

    }

    void dosing()                               // Dosing routine
    {
        float conTankN, conTankP, conTankK;     // Concentration values that the main tank must have
        float actualVolumeN, actualVolumeP, actualVolumeK, actualVolumeTank;    // Actual volume of the tanks
        float volumeToDoseN, volumeToDoseP, volumeToDoseK, volumeToDoseTank;    // Volumes to dose to the tanks
        float volumeN, volumeP, volumeK,volumeTank;                             // Volumes of the tanks after dosing
        float debugTLN, debugTLP, debugTLK;     // This is for debugging
        float nPlants;

        actualVolumeN = sensors::getNValuesMedian(1, weigthVerificationNValues);    // Get the values of the tank volumes
        actualVolumeP = sensors::getNValuesMedian(2, weigthVerificationNValues);
        actualVolumeK = sensors::getNValuesMedian(3, weigthVerificationNValues);
        actualVolumeTank = sensors::getNValuesMedian(4, weigthVerificationNValues);

        //Value dependant on the nutrients poured
        
        waterLitersN = (actualVolumeN - nutrientWeightN)/1000;                 // Water liters in the tanks
        waterLitersP = (actualVolumeP - nutrientWeightP)/1000;
        waterLitersK = (actualVolumeK - nutrientWeightK)/1000;
        
        // Value fixed
        /*
        waterLitersN = (actualVolumeN - gramsPerPlant)/1000;                 // Water liters in the tanks
        waterLitersP = (actualVolumeP - gramsPerPlant)/1000;
        waterLitersK = (actualVolumeK - gramsPerPlant)/1000;
        */
        // This is for debugging
        float debugTanksLevel[3] = {waterLitersN, waterLitersP, waterLitersK}; 
        for (int i = 0; i <3; i++)
        {
            Serial.print(debugTanksLevel[i],2);
            if (i < 2) Serial.print(',');           // Send them as a comma separated series of data
        }
        Serial.print('\n');                         // Send a line break to signalize end of message
        Serial.flush();

        conTankN = Serial.parseFloat();             // Recieve the concentration values for each nutrient tank
        conTankP = Serial.parseFloat();
        conTankK = Serial.parseFloat(); 
        nPlants  = Serial.parseFloat();

        float debugCons[4] = {conTankN, conTankP, conTankK, nPlants};    // This is for debugging
        for (int i = 0; i <4; i++)
        {
            Serial.print(debugCons[i],2);
            if (i < 3) Serial.print(',');           // Send them as a comma separated series of data
        }
        Serial.print('\n');                         // Send a line break to signalize end of message
        Serial.flush();

        // This is for debugging
        float debugConsPote[3] = {conN, conP, conK};
        for (int i = 0; i <3; i++)
        {
            Serial.print(debugConsPote[i],4);
            if (i < 2) Serial.print(',');           // Send them as a comma separated series of data
        }
        Serial.print('\n');                         // Send a line break to signalize end of message
        Serial.flush();

        // Calculate the volumes to dose in each tank, this is measured in grams of water
        volumeToDoseN = ((conTankN*nPlants*(1.27E+2/2.5E+2))/conN)*1000;
        volumeToDoseP = ((conTankP*nPlants*(1.27E+2/2.5E+2))/conP)*1000;
        volumeToDoseK = ((conTankK*nPlants*(1.27E+2/2.5E+2))/conK)*1000;
        volumeToDoseTank = ((nPlants*(-conK*conN*conP+conK*conN*conTankP
                       +conK*conP*conTankN+conN*conP
                       *conTankK)*(-1.27E+2/2.5E+2))
                       /(conK*conN*conP))*1000;

        // Calculate the volume each tank has to have after dosing, considering an overshoot
        volumeN = actualVolumeN - volumeToDoseN + nOvershoot;
        volumeP = actualVolumeP - volumeToDoseP + pOvershoot;
        volumeK = actualVolumeK - volumeToDoseK + kOvershoot;
        volumeTank = actualVolumeTank + volumeToDoseN + volumeToDoseP + volumeToDoseK + volumeToDoseTank;

        // This is for debugging
        float debugVolumes[4] = {volumeToDoseN, volumeToDoseP, volumeToDoseK, volumeTank};
        for (int i = 0; i <4; i++)
        {
            Serial.print(debugVolumes[i],2);
            if (i < 3) Serial.print(',');           // Send them as a comma separated series of data
        }
        Serial.print('\n');                         // Send a line break to signalize end of message
        Serial.flush();

        // Dose to each tank
        doseToTank(DOSE_PUMP_N,volumeN,1);
        doseToTank(DOSE_PUMP_P,volumeP,2);
        doseToTank(DOSE_PUMP_K,volumeK,3);

        float newVolumeTank = sensors::getNValuesMedian(4, weigthVerificationNValues);  // This variable is to verify that the main tank has been dosed to

        while (true)    // Stay in loop while the weight of the tank is less than the weight read before dosing plus the volume to dose of Nitrogen
        {
            newVolumeTank = sensors::getNValuesMedian(4, weigthVerificationNValues);
            if (newVolumeTank > (actualVolumeTank + volumeToDoseN))
            {
                break;
            }
        }
        digitalWrite(FILLING_VALVE_TANK,HIGH);          // Open the valve tank
        digitalWrite(FILLING_PUMP,HIGH);                // Turn on the filling pump
        Serial.print('1');  
        Serial.print('\n'); 
        while ((sensors::getScaleFiltered(4))<volumeTank)
        {
            Serial.println(sensors::getScaleFiltered(4));
            
        }
        digitalWrite(FILLING_PUMP,LOW);
        digitalWrite(FILLING_VALVE_TANK,LOW);
        Serial.print('0');
        Serial.print('\n'); 
        Serial.flush();
    
        Serial.print('d');
        Serial.print('\n');
        Serial.flush();
    

        tankLevelN = sensors::getNValuesMedian(1, weigthVerificationNValues);
        tankLevelP = sensors::getNValuesMedian(2, weigthVerificationNValues);
        tankLevelK = sensors::getNValuesMedian(3, weigthVerificationNValues);
        tankLevelTank = sensors::getNValuesMedian(4, weigthVerificationNValues);

        float tanksLevel[4] = {tankLevelN, tankLevelP, tankLevelK, tankLevelTank};
        for (int i = 0; i <4; i++)
        {
            Serial.print(tanksLevel[i],2);
            if (i < 3) Serial.print(',');           // Send them as a comma separated series of data
        }
        Serial.print('\n');
        Serial.flush();

        communications::sendSensorInfo(1);
        Serial.flush();
    
        return;
    }
    

    void irrigation()
    {
        char startIrrigation;              // Variable to signal the start of water filling routine
        
        while(!Serial.available());
        while (startIrrigation != '1') startIrrigation = Serial.read();   // Loop until the variable is 1
        while(Serial.available() > 0) Serial.read();

        Serial.print('1');          // Send acknowledgement
        Serial.flush();

        char data = 0;
        float temp;
        digitalWrite(IRRIGATION_PUMP,HIGH);
        digitalWrite(AIR_PUMP,HIGH);
        unsigned long dataSendTime;
        while(Serial.available()>0) Serial.read();
        dataSendTime = millis() + 5000;
        while (data != '1')
        {
            if (millis() >= dataSendTime)
            {
                dataSendTime += 5000;
                Serial.println(sensors::getScaleFiltered(4));
                temp = sensors::getScaleMedian(4);
            }
            if (Serial.available()) data = Serial.read();
        }
        digitalWrite(IRRIGATION_PUMP,LOW);
        digitalWrite(AIR_PUMP,LOW);
        Serial.println('x');
        return;
    }

    void refillATank()
    {
        char tankToRefill;              // Variable to signal the start of nutrient filling routine
        float refillReading = 0;
        char data;
        int next;

        while(!Serial.available());
        while (tankToRefill == 0) tankToRefill = Serial.read();   // Loop until the variable is different of 0
        while(Serial.available() > 0) Serial.read();

        Serial.print('1');          // Send acknowledgement
        Serial.flush();
        data = 0;
        next = 0;
        while(next == 0)            // Loop broken by a value recieved from the raspberry pi
        {
            while(!Serial.available());
            while (data == 0) data = Serial.read();                // Recieved value
            while(Serial.available()>0) Serial.read();
            switch (data)                               // Send a different sensor reading depending on the value recieved
            {
            case '1':                                   
                communications::sendSensorInfo(2);
                Serial.flush();
                data = 0;
                next = 0;
                break;
            
            case '2':
                refillReading = 0;
                if (tankToRefill == '1')
                {
                    refillReading= sensors::getNValuesMedian(1, weigthVerificationNValues); // Get n value median of scale 1 (K scale)
                    Serial.println(refillReading); 
                }
                else if (tankToRefill == '2')
                {
                    refillReading= sensors::getNValuesMedian(2, weigthVerificationNValues); // Get n value median of scale 1 (K scale)
                    Serial.println(refillReading); 
                }
                else if (tankToRefill == '3')
                {
                    refillReading= sensors::getNValuesMedian(3, weigthVerificationNValues); // Get n value median of scale 1 (K scale)
                    Serial.println(refillReading); 

                }
                data = 0;
                Serial.flush();
                next = 0;
                break;
            
            case '3':                   // End of routine value
                data = 0;
                next = 1;
                break;
    
            default:
                Serial.flush(); 

            }
        }
     

        switch(tankToRefill)
        {
            case '1':                                   
                fillTankWater(1,FILLING_VALVE_N);
                break;
            case '2':                                   
                fillTankWater(2,FILLING_VALVE_P);
                break;
            case '3':                                   
                fillTankWater(3,FILLING_VALVE_K);
                break;

            default:
                Serial.flush();

        }
        mixing();    





    }

    void dosingDemo()                               // Dosing routine
    {
        float conTankN, conTankP, conTankK;     // Concentration values that the main tank must have
        float actualVolumeN, actualVolumeP, actualVolumeK, actualVolumeTank;    // Actual volume of the tanks
        float volumeToDoseN, volumeToDoseP, volumeToDoseK, volumeToDoseTank;    // Volumes to dose to the tanks
        float volumeN, volumeP, volumeK,volumeTank;                             // Volumes of the tanks after dosing
        float debugTLN, debugTLP, debugTLK;     // This is for debugging
        float nPlants;

        actualVolumeN = sensors::getNValuesMedian(1, weigthVerificationNValues);    // Get the values of the tank volumes
        actualVolumeP = sensors::getNValuesMedian(2, weigthVerificationNValues);
        actualVolumeK = sensors::getNValuesMedian(3, weigthVerificationNValues);
        actualVolumeTank = sensors::getNValuesMedian(4, weigthVerificationNValues);

        //Value dependant on the nutrients poured
        
        waterLitersN = (actualVolumeN - nutrientWeightN)/1000;                 // Water liters in the tanks
        waterLitersP = (actualVolumeP - nutrientWeightP)/1000;
        waterLitersK = (actualVolumeK - nutrientWeightK)/1000;
        
        // Value fixed
        /*
        waterLitersN = (actualVolumeN - gramsPerPlant)/1000;                 // Water liters in the tanks
        waterLitersP = (actualVolumeP - gramsPerPlant)/1000;
        waterLitersK = (actualVolumeK - gramsPerPlant)/1000;
        */

        // This is for debugging
        float debugTanksLevel[3] = {waterLitersN, waterLitersP, waterLitersK}; 
        for (int i = 0; i <3; i++)
        {
            Serial.print(debugTanksLevel[i],2);
            if (i < 2) Serial.print(',');           // Send them as a comma separated series of data
        }
        Serial.print('\n');                         // Send a line break to signalize end of message
        Serial.flush();

        conTankN = Serial.parseFloat();             // Recieve the concentration values for each nutrient tank
        conTankP = Serial.parseFloat();
        conTankK = Serial.parseFloat(); 
        nPlants  = Serial.parseFloat();

        float debugCons[4] = {conTankN, conTankP, conTankK, nPlants};    // This is for debugging
        for (int i = 0; i <4; i++)
        {
            Serial.print(debugCons[i],2);
            if (i < 3) Serial.print(',');           // Send them as a comma separated series of data
        }
        Serial.print('\n');                         // Send a line break to signalize end of message
        Serial.flush();

        // This is for debugging
        float debugConsPote[3] = {conN, conP, conK};
        for (int i = 0; i <3; i++)
        {
            Serial.print(debugConsPote[i],4);
            if (i < 2) Serial.print(',');           // Send them as a comma separated series of data
        }
        Serial.print('\n');                         // Send a line break to signalize end of message
        Serial.flush();

        // Calculate the volumes to dose in each tank, this is measured in grams of water
        volumeToDoseN = ((conTankN*nPlants*(1.27E+2/2.5E+2))/conN)*1000;
        volumeToDoseP = ((conTankP*nPlants*(1.27E+2/2.5E+2))/conP)*1000;
        volumeToDoseK = ((conTankK*nPlants*(1.27E+2/2.5E+2))/conK)*1000;
        volumeToDoseTank = ((nPlants*(-conK*conN*conP+conK*conN*conTankP
                       +conK*conP*conTankN+conN*conP
                       *conTankK)*(-1.27E+2/2.5E+2))
                       /(conK*conN*conP))*1000;

        // Calculate the volume each tank has to have after dosing, considering an overshoot
        volumeN = actualVolumeN - volumeToDoseN + nOvershoot;
        volumeP = actualVolumeP - volumeToDoseP + pOvershoot;
        volumeK = actualVolumeK - volumeToDoseK + kOvershoot;
        volumeTank = actualVolumeTank + volumeToDoseN + volumeToDoseP + volumeToDoseK + volumeToDoseTank;

        // This is for debugging
        float debugVolumes[4] = {volumeToDoseN, volumeToDoseP, volumeToDoseK, volumeTank};
        for (int i = 0; i <4; i++)
        {
            Serial.print(debugVolumes[i],2);
            if (i < 3) Serial.print(',');           // Send them as a comma separated series of data
        }
        Serial.print('\n');                         // Send a line break to signalize end of message
        Serial.flush();

        // Dose to each tank
        doseToTank(DOSE_PUMP_N,volumeN,1);
        doseToTank(DOSE_PUMP_P,volumeP,2);
        doseToTank(DOSE_PUMP_K,volumeK,3);

        Serial.print('d');
        Serial.print('\n');
        Serial.flush();
    

        tankLevelN = sensors::getNValuesMedian(1, weigthVerificationNValues);
        tankLevelP = sensors::getNValuesMedian(2, weigthVerificationNValues);
        tankLevelK = sensors::getNValuesMedian(3, weigthVerificationNValues);
        tankLevelTank = sensors::getNValuesMedian(4, weigthVerificationNValues);

        float tanksLevel[4] = {tankLevelN, tankLevelP, tankLevelK, tankLevelTank};
        for (int i = 0; i <4; i++)
        {
            Serial.print(tanksLevel[i],2);
            if (i < 3) Serial.print(',');           // Send them as a comma separated series of data
        }
        Serial.print('\n');
        Serial.flush();

        communications::sendSensorInfo(1);
        Serial.flush();
    
        return;
    }

}

