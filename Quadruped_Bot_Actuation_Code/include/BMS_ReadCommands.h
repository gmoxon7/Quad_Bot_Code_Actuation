// BMS_ReadCommands.h
// ------------------
// Function declarations for reading data from the BMS (Battery Management System) module.
// Provides prototypes for functions that read and convert raw register values to real-world voltages and temperatures.
//
// Author: Greg Moxon
// Organisation: Moxon Electronics
// Created: 2024-05-19
//
// This file is part of the Quad_Bot_Code_Actuation project.

#pragma once

#ifndef BMS_READCOMMANDS_H
#define BMS_READCOMMANDS_H

#include "Arduino.h"
#include "BMS_CoreCommands.h"

// Read individual cell voltages
float readVCell1();
float readVCell2();
float readVCell3();
float readVCell4();
float readVCell5();

// Read sum of cell voltages
float readVCellSum();

// Read battery block voltage
float readVB();

// Read NTC thermistor or GPIO voltage
float readNTC_GPIO();

// Read die temperature
float readDieTemp();

// Read current (signed, in Amps)
float readCurrent();

// Coulomb counter result struct
struct CoulombCountResult {
    float coulombs;
    uint8_t sampleCount;
    float totalCoulombs;    // Total coulombs counted (global)
    unsigned long totalSampleCount; // Total sample count (global)
};

// Read coulomb counter (returns struct)
CoulombCountResult readCoulombCounter();

// Manufacturer data reads
uint32_t readManufacturerName();
uint16_t readManufacturerDate();
uint16_t readFirstUsageDate();
uint32_t readSerialNumber();
uint32_t readDeviceName();

#endif // BMS_READCOMMANDS_H