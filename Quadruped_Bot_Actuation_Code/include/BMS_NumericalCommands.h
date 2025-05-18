// BMS_NumericalCommands.h
// -----------------------
// Function declarations for sending numerical commands to the BMS (Battery Management System) module.
// Provides prototypes for functions that handle register threshold and calibration commands.
//
// Author: Greg Moxon
// Organisation: Moxon Electronics
// Created: 2024-05-16
//
// This file is part of the Quad_Bot_Code_Actuation project.

#pragma once

#ifndef BMS_NUMERICALCOMMANDS_H
#define BMS_NUMERICALCOMMANDS_H
// BMS_NumericalCommands.h: This header file contains function declarations for sending numerical commands to a BMS (Battery Management System) module.


#include <Arduino.h>
#include "BMS_CoreCommands.h"




// Declare your numerical command function(s)
void sendBMSNumericalCommand(const char* command, const char* arg1, const char* arg2 = nullptr);


#endif // BMS_NUMERICALCOMMANDS_H