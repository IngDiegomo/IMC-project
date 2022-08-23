#include "pinout.h"

void pinSetup()
{
    // Set tds as inputs
    pinMode(TDS_PIN_N,INPUT);
    pinMode(TDS_PIN_P,INPUT);
    pinMode(TDS_PIN_K,INPUT);

    // Set limit switchs as inputs
    pinMode(LIMIT_SWITCH_N,INPUT_PULLUP);
    pinMode(LIMIT_SWITCH_P,INPUT_PULLUP);
    pinMode(LIMIT_SWITCH_K,INPUT_PULLUP);

    // Set valves as outputs and turn them off
    digitalWrite(FILLING_VALVE_N,LOW);
    digitalWrite(FILLING_VALVE_P,LOW);
    digitalWrite(FILLING_VALVE_K,LOW);
    digitalWrite(FILLING_VALVE_TANK,LOW);

    pinMode(FILLING_VALVE_N,OUTPUT);
    pinMode(FILLING_VALVE_P,OUTPUT);
    pinMode(FILLING_VALVE_K,OUTPUT);
    pinMode(FILLING_VALVE_TANK,OUTPUT);

    // Set pumps as outputs and turn them off
    digitalWrite(FILLING_PUMP,LOW);
    digitalWrite(IRRIGATION_PUMP,LOW);
    digitalWrite(AIR_PUMP,LOW);
    digitalWrite(DOSE_PUMP_N,LOW);
    digitalWrite(DOSE_PUMP_P,LOW);
    digitalWrite(DOSE_PUMP_K,LOW);

    pinMode(FILLING_PUMP,OUTPUT);
    pinMode(IRRIGATION_PUMP,OUTPUT);
    pinMode(AIR_PUMP,OUTPUT);
    pinMode(DOSE_PUMP_N,OUTPUT);
    pinMode(DOSE_PUMP_P,OUTPUT);
    pinMode(DOSE_PUMP_K,OUTPUT);
    
    // Set motors as outputs and turn them off
    digitalWrite(MOTOR_N,LOW);
    digitalWrite(MOTOR_P,LOW);
    digitalWrite(MOTOR_K,LOW);

    pinMode(MOTOR_N,OUTPUT);
    pinMode(MOTOR_P,OUTPUT);
    pinMode(MOTOR_K,OUTPUT);
    
}