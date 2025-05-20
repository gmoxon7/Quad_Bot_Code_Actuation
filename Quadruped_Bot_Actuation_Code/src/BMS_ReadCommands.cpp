// BMS_ReadCommands.cpp
// --------------------
// Function definitions for reading data from the BMS (Battery Management System) module.
// Implements functions that read and convert raw register values to real-world voltages and temperatures.
//
// Author: Greg Moxon
// Organisation: Moxon Electronics
// Created: 2024-05-19
//
// This file is part of the Quad_Bot_Code_Actuation project.


#include "BMS_ReadCommands.h" // Include the header file for BMS read commands
#include "BMS_CoreCommands.h" // Include the header file for BMS core commands
#include "PinAssignments.h" // Include the header file for pin assignments

//be sure to integrate interrupts for the BMS chip to ensure that the data is not being read while the chip is in conversion mode.


//code to read cell voltages. 12bits across a range of 5V
float readVCell1() {
    uint16_t raw = readBMSData(0x49, 0x21);
    uint16_t cellBits = raw & 0x0FFF; // 12 bits: mask with 0b0000111111111111
    return (cellBits / 4095.0f) * 5.0f; // 5V range
}

float readVCell2() {
    uint16_t raw = readBMSData(0x49, 0x22);
    uint16_t cellBits = raw & 0x0FFF;
    return (cellBits / 4095.0f) * 5.0f;
}

float readVCell3() {
    uint16_t raw = readBMSData(0x49, 0x23);
    uint16_t cellBits = raw & 0x0FFF;
    return (cellBits / 4095.0f) * 5.0f;
}

float readVCell4() {
    uint16_t raw = readBMSData(0x49, 0x24);
    uint16_t cellBits = raw & 0x0FFF;
    return (cellBits / 4095.0f) * 5.0f;
}

float readVCell5() {
    uint16_t raw = readBMSData(0x49, 0x25);
    uint16_t cellBits = raw & 0x0FFF;
    return (cellBits / 4095.0f) * 5.0f;
}


float readVCellSum() {
    uint16_t raw = readBMSData(0x49, 0x26);
    uint16_t sumBits = raw & 0x7FFF; // 15 bits: mask with 0b0111111111111111
    return (sumBits / 32767.0f) * 25.0f; // 25V range
}



float readVB() {
    uint16_t raw = readBMSData(0x49, 0x27);
    uint16_t vbBits = raw & 0x0FFF; // 12 bits: mask with 0b0000111111111111
    return (vbBits / 4095.0f) * 25.0f; // 25V range
}


float readNTC_GPIO() {
    uint16_t raw = readBMSData(0x49, 0x28);
    uint16_t ntcBits = raw & 0x0FFF; // 12 bits: mask with 0b0000111111111111
    return (ntcBits / 4095.0f) * 3.3f; // 3.3V range
}

float readDieTemp() {
    uint16_t raw = readBMSData(0x49, 0x29);
    uint16_t dieBits = raw & 0x0FFF; // 12 bits: mask with 0b0000111111111111
    // Formula: T = 343.165 - 0.196 * DIE_TEMP_MEAS
    return 343.165f - 0.196f * dieBits;
}



float readCurrent() {
    uint16_t raw = readBMSData(0x49, 0x2C); 
    int16_t signedRaw = (int16_t)raw; // Interpret as signed 16-bit (two's complement)

    // Use your defined max voltage and sense resistor
    // Example: float voltageLimitRangeExt = 0.3f; // 300mV
    // Example: float senseResistor = 0.008f; // 8 mΩ

    float measuredVoltage = (signedRaw / 32767.0f) * voltageLimitRangeExt;
    float current = measuredVoltage / senseResistor;

    return current;
}


CoulombCountResult readCoulombCounter() {
    setBMSConversionState("CONVERSION_OFF");

    uint16_t msb = readBMSData(0x49, 0x2D);      // 16 MSB
    uint16_t lsb_and_count = readBMSData(0x49, 0x2E); // 8 MSB (accumulator), 8 LSB (sample count)

    uint32_t acc24 = ((uint32_t)msb << 8) | (lsb_and_count >> 8); // 24 bits
    // Sign-extend if negative (two's complement)
    if (acc24 & 0x800000) acc24 |= 0xFF000000;
    int32_t signedAcc = (int32_t)acc24;

    // Scaling: same as current reading, but for 24 bits
    // Max positive = 0x7FFFFF, max negative = -0x800000
    // Use voltageLimitRangeExt and senseResistor as in current reading
    float measuredVoltage = (signedAcc / 8388607.0f) * voltageLimitRangeExt; // 8388607 = 0x7FFFFF
    float coulombs = measuredVoltage / senseResistor;

    uint8_t sampleCount = lsb_and_count & 0xFF;

    setBMSConversionState("CONVERSION_ON");

    CoulombCountResult result;
    result.coulombs = coulombs;
    result.sampleCount = sampleCount;
    return result;
}

// Manufacturer Name (32-bit, from 0x17 MSB and 0x18 LSB)
uint32_t readManufacturerName() {
    uint16_t msb = readBMSData(0x49, 0x17);
    uint16_t lsb = readBMSData(0x49, 0x18);
    return ((uint32_t)msb << 16) | lsb;
}

// Manufacturer Date (16-bit, from 0x19)
uint16_t readManufacturerDate() {
    return readBMSData(0x49, 0x19);
}

// First Usage Date (16-bit, from 0x1A)
uint16_t readFirstUsageDate() {
    return readBMSData(0x49, 0x1A);
}

// Serial Number (32-bit, from 0x1B MSB and 0x1C LSB)
uint32_t readSerialNumber() {
    uint16_t msb = readBMSData(0x49, 0x1B);
    uint16_t lsb = readBMSData(0x49, 0x1C);
    return ((uint32_t)msb << 16) | lsb;
}

// Device Name (32-bit, from 0x1D MSB and 0x1E LSB)
uint32_t readDeviceName() {
    uint16_t msb = readBMSData(0x49, 0x1D);
    uint16_t lsb = readBMSData(0x49, 0x1E);
    return ((uint32_t)msb << 16) | lsb;
}





// scope to add diagnostic failure messages.