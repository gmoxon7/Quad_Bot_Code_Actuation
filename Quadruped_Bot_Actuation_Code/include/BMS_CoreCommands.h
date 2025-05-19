// BMS_CoreCommands.h
// ------------------
// Function declarations for I2C communication with the L9961 BMS (Battery Management System) module from ST Microelectronics.
// Includes functions to read and write data to the BMS module using I2C communication.
//
// Author: Greg Moxon
// Organisation: Moxon Electronics
// Created: 2024-05-16
//
// This file is part of the Quad_Bot_Code_Actuation project.

#ifndef BMS_CORECOMMANDS_H
#define BMS_CORECOMMANDS_H

#include <Arduino.h>
#include <Wire.h> // Include the Wire library for I2C communication

extern int bmsConversionActive;


extern float senseResistor;
extern const float voltageLimitRangeExt;
extern float Imax;
extern float Vcur_res;
extern const float VCELL_RES;
extern const float VB_RES;
extern const float VNTC_RES;

// simple basic forms of functions.
uint16_t readBMSData(uint8_t chipAddress, uint8_t registerAddress);
void writeBMSData(uint8_t chipAddress, uint8_t registerAddress, uint16_t data);

//commands that use chars for the command name.
void setBMSConversionState(const char* state);






#endif // BMS_CORECOMMANDS_H