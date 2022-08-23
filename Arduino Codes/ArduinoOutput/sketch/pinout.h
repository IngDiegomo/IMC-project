#ifndef PINOUT_H
#define PINOUT_H
#include <Arduino.h>

// Pins for TDS sensors
#define TDS_PIN_N               A0
#define TDS_PIN_P               A1
#define TDS_PIN_K               A2

// Pins for weight scales
#define SCALE_DT_N              33
#define SCALE_SCK_N             35
#define SCALE_DT_P              29
#define SCALE_SCK_P             31
#define SCALE_DT_K              25
#define SCALE_SCK_K             27
#define SCALE_DT_TANK           37
#define SCALE_SCK_TANK          39

// Pins for lid limit switchs
#define LIMIT_SWITCH_N          41
#define LIMIT_SWITCH_P          43
#define LIMIT_SWITCH_K          18

// Pins for filling valves
#define FILLING_VALVE_N         A4
#define FILLING_VALVE_P         A5
#define FILLING_VALVE_K         20
#define FILLING_VALVE_TANK      21

// Pins for pumps
#define FILLING_PUMP            19
#define IRRIGATION_PUMP         22
#define AIR_PUMP                23
#define DOSE_PUMP_N             4
#define DOSE_PUMP_P             5
#define DOSE_PUMP_K             6

// Pins for mixing motors
#define MOTOR_N                 7
#define MOTOR_P                 8
#define MOTOR_K                 9

// Function for setting up all pin modes
void pinSetup();

#endif