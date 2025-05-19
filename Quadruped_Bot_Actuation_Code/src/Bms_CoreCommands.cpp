// Bms_CoreCommands.cpp
// --------------------
// Implementation of I2C functions for reading and writing data to the L9961 BMS (Battery Management System) module.
// Includes functions for register access, conversion state control, and real-time BMS commands.
//
// Author: Greg Moxon
// Organisation: Moxon Electronics
// Created: 2024-05-16
//
// This file is part of the Quad_Bot_Code_Actuation project.


#include "PinAssignments.h"
#include "BMS_CoreCommands.h" // Include the header file for BMS I2C functions

//set your current sense resistor here. 9mΩ is the default value for the L9961
float senseResistor = 0.008f; // 8 mΩ = 0.008 Ω (can be changed elsewhere)
const float voltageLimitRangeExt = 0.300f; // 300mV as per datasheet

float Imax = voltageLimitRangeExt / senseResistor;
float Vcur_res = voltageLimitRangeExt / 32767.0f; // 32767 = max positive value for 16-bit signed integer

const float VCELL_RES = 0.00122f; // Example value, check your datasheet
const float VB_RES = 0.0061f; // 6.1mV
const float VNTC_RES = 0.000806f; // 0.806mV

int bmsConversionActive = 0; // 1 = ON, 0 = OFF This variable is used in the config and identity functions to keep track of if the measuring registers are changing.

// Single Read Function for L9961
uint16_t readBMSData(uint8_t chipAddress, uint8_t registerAddress) {
    uint16_t data = 0;

    // Step 1: Send the slave address with the write bit and the register address
    Wire1.beginTransmission(chipAddress);
    Wire1.write(registerAddress);
    Wire1.endTransmission(true); // Send a repeated start condition
    delayMicroseconds(3); // Small delay to allow the device to process the request (tbuf)
    // Step 2: Send the slave address with the read bit
    Wire1.requestFrom(chipAddress, (uint8_t)2, true); // Request 2 bytes from the slave

    // Step 3: Read the two bytes of data
    if (Wire1.available() >= 2) {
        uint8_t highByte = Wire1.read(); // Read the first byte (MSB)
        uint8_t lowByte = Wire1.read();  // Read the second byte (LSB)
        data = (highByte << 8) | lowByte; // Combine the two bytes into a 16-bit value
    }

    return data;
}


// Function to write data to the BMS module
void writeBMSData(uint8_t chipAddress, uint8_t registerAddress, uint16_t data) {
    // Step 1: Start communication with the slave device
    Wire1.beginTransmission(chipAddress);

    // Step 2: Send the register address
    Wire1.write(registerAddress);

    // Step 3: Send the two data bytes (MSB first, then LSB)
    uint8_t highByte = (data >> 8) & 0xFF; // Extract the most significant byte
    uint8_t lowByte = data & 0xFF;        // Extract the least significant byte
    Wire1.write(highByte);                // Send the MSB
    Wire1.write(lowByte);                 // Send the LSB

    // Step 4: End the transmission (send a stop condition)
    if (Wire1.endTransmission(true) != 0) { // Check for errors
        Serial.println("Error: Failed to write data to BMS.");
    } else {
        Serial.println("Write successful.");
    }
}




//function used whenever a configuration is set. this stops registers changing while been written.
void setBMSConversionState(const char* state) {
    if (strcmp(state, "CONVERSION_OFF") == 0) {
        writeBMSData(0x49, 0x02, 0x0000); // Conversion off: register 0x02, data 0x0000
        bmsConversionActive = 0;
        Serial.println("Conversion OFF command sent.");
    } else if (strcmp(state, "CONVERSION_ON") == 0) {
        writeBMSData(0x49, 0x02, 0x0FFF); // Conversion on: register 0x02, data 0x0FFF conversions occur every 310ms
        bmsConversionActive = 1;
        Serial.println("Conversion ON command sent.");
    } else {
        Serial.println("Unknown conversion state command.");
    }
}






