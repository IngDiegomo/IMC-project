#include "pinout.h"

void pinSetup()
{
    // Set emergency as input
    pinMode(EMERGENCY, INPUT);

    // Set tds as inputs
    pinMode(TDS_PIN_N,INPUT);
    pinMode(TDS_PIN_P,INPUT);
    pinMode(TDS_PIN_K,INPUT);

    // Set limit switchs as inputs
    pinMode(LIMIT_SWITCH_N,INPUT_PULLUP);
    pinMode(LIMIT_SWITCH_P,INPUT_PULLUP);
    pinMode(LIMIT_SWITCH_K,INPUT_PULLUP);

    // Set valves as outputs and turn them off
    pinMode(FILLING_VALVE_N,OUTPUT);
    pinMode(FILLING_VALVE_P,OUTPUT);
    pinMode(FILLING_VALVE_K,OUTPUT);
    pinMode(FILLING_VALVE_TANK,OUTPUT);

    digitalWrite(FILLING_VALVE_N,LOW);
    digitalWrite(FILLING_VALVE_P,LOW);
    digitalWrite(FILLING_VALVE_K,LOW);
    digitalWrite(FILLING_VALVE_TANK,LOW);

    // Set pumps as outputs and turn them off
    pinMode(FILLING_PUMP,OUTPUT);
    pinMode(IRRIGATION_PUMP,OUTPUT);
    pinMode(AIR_PUMP,OUTPUT);
    pinMode(DOSE_PUMP_N,OUTPUT);
    pinMode(DOSE_PUMP_P,OUTPUT);
    pinMode(DOSE_PUMP_K,OUTPUT);

    digitalWrite(FILLING_PUMP,LOW);
    digitalWrite(IRRIGATION_PUMP,LOW);
    digitalWrite(AIR_PUMP,LOW);
    digitalWrite(DOSE_PUMP_N,LOW);
    digitalWrite(DOSE_PUMP_P,LOW);
    digitalWrite(DOSE_PUMP_K,LOW);

    
    // Set motors as outputs and turn them off
    pinMode(MOTOR_N,OUTPUT);
    pinMode(MOTOR_P,OUTPUT);
    pinMode(MOTOR_K,OUTPUT);

    digitalWrite(MOTOR_N,LOW);
    digitalWrite(MOTOR_P,LOW);
    digitalWrite(MOTOR_K,LOW);

    
}