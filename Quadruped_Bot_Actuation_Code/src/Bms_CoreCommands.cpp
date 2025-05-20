// Bms_CoreCommands.cpp
// --------------------
// Implementation of I2C functions for reading and writing data to the L9961 BMS (Battery Management System) module.
// Includes functions for register access, conversion state control, and real-time BMS commands.
//
// There are some places where values need to be configured in this function. 
// cellStackSize, cellfilter, currentFilter, and senseResistor are all going to fit to your preferances.
//
//
//
// Author: Greg Moxon
// Organisation: Moxon Electronics
// Created: 2024-05-16
//
// This file is part of the Quad_Bot_Code_Actuation project.


#include "PinAssignments.h"
#include "BMS_CoreCommands.h" // Include the header file for BMS I2C functions

//set your current sense resistor here. 9mΩ is the default value for the L9961

//how many cells are you using? 3-5 cells are supported by the L9961.
int cellStackSize = 5; // set to your actual number of cells (3-5 for your system)
//use thes values to decide how long to filter voltage and current information. see datasheet CUR_FILTER and CELL_FILTER fields for details. 
uint8_t currentFilter = 3; // Range: 0 to 3, uses only two bits
uint8_t cellFilter = 3; // Range: 0 to 3, uses only two bits
uint8_t shortcircuitFilter = 3; // Range: 0 to 3, uses only two bits
uint8_t measureCycle =31; // range 0-31, this is the number of conversion cycles to average over. 31 is the maximum value and will give the best results.
// specific to your design. how big is the sense resistor your using
float senseResistor = 0.008f; // 8 mΩ = 0.008 Ω (can be changed elsewhere)



uint8_t cellMask = ((1 << cellStackSize) - 1) & 0b11111; // e.g., 3 cells: 0b00111, 5 cells: 0b11111 capped at 5

// Initialize cellFilterFloat based on cellFilter value, this is the number of microseconds for a conversion.
uint8_t cellFilter = 0; // Range: 0 to 3, uses only two bits

// Initialize cellFilterInt based on cellFilter value, in microseconds
uint32_t cellFilterInt = (cellFilter == 0) ? 800 :
                         (cellFilter == 1) ? 1310 :
                         (cellFilter == 2) ? 4380 :
                         (cellFilter == 3) ? 16670 : 800;
// Create a bitmask where each cell uses one bit, all set to 1 for the number of cells

// Calculate current filter float value (528 microseconds * 2^(3 + currentFilter)) 4.22ms to 33.79ms
uint32_t currentFilterInt = 528 * (1 << (3 + currentFilter)); // in microseconds
uint32_t measureCycleInt = 1000 * (1 << (3 + measureCycle)); // in microseconds


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






