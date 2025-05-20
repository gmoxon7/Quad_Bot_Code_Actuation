// BMS_SetupCommands.h
// -------------------
// Function declarations for BMS (Battery Management System) setup commands.
// Provides prototypes for sending identity and configuration commands to the BMS.
//
// Author: Greg Moxon
// Organisation: Moxon Electronics
// Created: 2024-05-16
//
// This file is part of the Quad_Bot_Code_Actuation project.



#pragma once

#ifndef BMS_SETUPCOMMANDS_H
#define BMS_SETUPCOMMANDS_H


#include <Arduino.h>
#include "BMS_CoreCommands.h"




void sendBMSIdentityCommand(const char* command, uint16_t data);
void sendBMSConfigCommand(const char* command, const char* valueStr);
void RWBMSNVM(const char* command);
void sendBMSRealTimeCommand(const char* command);



#endif // BMS_SETUPCOMMANDS_H